#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <random>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <chrono>

using Complex = std::complex<double>;

// Helper function to convert strings to uppercase for case-insensitive commands
std::string toUpper(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });
    return str;
}

class QubitRegister {
private:
    int numQubits;
    int numStates;
    std::vector<Complex> stateVector;

public:
    QubitRegister(int n = 1) {
        init(n);
    }

    void init(int n) {
        numQubits = n;
        numStates = 1 << n;
        stateVector.assign(numStates, Complex(0.0, 0.0));
        stateVector[0] = Complex(1.0, 0.0);
    }

    void printState() const {
        std::cout << "State Vector (" << numQubits << " Qubits, " << numStates << " States):" << std::endl;
        for (int i = 0; i < numStates; ++i) {
            double prob = std::norm(stateVector[i]);
            if (prob > 1e-6) {
                std::cout << "  |";
                for (int b = numQubits - 1; b >= 0; --b) {
                    std::cout << ((i >> b) & 1);
                }
                std::cout << ">: " << stateVector[i] << " (Prob: " << std::fixed << std::setprecision(1) << prob * 100 << "%)" << std::endl;
            }
        }
        std::cout << "-----------------------------------" << std::endl;
    }

    // Hadamard Gate (Superposition)
    void applyHadamard(int targetQubit) {
        if (targetQubit < 0 || targetQubit >= numQubits) {
            std::cout << "[ERROR] Qubit index out of range." << std::endl;
            return;
        }
        double invSqrt2 = 1.0 / std::sqrt(2.0);
        int bitMask = 1 << targetQubit;

        for (int i = 0; i < numStates; ++i) {
            if ((i & bitMask) == 0) {
                int pairIndex = i | bitMask;
                Complex u = stateVector[i];
                Complex v = stateVector[pairIndex];

                stateVector[i] = (u + v) * invSqrt2;
                stateVector[pairIndex] = (u - v) * invSqrt2;
            }
        }
    }

    // Pauli-X Gate (Bit-Flip / NOT)
    void applyPauliX(int targetQubit) {
        if (targetQubit < 0 || targetQubit >= numQubits) {
            std::cout << "[ERROR] Qubit index out of range." << std::endl;
            return;
        }
        int bitMask = 1 << targetQubit;

        for (int i = 0; i < numStates; ++i) {
            if ((i & bitMask) == 0) {
                int pairIndex = i | bitMask;
                std::swap(stateVector[i], stateVector[pairIndex]);
            }
        }
    }

    // Pauli-Z Gate (Phase Flip)
    void applyPauliZ(int targetQubit) {
        if (targetQubit < 0 || targetQubit >= numQubits) {
            std::cout << "[ERROR] Qubit index out of range." << std::endl;
            return;
        }
        int bitMask = 1 << targetQubit;

        for (int i = 0; i < numStates; ++i) {
            if ((i & bitMask) != 0) {
                stateVector[i] *= Complex(-1.0, 0.0);
            }
        }
    }

    // Controlled-NOT Gate (Entanglement)
    void applyCNOT(int controlQubit, int targetQubit) {
        if (controlQubit < 0 || controlQubit >= numQubits || targetQubit < 0 || targetQubit >= numQubits) {
            std::cout << "[ERROR] Qubit index out of range." << std::endl;
            return;
        }
        int controlMask = 1 << controlQubit;
        int targetMask = 1 << targetQubit;

        for (int i = 0; i < numStates; ++i) {
            if (((i & controlMask) != 0) && ((i & targetMask) == 0)) {
                int pairIndex = i | targetMask;
                std::swap(stateVector[i], stateVector[pairIndex]);
            }
        }
    }

    // SWAP Gate (State Exchange)
    void applySWAP(int q1, int q2) {
        if (q1 < 0 || q1 >= numQubits || q2 < 0 || q2 >= numQubits) {
            std::cout << "[ERROR] Qubit index out of range." << std::endl;
            return;
        }
        if (q1 == q2) return;

        applyCNOT(q1, q2);
        applyCNOT(q2, q1);
        applyCNOT(q1, q2);
    }

    // Measurement & Wavefunction Collapse
    void measureAll() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        double r = dist(gen);

        double cumulativeProb = 0.0;
        int measuredIndex = 0;

        for (int i = 0; i < numStates; ++i) {
            cumulativeProb += std::norm(stateVector[i]);
            if (r <= cumulativeProb) {
                measuredIndex = i;
                break;
            }
        }

        for (int i = 0; i < numStates; ++i) {
            stateVector[i] = (i == measuredIndex) ? Complex(1.0, 0.0) : Complex(0.0, 0.0);
        }

        std::cout << ">>> Measured System Result: |";
        for (int b = numQubits - 1; b >= 0; --b) {
            std::cout << ((measuredIndex >> b) & 1);
        }
        std::cout << "> <<<\n" << std::endl;
    }

    // Execute script file directly from disk (.ztron)
    void runScript(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "[ERROR] Could not open file: " << filename << std::endl;
            return;
        }

        std::cout << "[RUNNING SCRIPT: " << filename << "]" << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty() && line[0] != '#') {
                std::cout << "> " << line << std::endl;
                executeInstruction(line);
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::cout << "[SCRIPT EXECUTION COMPLETE - " << std::fixed << std::setprecision(2) << duration.count() << " ms]" << std::endl;
    }

    bool executeInstruction(const std::string& line) {
        if (line.empty() || line[0] == '#') return true;

        std::stringstream ss(line);
        std::string rawOp;
        ss >> rawOp;

        std::string op = toUpper(rawOp);

        if (op == "INIT") {
            int n;
            if (ss >> n) {
                init(n);
                std::cout << "[EXEC] Initialized " << n << "-qubit register." << std::endl;
            }
            else {
                std::cout << "[USAGE] INIT <num_qubits>" << std::endl;
            }
        }
        else if (op == "H") {
            int target;
            if (ss >> target) {
                applyHadamard(target);
                std::cout << "[EXEC] Applied Hadamard Gate to Qubit " << target << std::endl;
            }
            else {
                std::cout << "[USAGE] H <target_qubit>" << std::endl;
            }
        }
        else if (op == "X" || op == "NOT") {
            int target;
            if (ss >> target) {
                applyPauliX(target);
                std::cout << "[EXEC] Applied Pauli-X (NOT) Gate to Qubit " << target << std::endl;
            }
            else {
                std::cout << "[USAGE] X <target_qubit>" << std::endl;
            }
        }
        else if (op == "Z") {
            int target;
            if (ss >> target) {
                applyPauliZ(target);
                std::cout << "[EXEC] Applied Pauli-Z Phase Gate to Qubit " << target << std::endl;
            }
            else {
                std::cout << "[USAGE] Z <target_qubit>" << std::endl;
            }
        }
        else if (op == "CNOT") {
            int control, target;
            if (ss >> control >> target) {
                applyCNOT(control, target);
                std::cout << "[EXEC] Applied CNOT Gate (Control: " << control << ", Target: " << target << ")" << std::endl;
            }
            else {
                std::cout << "[USAGE] CNOT <control_qubit> <target_qubit>" << std::endl;
            }
        }
        else if (op == "SWAP") {
            int q1, q2;
            if (ss >> q1 >> q2) {
                applySWAP(q1, q2);
                std::cout << "[EXEC] Applied SWAP Gate between Qubit " << q1 << " and Qubit " << q2 << std::endl;
            }
            else {
                std::cout << "[USAGE] SWAP <qubit1> <qubit2>" << std::endl;
            }
        }
        else if (op == "MEASURE") {
            std::cout << "[EXEC] Measuring Qubit Register..." << std::endl;
            measureAll();
        }
        else if (op == "PRINT") {
            printState();
        }
        else if (op == "RUN") {
            std::string filename;
            if (ss >> filename) {
                runScript(filename);
            }
            else {
                std::cout << "[USAGE] RUN <filename.ztron>" << std::endl;
            }
        }
        else if (op == "HELP") {
            std::cout << "Available ZTron ISA Commands:\n";
            std::cout << "  INIT <n>        - Allocate n qubits\n";
            std::cout << "  H <q>           - Apply Hadamard gate to qubit q\n";
            std::cout << "  X <q>           - Apply Pauli-X (NOT) gate to qubit q\n";
            std::cout << "  Z <q>           - Apply Pauli-Z Phase gate to qubit q\n";
            std::cout << "  CNOT <c> <t>    - Apply CNOT (Control c, Target t)\n";
            std::cout << "  SWAP <q1> <q2>  - Swap state of qubit q1 and qubit q2\n";
            std::cout << "  MEASURE         - Measure state & collapse wave function\n";
            std::cout << "  PRINT           - Display current state vector\n";
            std::cout << "  RUN <file>      - Execute a .ztron assembly script file\n";
            std::cout << "  EXIT            - Quit terminal\n";
        }
        else if (op == "EXIT" || op == "QUIT") {
            return false;
        }
        else {
            std::cout << "[ERROR] Unknown command. Type HELP for command list." << std::endl;
        }
        return true;
    }
};

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << "       ZTron QOS Interactive Shell v0.9                   " << std::endl;
    std::cout << "  Hardware Abstraction Layer & Full Gate Suite Enabled    " << std::endl;
    std::cout << "==========================================================\n" << std::endl;

    QubitRegister ztronCore(2);
    std::string userInput;

    while (true) {
        std::cout << "ztron-qos> ";
        if (!std::getline(std::cin, userInput)) break;
        if (!ztronCore.executeInstruction(userInput)) break;
    }

    std::cout << "\n[ZTron QOS Shutting Down...]" << std::endl;
    return 0;
}