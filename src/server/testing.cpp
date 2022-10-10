#include "../encryption/des.cpp"

int main() {
    DES des;
    unsigned char* plainText = (unsigned char*)"เวียนหัวมาก ทำทุกอย่างโดยใช้ cpp นี้ ซึ่งใช้ยากมากจนหัวจะระเบิด";
    int plainTextLen = 176;
    const unsigned char* key = (const unsigned char*)"akuadala";

    // for(int i = 0; i < 8; i++) {
    //     cout << hex << (unsigned int)plainText[i];
    // }
    // cout << endl;
    uint fix_len = uint(ceil(float(plainTextLen)/float(8))*8);
    unsigned char* plainTextFixed = new unsigned char[fix_len*8];
    unsigned char* decryptedSum = new unsigned char[fix_len*8];
    memset(plainTextFixed, '\0', fix_len*8);
    memcpy(plainTextFixed, plainText, plainTextLen);
    cout << plainTextFixed << endl;
    for(int i = 0; i < ceil(float(plainTextLen)/float(8)); i++) {
        unsigned char *currentText = new unsigned char[8];
        memset(currentText, '\0', 8);
        memcpy(currentText, plainTextFixed+(i*8), 8);
        cout << currentText << endl;

        unsigned char *currentTextHex = des.charToHex(currentText);

        unsigned char *keyHex = des.charToHex(key);
        // cout << keyHex << endl;

        unsigned char *currentTextAfterHex = des.hexToChar(currentTextHex);
        // cout << currentTextAfterHex << endl;

        cout << "Encryption: \n";
        unsigned char *cipherTextHex = des.encryptBlock(currentTextHex, keyHex, false);
        unsigned char *cipherTextChar = des.hexToChar(cipherTextHex);

        cout << "HEX = " << cipherTextHex << " Char = " << cipherTextChar << "\n";
        cout << "\n";
        
        cout << "Decryption: \n";
        unsigned char *decryptedTextHex = des.decryptBlock(cipherTextHex, keyHex);
        unsigned char *decryptedTextChar = des.hexToChar(decryptedTextHex);
        cout << "HEX = " << decryptedTextHex << " Char = " << decryptedTextChar << "\n";
        memcpy(decryptedSum+(i*8), decryptedTextChar, 8);
    }

    cout << "Decrypted Sum: " << decryptedSum << endl;
    return 0;
}