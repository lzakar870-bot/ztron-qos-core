# ZTron Quantum Operating System (QOS) v0.9 Core Engine

**ZTron QOS** is a proprietary quantum-software orchestration layer designed to bridge high-level quantum algorithm assembly with low-level quantum simulation engines and hardware backends. 

Built in C++, ZTron provides an $N$-qubit state-vector execution engine, real-time command-line interface (CLI), and a script parser for native `.ztron` assembly execution.

---

## Technical Specifications

* **Execution Core:** $N$-qubit complex state-vector linear algebra engine ($2^N$ quantum amplitude register).
* **Instruction Set Architecture (ISA):** Low-level custom assembly processing unit supporting parameter validation and real-time state tracking.
* **Script Execution Engine:** File-stream parser executing `.ztron` scripts directly from disk.
* **Language/Standard:** C++17 (MSVC Compatible).

---

## ZTron ISA Command Matrix

| Instruction | Parameters | Description |
| :--- | :--- | :--- |
| `INIT` | `<n>` | Allocates state vector for $n$ qubits (resets register to $\Vert{}0\dots0\rangle$). |
| `H` | `<q>` | Applies Hadamard gate to target qubit `q` (superposition). |
| `X` | `<q>` | Applies Pauli-X (NOT) gate to target qubit `q` (bit flip). |
| `Z` | `<q>` | Applies Pauli-Z phase gate to target qubit `q`. |
| `CNOT` | `<control> <target>` | Applies Controlled-NOT gate between control and target qubits. |
| `SWAP` | `<q1> <q2>` | Swaps state amplitudes between qubit `q1` and qubit `q2`. |
| `MEASURE` | — | Measures all qubits, collapsing state vector probabilistically. |
| `PRINT` | — | Displays state vector probabilities and complex amplitudes. |
| `RUN` | `<file.ztron>` | Parses and executes an assembly script from disk. |
| `EXIT` | — | Terminates the ZTron terminal session. |

---

## Execution Demos Included

* **`bell_state.ztron`**: Generates a 2-qubit maximally entangled Bell state ($\frac{\Vert{}00\rangle + \Vert{}11\rangle}{\sqrt{2}}$).
* **`Teleportation.ztron`**: Full 3-qubit Quantum Teleportation protocol incorporating EPR pair creation and Alice-to-Bob state transmission.

---

## Architectural Vision & Roadmap

1. **v0.9 (Current Baseline):** C++ state-vector math backend, modular CLI interactive shell, `.ztron` file parser, Git tracking.
2. **v1.0 (Target):** Expanded gate suite ($S$, $T$, $R_y(\theta)$ rotation gates) and Hardware Abstraction Layer (HAL) stubs for hybrid QPU dispatch.
