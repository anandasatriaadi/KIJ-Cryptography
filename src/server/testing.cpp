#include "../encryption/des.cpp"

int main() {

    // cout << des.decryptBlock((unsigned char *)"6174FF9737417F6F", (const unsigned char *)"616B756164616C61");

    unsigned char* plainText = (unsigned char*)"4ku sungguh masih s4yang padamu, 7angan sampai kau meninggalkan aku, Begitu sangat berharga dirimu, Bag1ikuuu";
    unsigned int plainTextLen = (unsigned int) strlen((char*)plainText);
    const unsigned char* key = (const unsigned char*)"akuadala";
    
    DES des = DES();
    cout << "Plain Text Len" << plainTextLen << endl;

    // for(int i = 0; i < 8; i++) {
    //     cout << hex << (unsigned int)plainText[i];
    // }
    // cout << endl;

    unsigned char *resEncrypt = des.encryptCBC_Continous(plainText, plainTextLen);
    for (int i = 0; i < 9; i++)
    {
        unsigned char *res = (unsigned char *) malloc(sizeof(unsigned char) * 16);
        des.charToHex(resEncrypt+(i*8), res);
        cout << "resEncrypt: " << res << endl;
        free(res);
    }
    

    cout << endl;
    cout << "ajksdasd DECRYPTION " << endl;
    des.resetIV();
    unsigned char *resDecrypt = des.decryptCBC_Continous(resEncrypt, plainTextLen);
    cout << "resDecrypt: " << resDecrypt << endl;

    return 0;
}