Here’s an updated **README** tailored to your project, explaining how to build the source code, deploy the server, and simulate the attack using the Word document. 

---

# **Ransomware Simulation Project**

This project simulates a modern ransomware workflow, integrating file encryption, payload delivery, and deployment via VBA macros. It demonstrates how ransomware can encrypt files on a target system and manage encryption keys using AES and RSA algorithms. The entire process is designed for **educational and research purposes only**.

---

## **Project Highlights**
1. **Encryption Workflow**:
   - Each file is encrypted with a unique AES-256 key.  
   - AES keys are encrypted with a hardcoded RSA public key.  
   - Files are overwritten with their encrypted versions, and encrypted AES keys are saved in `.key` files.

2. **Payload Delivery**:
   - A **Flask Web Server** hosts the ransomware payload (`encrypt.exe`) for remote delivery.  
   - The payload is downloaded and executed using a **VBA macro** embedded in a Word document.

3. **Decryption Workflow**:
   - Decryption is achieved using the corresponding RSA private key to unlock the AES keys and restore the original files.

4. **Source Code**:
   - The project provides C++ source code for the encryption and decryption executables.

---

## **Repository Structure**

```plaintext
├───Payload
│   ├───Isolated Payload         # Contains isolated test files for encryption
├───Webserver                    # Flask-based web server for payload delivery
├───decrypt.cpp                  # C++ source code for the decryption program
├───decrypt.exe                  # Compiled executable of the decryption program
├───encrypt.cpp                  # C++ source code for the encryption program
├───encrypt.exe                  # Compiled executable of the encryption program
└───Readme.md                    # Documentation for the project
```

---

## **How to Build the Encryption and Decryption Executables**

### **Requirements**
- **C++ Compiler**: `g++` or any compiler that supports C++17 or later.
- **OpenSSL Library**: Required for encryption and key management.
  - Install OpenSSL:
    - On Ubuntu/Debian:  
      ```bash
      sudo apt-get install libssl-dev
      ```
    - On macOS (via Homebrew):  
      ```bash
      brew install openssl
      ```

### **Steps to Build**

1. **Navigate to the Source Code**:
   ```bash
   cd /path/to/source/code
   ```

2. **Compile the Encryption Program**:
   ```bash
   g++ encrypt.cpp -o encrypt.exe -lssl -lcrypto -std=c++17
   ```
   This generates the `encrypt.exe` executable for file encryption.

3. **Compile the Decryption Program**:
   ```bash
   g++ decrypt.cpp -o decrypt.exe -lssl -lcrypto -std=c++17
   ```
   This generates the `decrypt.exe` executable for file decryption.

---

## **How to Use the Server and Payload**

### **Setting Up the Flask Web Server**

1. **Navigate to the Web Server Directory**:
   ```bash
   cd Webserver
   ```

2. **Install Flask**:
   ```bash
   pip install flask
   ```

3. **Run the Server**:
   ```bash
   python server.py
   ```
   - The server starts on `http://127.0.0.1:5000`.
   - It hosts the `encrypt.exe` file for download by the Word document macro.

---

### **Simulating the Attack**

1. **Prepare the Word Document**:
   - Embed the VBA macro (`Document_Open`) in a Word document.
   - The macro downloads the `encrypt.exe` file from the Flask server and executes it on the target system.

2. **Place the Payload on the Server**:
   - Copy the `encrypt.exe` file into the Web Server directory.
   - Ensure the file name matches the one used in the macro (e.g., `http://127.0.0.1:5000/encrypt.exe`). Most likely, the name in the macro would be `ransomware.exe`.

3. **Trigger the Attack**:
   - Open the Word document. The macro will:
     - Download `encrypt.exe` from the server.
     - Save it in a local folder and execute it.
     - The encryption program starts encrypting files in the target directory.

---

## **Testing and Decryption**

1. **Encrypt Files**:
   - Run the encryption program manually for testing:
     ```bash
     ./encrypt.exe /path/to/test_folder
     ```
   - The program encrypts all files in the specified folder, saving encrypted AES keys in `.key` files.

2. **Decrypt Files**:
   - Use the decryption program with the RSA private key:
     ```bash
     ./decrypt.exe /path/to/test_folder
     ```
   - This restores the original files.

---

## **Important Notes**

### **Controlled Environment**
- Always test the project in a **controlled environment** such as a virtual machine.  
- Do not execute the ransomware simulation on live systems or sensitive data.

### **Backup**
- Ensure backups of all test data are available, as the program overwrites files during encryption.

### **Ethical Use**
- This project is for **educational purposes only**. Misuse of the code or techniques is illegal and unethical.

---

## **License**
This project is open-source and is provided "as-is" without any warranty. It is intended for **educational and research purposes only**. Misuse of this software is strictly prohibited.