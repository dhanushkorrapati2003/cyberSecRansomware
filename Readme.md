# File Encryption Program

This program recursively encrypts all files in a specified folder using a combination of RSA and AES encryption. Each file is encrypted with a unique AES-256 key, and that key is then encrypted using a hardcoded RSA public key. The encrypted files are saved back in their original location, and the AES keys are stored in separate `.key` files.

## Features
- **AES Encryption**: Each file is encrypted using AES-256 encryption.
- **RSA Encryption**: The AES key for each file is encrypted using the RSA public key.
- **Key Management**: Encrypted AES keys are saved in separate `.key` files for each encrypted file.
- **Recursive Encryption**: All files in a folder, including those in subfolders, are encrypted.
- **Self-Protection**: The program ensures that it does not encrypt its own executable file during the process.

## Prerequisites

This program depends on the OpenSSL library. You will need to have OpenSSL installed to compile and run the code.

### Install OpenSSL
- On **Ubuntu/Debian**:
  ```bash
  sudo apt-get install libssl-dev
  ```
- On **macOS** (with Homebrew):
  ```bash
  brew install openssl
  ```

## Compilation

To compile this program, use `g++` with the necessary OpenSSL libraries. Run the following command:

```bash
g++ -o encryptor encryptor.cpp -lssl -lcrypto -std=c++17
```

This command assumes your code file is named `encryptor.cpp` and that OpenSSL libraries are properly linked.

## Usage

1. **Run the Program**: To execute the program, provide the path to the folder you want to encrypt.
   
   ```bash
   ./encryptor /path/to/folder
   ```

   If no folder path is provided, the program will default to encrypting the current directory (`.`).

2. **Encryption Process**:
   - The program will generate an AES-256 key and IV for each file in the folder.
   - It will encrypt each file using AES, then encrypt the AES key using the hardcoded RSA public key.
   - The encrypted file will overwrite the original file.
   - The encrypted AES key will be saved in a `.key` file alongside the encrypted file.

3. **Example**:
   ```bash
   ./encryptor ./my_documents
   ```
   This will encrypt all files in the `./my_documents` directory.

## Files Generated
- **Encrypted Files**: Each file in the specified folder will be encrypted, and the original content will be replaced with the encrypted data.
- **.key Files**: For each file that is encrypted, a corresponding `.key` file is generated in the same directory. The `.key` file contains the encrypted AES key used to decrypt the file.

## Important Considerations
- **Self-Protection**: The program checks its own executable file and skips it during encryption to prevent encrypting itself.
- **Backup**: Make sure to have a backup of important files before running the program, as it overwrites the original files with encrypted versions.

## Error Handling
- The program prints error messages if:
  - It fails to open or read any file.
  - It encounters problems during AES or RSA encryption.
  - It fails to load the hardcoded RSA public key.
  
## Customization

### Modifying the RSA Public Key
The RSA public key is currently hardcoded in the source code. If you wish to use your own RSA public key, replace the content of `hardcoded_public_key_pem` in the code with your actual public key in PEM format.

## License

This code is provided as-is without any warranty. You are free to modify and use it for educational or personal projects.