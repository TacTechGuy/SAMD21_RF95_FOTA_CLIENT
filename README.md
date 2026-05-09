# Secure RF95 LoRa Telemetry & Systems Integration
**Hardware:** SAMD21 (Cortex-M0+) | **Radio:** RF95 (LoRa) | **Security:** Speck Cipher | **Control:** Visual Basic GUI

---

## 🛠️ Project Overview
This is an end-to-end embedded system designed for secure, long-range binary file transfer. It bridges the gap between hardware-level encryption and high-level user control, featuring a custom-built PC interface to manage the SAMD21 firmware.

### 🛡️ Core Security & Networking Features
*   **Speck Block Cipher:** Lightweight 128-bit encryption optimized for resource-constrained MCUs.
*   **Modified RadioHead Driver:** Custom-modified `RHEncryptedDriver` to bridge the Speck implementation with the LoRa transport layer.
*   **Binary Integrity Hashing:** Custom verification routines to ensure zero-corruption during SD card file streams.
*   **Secret Management:** Professional isolation of cryptographic keys via `secrets.h` and `.gitignore` protocols.

### 🖥️ Systems Integration (PC-to-Hardware)
To facilitate testing and deployment, a custom **Visual Basic Serial GUI** was developed. This tool allows for:
*   Real-time command execution from a PC to the SAMD21.
*   Monitoring of file transfer progress and hash verification.
*   Hardware-in-the-loop (HIL) testing of the RF95 radio states.

---

## 📂 System Architecture
*   `src/`: Dedicated build targets for **Transmitter** and **Receiver** roles.
*   `lib/`: Modularized libraries for RadioHead, Speck, and FileTransfer logic.
*   `tools/`: Visual Basic source code for the Serial Control Interface.
*   `include/`: Private configuration and security headers.

---

## 📝 API Usage (FileTransfer Library)
1. **Initialize:** `fileTransfer.initalizeHashFile(file, "read");`
2. **Measure:** `fileTransfer.fileSize();`
3. **Process:** `fileTransfer.hashFileStream();`
4. **Validate:** `fileTransfer.getFileHash();`

---

## 🚀 Professional Development Workflow
This project demonstrates a transition from rapid Arduino prototyping to professional **PlatformIO** engineering:
- [x] Separation of Secrets (DevSecOps best practices).
- [x] Modular C++ Library Design.
- [x] Version Control (Git) with structured commit history.
- [ ] *Next Step:* Implementing Unit Testing for cryptographic verification.

---

## 👨‍💻 Development History
Evolved from several specialized test environments:
*   **Serial Interface:** Command-line testing between a custom VB GUI and SAMD21.
*   **SD Binary Testing:** Isolated file read/write performance benchmarks.
*   **Legacy Integration:** Merged standalone `serialCommands` and `ReadWriteSDBinary` sketches into a unified, modular system.

