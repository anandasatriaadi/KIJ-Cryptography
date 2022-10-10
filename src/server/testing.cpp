#include "../encryption/des.cpp"

int main() {

    // cout << des.decryptBlock((unsigned char *)"6174FF9737417F6F", (const unsigned char *)"616B756164616C61");

    unsigned char* plainText = (unsigned char*)"aku anak gembala yang baik hati";
    int plainTextLen = 32;
    const unsigned char* key = (const unsigned char*)"akuadala";
    
    DES des = DES();

    // for(int i = 0; i < 8; i++) {
    //     cout << hex << (unsigned int)plainText[i];
    // }
    // cout << endl;

    unsigned char *resEncrypt = des.encryptCBC(plainText, plainTextLen, key);
    for (int i = 0; i < 4; i++)
    {
        cout << "resEncrypt: " << des.charToHex(resEncrypt+(i*8)) << endl;
    }
    

    cout << endl;
    cout << "ajksdasd DECRYPTION " << endl;
    unsigned char *resDecrypt = des.decryptCBC(resEncrypt, plainTextLen, key);
    cout << "resDecrypt: " << resDecrypt << endl;

    return 0;
}