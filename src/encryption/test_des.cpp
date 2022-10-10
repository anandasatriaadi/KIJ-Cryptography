#include "../encryption/des.cpp"

int main() {
    DES des;
    unsigned char* plainText = (unsigned char*)"123456ABCD132536";
    unsigned char* cipherText = (unsigned char*)"C0B7A8D05F3A829C";
    const unsigned char* key = (const unsigned char*)"AABB09182736CCDD";

    cout << "Encryption: \n";
    des.encryptBlock(plainText, key, false);
    cout << "\n";
    
    cout << "Decryption: \n";
    des.decryptBlock(cipherText, key);
    
    return 0;
}