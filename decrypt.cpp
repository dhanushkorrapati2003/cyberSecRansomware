#include <iostream>
#include <fstream>
#include <filesystem>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/err.h>

namespace fs = std::filesystem;

// Hardcoded RSA private key in PEM format
const std::string hardcoded_private_key_pem = R"(
-----BEGIN RSA PRIVATE KEY-----
MIIEogIBAAKCAQEAkdsqDDz2aNs1G3pVc6FnnyGltUmYVQHMpCAfffUHqGKzX9KV
Sq5nzgk9ToOHHtVaVO+btfGQ7qhFcbbnUHNUpTBQ2Ns7bejOevop/mDPd0FFACIT
fbCvWSW9e5iig+OVaA1V98D6nJbfqomdUQ8Q9m+cy4OtEE7UIQsjlB7dn8QZLQx9
+Vdc9yT2kgcsnm/sgJF4U/ITfzoZ0rJJCFnp0H4/m6+63VBpSEYDRHtjx2mBf4T1
6p/1ZAD0LNQ7HGmitueN1cl/N+dhRNxmHq5eYGafOo/JMCHj4irI8pTIP9zg3HpN
xNUr2nQ/A0zn0EfKDCf7m0h6BIUxFf2fjjIkEQIDAQABAoIBAChLLJWO7d/P1x8N
L7gu+cpraDTp7Oxm/9rDiQ+p9oCdK8KoEVg9UC4KPWaITXbGKBtYymJ8JrbXw7+D
cOm3wsWT0HHoNsXJT+mHXLyKkCX44RAFWe8aNYbiNzOC2jqnT6ocLQixxkomIAoj
1FMFX4FFGkLa0J+1t0KdqimMUVuGTTzOeEgvU6r189HdX86QkKCujjlBnAnqIX5y
wJP0jlCNJ2pRpN67RKFRFk/aKovI4z4YzY1PfpuC5S4Nr2lGa9TsCzUNjmH2nELP
PjndQ+b0vVv5h/RcQVvc9yLcU5IJy75sPc8atbsD+59xGXWcZ8lVDiuy7C1gOaud
cjufFgECgYEAyEKt2dhhDLfLec++mUqRIiw1v/ndNcKXaMgkUZs9xHrul6wG/tgI
nQVSbaXt3e0QxqUJK4hE0+xzIqkz1SPD8wfwFYEF11cVvwdxru/m440vZFqsBwdI
USiXYT33G+DZcXsJcza7zugaOf740uFkrOXRAbVvWWBI8TLlEjV8j3ECgYEAunP3
t3oP7DDKfsiSrCEsjmM+ra3VY4Fqd1fIR0cHnHIlJE15t5bulc0WLO34D2euapUV
xv0zGi9Zb1720FhY64vYZzh89w2rlJ4OCw7jHLUzv0Tc9U32uBg1YDbEA6LvgEnl
++uDVCCbVg9VRVoKPz6QOIpVnZN5XGGEhnzfzqECgYAfayavaN1BC4zm3Yr/rc07
WUhNiDwGdTMgA1Z/XyW/r7HDJgAq92XSjMUz4raA/PKG19mqdMSnow8lPvqc0/Go
LXSjSwzJjiYhRF+3nHfS95GZ4wfeSmw0H3Qwk3Izc4Fp5m3DH7EzcwK6WqBbJXWQ
ErZJ2FdR5GNNRDjgNHgIYQKBgCBT6Rhr1oTqNMHkvuGPYu1RMdXlNQFqt+33T2dk
XmJ2vA/eSwNU7QsWIwkpPYBWRrx+GueEkS6yFPylZ3/aQZidh6jl92zSdvRurxql
uyUIGe/I3dK+cM+ciuyisju7LXF93p1eSUf121kBkZRTt0Ed9N2Ixq1qm3k0A0la
L2fhAoGAfn5MVPvAXgBdZWlaeJVYVgjRRdToN0UvAN5A8o2LtQxQVvyUlpjwkkXB
JlI6Enk02L5E764fiwuHve2kFJhYIAllmq9Sl6OBKzzEO/JhTDZSgi0C9I0ADYWx
djV36+KTlC683M32uVLNn1Tcp94OzcvNYVIH7FEwnVRIuX2Y5/A=
-----END RSA PRIVATE KEY-----
)";

// Decrypt AES key using RSA private key
std::string rsa_decrypt_aes_key(const unsigned char* encrypted_aes_key, int encrypted_key_len, RSA* rsa_private_key) {
    std::string decrypted_key(RSA_size(rsa_private_key), '\0');
    int result = RSA_private_decrypt(encrypted_key_len, encrypted_aes_key, reinterpret_cast<unsigned char*>(&decrypted_key[0]), rsa_private_key, RSA_PKCS1_OAEP_PADDING);

    if (result == -1) {
        std::cerr << "RSA decryption failed: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
        return "";
    }

    decrypted_key.resize(result); // Resize to actual key length
    return decrypted_key;
}

// Decrypt data using AES
std::string aes_decrypt(const std::string& encrypted_data, unsigned char* aes_key, unsigned char* iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    std::string plaintext(encrypted_data.size(), '\0');
    int len, plaintext_len;

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, aes_key, iv);

    EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(&plaintext[0]), &len, reinterpret_cast<const unsigned char*>(encrypted_data.c_str()), encrypted_data.size());
    plaintext_len = len;

    EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&plaintext[0]) + len, &len);
    plaintext_len += len;

    plaintext.resize(plaintext_len);  // Resize to the actual plaintext size
    EVP_CIPHER_CTX_free(ctx);

    return plaintext;
}

// Load the private RSA key from a hardcoded PEM string
RSA* load_hardcoded_private_key() {
    BIO* bio = BIO_new_mem_buf(hardcoded_private_key_pem.data(), -1);
    RSA* rsa_private_key = PEM_read_bio_RSAPrivateKey(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);

    if (!rsa_private_key) {
        std::cerr << "Failed to load private key from hardcoded PEM." << std::endl;
    }

    return rsa_private_key;
}

// Function to decrypt all files in a folder recursively
void decrypt_files_in_folder(const std::string& folder_path, RSA* rsa_private_key) {
    for (const auto& entry : fs::recursive_directory_iterator(folder_path, fs::directory_options::skip_permission_denied)) {
        if (entry.is_regular_file() && entry.path().extension() != ".key") { // Avoid processing the key files themselves
            std::string key_file = entry.path().string() + ".key";

            // Read the encrypted AES key from the key file
            std::ifstream key_in(key_file, std::ios::binary);
            if (!key_in) {
                std::cerr << "Failed to open key file: " << key_file << std::endl;
                continue;
            }

            std::string encrypted_aes_key((std::istreambuf_iterator<char>(key_in)), std::istreambuf_iterator<char>());
            key_in.close();

            // Decrypt the AES key using the RSA private key
            std::string decrypted_aes_key = rsa_decrypt_aes_key(reinterpret_cast<const unsigned char*>(encrypted_aes_key.data()), encrypted_aes_key.size(), rsa_private_key);
            if (decrypted_aes_key.empty()) {
                std::cerr << "Failed to decrypt AES key for file: " << entry.path() << std::endl;
                continue;
            }

            // Read the encrypted file content
            std::ifstream encrypted_file(entry.path(), std::ios::binary);
            if (!encrypted_file) {
                std::cerr << "Failed to open encrypted file: " << entry.path() << std::endl;
                continue;
            }

            // Read the IV and encrypted content from the file
            unsigned char iv[AES_BLOCK_SIZE];
            encrypted_file.read(reinterpret_cast<char*>(iv), AES_BLOCK_SIZE);

            std::string encrypted_content((std::istreambuf_iterator<char>(encrypted_file)), std::istreambuf_iterator<char>());
            encrypted_file.close();

            // Decrypt the file content using the decrypted AES key and IV
            std::string decrypted_content = aes_decrypt(encrypted_content, reinterpret_cast<unsigned char*>(&decrypted_aes_key[0]), iv);

            // Save the decrypted content back to the file
            std::ofstream decrypted_file(entry.path(), std::ios::binary);
            decrypted_file.write(decrypted_content.c_str(), decrypted_content.size());
            decrypted_file.close();

            // Optionally, remove the key file after decryption
            fs::remove(key_file);

            std::cout << "File: " << entry.path().filename().string() << " has been decrypted.\n";
        }
    }
}

int main() {
    std::string folder_path = ".";  // Change this to the folder you want to decrypt

    // Load the RSA private key from the hardcoded PEM string
    RSA* rsa_private_key = load_hardcoded_private_key();
    if (!rsa_private_key) {
        std::cerr << "Error loading private key. Exiting." << std::endl;
        return 1;
    }

    // Decrypt all files using the RSA private key
    decrypt_files_in_folder(folder_path, rsa_private_key);

    // Free the RSA key
    RSA_free(rsa_private_key);

    std::cout << "\nDecryption complete. Files have been decrypted.\n";
    return 0;
}
 