#include <bitset>
#include <cstring>
#include <iostream>
#include <string>
#include <bits/stdc++.h>

using namespace std;

class DES {
    private:
        unsigned char **compressedKeysBin;
        bool roundKeysGenerated;
        bool isDecrypting;
        const uint BLOCK_LEN = 8;
        
        const unsigned char *KEY = (const unsigned char*)"4KUK4NT0";
        unsigned char *IV = (unsigned char*)"0010000001111001011000010110111001100111001000000110001001100001";
        unsigned char CBC_XORER[64];

        const int KEY_PARITY[56] = { 
            57, 49, 41, 33, 25, 17, 9,  1,  
            58, 50, 42, 34, 26, 18, 10, 2,  
            59, 51, 43, 35, 27, 19, 11, 3,
            60, 52, 44, 36, 63, 55, 47, 39, 
            31, 23, 15, 7, 62, 54, 46, 38, 
            30, 22, 14, 6,  61, 53, 45, 37,
            29, 21, 13, 5,  28, 20, 12, 4 
        };
        const int AMOUNT_SHIFT_PER_ROUND[16] = { 
            1, 1, 2, 2, 2, 2, 2, 2,
            1, 2, 2, 2, 2, 2, 2, 1     
        };
        const int INITIAL_PERMUTATION_TABLE[48] = {
            14, 17, 11, 24, 1,  5,  3,  28,
            15, 6,  21, 10, 23, 19, 12, 4,
            26, 8,  16, 7,  27, 20, 13, 2,
            41, 52, 31, 37, 47, 55, 30, 40,
            51, 45, 33, 48, 44, 49, 39, 56,
            34, 53, 46, 42, 50, 36, 29, 32
        };
        const int TEXT_PARITY[64] = {
            58, 50, 42, 34, 26, 18, 10, 2,  
            60, 52, 44, 36, 28, 20, 12, 4,  
            62, 54, 46, 38, 30, 22, 14, 6, 
            64, 56, 48, 40, 32, 24, 16, 8,  
            57, 49, 41, 33, 25, 17, 9,  1,  
            59, 51, 43, 35, 27, 19, 11, 3,  
            61, 53, 45, 37, 29, 21, 13, 5,  
            63, 55, 47, 39, 31, 23, 15, 7 
        };
        const int EXPANSION_TABLE[48] = {
            32, 1,  2,  3,  4,  5,  4,  5,  
            6,  7,  8,  9, 8,  9,  10, 11, 
            12, 13, 12, 13, 14, 15, 16, 17,
            16, 17, 18, 19, 20, 21, 20, 21, 
            22, 23, 24, 25, 24, 25, 26, 27, 
            28, 29, 28, 29, 30, 31, 32, 1
        };
        const unsigned char S_BOX_TABLE[8][4][16] = {
            {
                14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
                0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
                4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
                15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13
            },
            {
                15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
                3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
                0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
                13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9
            },
            {
                10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
                13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
                13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
                1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12
            },
            {
                7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
                13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
                10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
                3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14
            },
            {
                2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
                14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
                4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
                11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3
            },
            {
                12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
                10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
                9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
                4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13
            },
            {
                4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
                13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
                1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
                6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12
            },
            {
                13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
                1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
                7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
                2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
            }
        };
        const int PERMUTATION_TABLE[32] = {
            16, 7, 20, 21, 29, 12, 28, 17, 
            1,  15, 23, 26, 5, 18, 31, 10, 
            2,  8,  24, 14, 32, 27, 3,  9, 19, 
            13, 30, 6,  22, 11, 4,  25
        };
        const int INVERSE_PERMUTATION_TABLE[64] = {
            40, 8,  48, 16, 56, 24, 64, 32, 
            39, 7,  47, 15, 55, 23, 63, 31, 
            38, 6,  46, 14, 54, 22, 62, 30, 
            37, 5,  45, 13, 53, 21, 61, 29, 
            36, 4,  44, 12, 52, 20, 60, 28, 
            35, 3,  43, 11, 51, 19, 59, 27, 
            34, 2,  42, 10, 50, 18, 58, 26, 
            33, 1,  41, 9,  49, 17, 57, 25
        };
        map<unsigned char, unsigned char *> MAP_HEX;
        map<unsigned long, unsigned char> MAP_BIN;
        void generateSubKeys(const unsigned char *key);

    public:                                                                                             
        explicit DES();
        ~DES();
        void encryptBlock(unsigned char *plainText, const unsigned char *text, unsigned char result[]);
        unsigned char *encryptCBC(unsigned char *plainText, uint lenPlainText);
        unsigned char *decryptCBC(unsigned char *cipherText, uint lenCipherText);
        void encryptCBC_Continous(unsigned char *plainText, uint lenPlainText, unsigned char result[]);
        void decryptCBC_Continous(unsigned char *cipherText, uint lenCipherText, unsigned char result[]);
        void flipRoundKeys();

        // untility class for key encryption
        void hexToBinary(unsigned char *text, unsigned char *res); 
        void binaryToHex(unsigned char *text, unsigned char *res); 
        void permuteKeyWithParity(const unsigned char *text, unsigned char *res); 
        void shiftLeft(unsigned char *temp, int round);
        void permuteKeyWithKeyComposition(const unsigned char *text, unsigned char *res); 
        void permuteWithExpTab(unsigned char *text, unsigned char *res);
        void xorOperation(unsigned char *text1, unsigned char *text2, uint size, unsigned char *res);
        void permuteWithPermutationTab(const unsigned char *text, unsigned char *res);
        void permuteWithFinalPermutationTab(unsigned char *text, unsigned char *res);
        void decryptBlock(unsigned char *cipherText, const unsigned char *key, unsigned char result[]);

        // Decrypt
        void permuteTextWithParity(const unsigned char *text, unsigned char *res);

        // Utility class for text conversion
        unsigned char **generate2DUC(uint rows, uint col);
        void charToHex(const unsigned char *text, unsigned char *res);
        void hexToChar(unsigned char *text, unsigned char *res);
        void resetIV();
};

DES::DES() {
    this->compressedKeysBin = generate2DUC(16, 48);
    this->isDecrypting = false;
    resetIV();
    
    MAP_HEX['0'] = (unsigned char *)"0000";
    MAP_HEX['1'] = (unsigned char *)"0001";
    MAP_HEX['2'] = (unsigned char *)"0010";
    MAP_HEX['3'] = (unsigned char *)"0011";
    MAP_HEX['4'] = (unsigned char *)"0100";
    MAP_HEX['5'] = (unsigned char *)"0101";
    MAP_HEX['6'] = (unsigned char *)"0110";
    MAP_HEX['7'] = (unsigned char *)"0111";
    MAP_HEX['8'] = (unsigned char *)"1000";
    MAP_HEX['9'] = (unsigned char *)"1001";
    MAP_HEX['A'] = (unsigned char *)"1010";
    MAP_HEX['B'] = (unsigned char *)"1011";
    MAP_HEX['C'] = (unsigned char *)"1100";
    MAP_HEX['D'] = (unsigned char *)"1101";
    MAP_HEX['E'] = (unsigned char *)"1110";
    MAP_HEX['F'] = (unsigned char *)"1111";
    
    MAP_BIN[0] = (unsigned char)'0';
    MAP_BIN[1] = (unsigned char)'1';
    MAP_BIN[2] = (unsigned char)'2';
    MAP_BIN[3] = (unsigned char)'3';
    MAP_BIN[4] = (unsigned char)'4';
    MAP_BIN[5] = (unsigned char)'5';
    MAP_BIN[6] = (unsigned char)'6';
    MAP_BIN[7] = (unsigned char)'7';
    MAP_BIN[8] = (unsigned char)'8';
    MAP_BIN[9] = (unsigned char)'9';
    MAP_BIN[10] = (unsigned char)'A';
    MAP_BIN[11] = (unsigned char)'B';
    MAP_BIN[12] = (unsigned char)'C';
    MAP_BIN[13] = (unsigned char)'D';
    MAP_BIN[14] = (unsigned char)'E';
    MAP_BIN[15] = (unsigned char)'F';
}

DES::~DES() {
    for (int i = 0; i < 16; i++) {
        delete[] compressedKeysBin[i];
    }
    delete[] compressedKeysBin;
}

/* ======== START ::: UTILITY METHOD ======== */
void DES::resetIV(){
    copy(IV, IV + 64, CBC_XORER);
}

unsigned char ** DES::generate2DUC(uint rows, uint col){
    unsigned char **res; 
    res = new unsigned char *[rows];
    for (int i = 0; i < rows; i++){
        res[i] = new unsigned char[col];
    }
    return res;
}

void DES::hexToBinary(unsigned char *text, unsigned char *res)
{
    for(int i=0; i<16; i++){
        copy(MAP_HEX[text[i]], MAP_HEX[text[i]] + 4, res + i*4);
    }
} 
void DES::binaryToHex(unsigned char *text, unsigned char *res)
{
    unsigned char temp[5];
    fill(temp, temp + 5, '\0');
    for(int i = 0; i < 16; i++){
        copy(text + i*4, text + i*4 + 4, temp);
        res[i] = MAP_BIN[bitset<4>(temp).to_ulong()];
    }
} 
/* ======== END ::: UTILITY METHOD ======== */

/* ======== START ::: ENCRYPT KEY ======== */
void DES::permuteKeyWithParity(const unsigned char *key, unsigned char *res)
{
    for(int i=0; i<56; i++){
        res[i] = key[this->KEY_PARITY[i]-1];
    }
} 
void DES::shiftLeft(unsigned char *res, int round)
{
    unsigned char temp[28];
    
    for(int i = 0; i < this->AMOUNT_SHIFT_PER_ROUND[round]; i++){
        // cout << "BEFORE SHIFT " << i << " ::: " << res << endl;
        int j = 1;
        for(; j < 28; j++){
            temp[j-1] = res[j];
        }
        // make the new temp as new res 
        temp[j-1] = res[0];
        fill(res, res + 28, '\0');
        copy(temp, temp + 28, res);
    }
} 
void DES::permuteKeyWithKeyComposition(const unsigned char *key, unsigned char *res)
{
    for(int i=0; i<48; i++){
        res[i] = key[this->INITIAL_PERMUTATION_TABLE[i]-1];
    }
} 
/* ======== END ::: ENCRYPT KEY ======== */

/* ======== START ::: ENCRYPT & DECRYPT ======== */
void DES::permuteTextWithParity(const unsigned char *text, unsigned char *res)
{
    for(int i=0; i<64; i++){
        res[i] = text[this->TEXT_PARITY[i]-1];
    }
} 
void DES::permuteWithExpTab(unsigned char *text, unsigned char *res)
{
    for(int i=0; i<48; i++){
        res[i] = text[this->EXPANSION_TABLE[i]-1];
    }
} 
void DES::xorOperation(unsigned char *text1, unsigned char *text2, uint size, unsigned char *res)
{
    for(int i=0; i<size; i++){
        res[i] = text1[i] != text2[i] ? '1' : '0';
    }
}

void DES::permuteWithPermutationTab(const unsigned char *text, unsigned char *res)
{
    for(int i=0; i<32; i++){
        res[i] = text[this->PERMUTATION_TABLE[i]-1];
    }
} 
void DES::permuteWithFinalPermutationTab(unsigned char *text, unsigned char *res)
{
    for(int i=0; i<64; i++){
        res[i] = text[this->INVERSE_PERMUTATION_TABLE[i]-1];
    }
} 
/* ======== END ::: ENCRYPT & DECRYPT ======== */

void DES::generateSubKeys(const unsigned char *key)
{
    // 1. make key from hex to binary 
    unsigned char binaryKey[64];
    hexToBinary((unsigned char *)key, binaryKey);

    // 2. initial permutation 
    unsigned char ipRes[56];
    permuteKeyWithParity(binaryKey, ipRes); 

    // 3. splitting to LPT and RPT 
    unsigned char lpt[28];
    unsigned char rpt[28];
    copy(ipRes, ipRes + 28, lpt);
    copy(ipRes + 28, ipRes + 56, rpt);

    // 4. key transformation
    unsigned char combine[56];
    for(int i=0; i<16; i++){
        // shift left 
        shiftLeft(lpt, i);
        shiftLeft(rpt, i);

        copy(lpt, lpt + 28, combine);
        copy(rpt, rpt + 28, combine + 28);

        // get round keys from compression permutation (from 56 bits to 48 bits)
        unsigned char *compressedKey = new unsigned char[48];
        permuteKeyWithKeyComposition(combine, compressedKey);
        this->compressedKeysBin[i] = compressedKey;
    }
    // delete[] binaryKey;
    // delete[] ipRes;
    // delete[] combine;
    // delete[] lpt;
    // delete[] rpt;
}
void DES::encryptBlock(unsigned char *plainText, const unsigned char *key, unsigned char result[])
{
    // cout << plainText << "   " << key << endl;
    if(!this->roundKeysGenerated){
        generateSubKeys(key);
    }
    if(this->isDecrypting){
        flipRoundKeys();
    }

    // 1. make text from hex to binary 
    unsigned char binaryText[64];
    hexToBinary(plainText, binaryText);
    
    // 2. initial permutation for text 
    unsigned char permutatedText[64];
    permuteTextWithParity(binaryText, permutatedText);

    // 3. split text 
    unsigned char lpt[32];
    copy(permutatedText, permutatedText + 32, lpt);

    unsigned char rpt[32];
    copy(permutatedText + 32, permutatedText + 64, rpt);

    unsigned char rptExpanded[48];
    unsigned char permutedSBox[32];
    unsigned char xored[48];
    for(int i=0; i<16; i++){
        // 4. expand with D-Box (from 32 bits to 48 bits)
        permuteWithExpTab(rpt, rptExpanded);

        // xor with compressed binary key from same round 
        xorOperation(this->compressedKeysBin[i], rptExpanded, 48, xored);        
        
        /*
            res of 48 bits divided to 8 equal parts and passed trough 8 sub boxes(each is 6 bits).
            res of that is each box is reduced from 6 to 4 bits
        */
        string sBoxRes = "";
        for (int i = 0; i < 8; i++) {
            /*
                ex[] = 100101
                row = binToHex(ex[0]ex[5])
                col = binToHex(ex[1]ex[2]ex[3]ex[4])
            */
            int row = 2 * int(xored[i * 6] - '0') + int(xored[i * 6 + 5] - '0');
            int col = 8 * int(xored[i * 6 + 1] - '0')
                      + 4 * int(xored[i * 6 + 2] - '0')
                      + 2 * int(xored[i * 6 + 3] - '0')
                      + int(xored[i * 6 + 4] - '0');
            
            int val = this->S_BOX_TABLE[i][row][col];

            // change elm S-BOX to binary 
            sBoxRes += char(val / 8 + '0');
            val = val % 8;
            sBoxRes += char(val / 4 + '0');
            val = val % 4;
            sBoxRes += char(val / 2 + '0');
            val = val % 2;
            sBoxRes += char(val + '0');
        }

        permuteWithPermutationTab((unsigned char*)sBoxRes.c_str(), permutedSBox);

        xorOperation(permutedSBox, lpt, 32, xored);

        copy(xored, xored + 32, lpt);
        // lpt = xored;

        if(i != 15){
            unsigned char temp[32];
            copy(rpt, rpt + 32, temp);
            copy(lpt, lpt + 32, rpt);
            copy(temp, temp + 32, lpt);
        }

    }
    // delete[] rptExpanded;
    // delete[] permutedSBox;
 
    unsigned char fullText[64];
    copy(lpt, lpt + 32, fullText);
    copy(rpt, rpt + 32, fullText + 32);

    unsigned char cipherTextBin[64];
    permuteWithFinalPermutationTab(fullText, cipherTextBin);
    unsigned char *cipherText = new unsigned char[16];
    binaryToHex(cipherTextBin, cipherText);

    copy(cipherText, cipherText + 16, result);
}
void DES::decryptBlock(unsigned char *cipherText, const unsigned char *key, unsigned char result[])
{
    int size = 16;
    
    // reverse round keys 
    flipRoundKeys();

    encryptBlock(cipherText, key, result);
}

void DES::charToHex(const unsigned char *text, unsigned char *res) {
    unsigned char hex[16];
    unsigned char binary[64];
    for (int i = 0; i < 8; i++) {
        memcpy(binary + (i * 8), bitset<8>(text[i]).to_string().c_str(), 8);
    }
    binaryToHex(binary, res);
}

void DES::hexToChar(unsigned char *text, unsigned char *res) {
    unsigned char binary[64];
    hexToBinary(text, binary);
    
    string temp = "";
    for (int i = 0; i < 64; i++) {
        temp += binary[i];
        if((i + 1) % 8 == 0) {
            res[i / 8] = (unsigned char)bitset<8>(temp).to_ulong();
            temp = "";
        }
    }
}

unsigned char *DES::encryptCBC(unsigned char *plainText, uint lenPlainText)
{
    const unsigned char *key = this->KEY;

    uint fix_len = uint(ceil(float(lenPlainText)/float(8))*8);
    unsigned char plainTextFixed[fix_len];
    memset(plainTextFixed, '\0', fix_len);
    memcpy(plainTextFixed, plainText, lenPlainText);

    unsigned char *cipherText = new unsigned char[fix_len];
    memset(cipherText, '\0', fix_len);

    unsigned char xorer[64];
    memcpy(xorer, IV, 64);

    for(int i = 0; i < ceil(float(lenPlainText)/float(8)); i++) {
        unsigned char currentText[8];
        memset(currentText, '\0', 8);
        memcpy(currentText, plainTextFixed+(i*8), 8);

        unsigned char currentTextHex[16];
        charToHex(currentText, currentTextHex);
        unsigned char currentTextBin[64];
        hexToBinary(currentTextHex, currentTextBin);

        unsigned char keyHex[16];
        charToHex(key, keyHex);

        unsigned char xoredWithPlainTextBin[64];
        xorOperation(currentTextBin, xorer, 64, xoredWithPlainTextBin);
        unsigned char xoredWithPlainTextHex[16];
        binaryToHex(xoredWithPlainTextBin, xoredWithPlainTextHex);
        
        unsigned char cipherTextHex[16];
        encryptBlock(xoredWithPlainTextHex, keyHex, cipherTextHex);
        unsigned char cipherTextBin[64];
        hexToBinary(cipherTextHex, cipherTextBin);

        unsigned char cipherTextChar[8];
        hexToChar(cipherTextHex, cipherTextChar);
        memcpy(cipherText+(i*8), cipherTextChar, 8);
        memcpy(xorer, cipherTextBin, 64);
    }

    return cipherText;
}

unsigned char *DES::decryptCBC(unsigned char *cipherText, uint lenCipherText)
{
    const unsigned char *key = this->KEY;
    // cout << cipherText << " " << lenCipherText << " " << key << endl;

    uint fix_len = uint(ceil(float(lenCipherText)/float(8))*8);
    unsigned char cipherTextFixed[fix_len];
    fill(cipherTextFixed, cipherTextFixed + fix_len, '\0');
    copy(cipherTextFixed, cipherTextFixed + fix_len, cipherText);
    

    unsigned char *plainText = new unsigned char[fix_len];
    fill(plainText, plainText + fix_len, '\0');

    unsigned char xorer[64];
    copy(xorer, xorer + 64, IV);

    for(int i = 0; i < ceil(float(lenCipherText)/float(8)); i++) {
        unsigned char currentText[8];
        fill(currentText, currentText + 8, '\0');
        copy(currentText, currentText + 8, cipherTextFixed+(i*8));

        unsigned char currentTextHex[16];
        charToHex(currentText, currentTextHex);
        unsigned char currentTextBin[64];
        hexToBinary(currentTextHex, currentTextBin);

        unsigned char keyHex[16];
        charToHex(key, keyHex);
        
        // TODO OI: Decrypt cipher text
        // cout << "currentTextHex      " << currentTextHex << endl;
        unsigned char decryptedTextHex[16];
        decryptBlock(currentTextHex, keyHex, decryptedTextHex);
        unsigned char decryptedTextBin[64];
        hexToBinary(decryptedTextHex, decryptedTextBin);
        // cout << "decryptedTextHex    " << decryptedTextHex << endl;
        
        // TODO OI: XOR currentTextBin with xorer
        unsigned char xoredWithPlainTextBin[64];
        xorOperation(decryptedTextBin, xorer, 64, xoredWithPlainTextBin);
        unsigned char xoredWithPlainTextHex[16];
        binaryToHex(xoredWithPlainTextBin, xoredWithPlainTextHex);

        unsigned char plainTextChar[8];
        hexToChar(xoredWithPlainTextHex, plainTextChar);
        copy(plainText+(i*8), plainText+(i*8) + 8, plainTextChar);
        copy(xorer, xorer + 64, currentTextBin);
    }

    return plainText;
}

void DES::encryptCBC_Continous(unsigned char *plainText, uint lenPlainText, unsigned char result[])
{
    const unsigned char *key = this->KEY;

    uint fix_len = uint(ceil(float(lenPlainText)/float(8))*8);
    unsigned char plainTextFixed[fix_len];
    fill(plainTextFixed, plainTextFixed + fix_len, '\0');
    copy(plainText, plainText + lenPlainText, plainTextFixed);

    unsigned char *cipherText = new unsigned char[fix_len];
    fill(cipherText, cipherText + fix_len, '\0');

    unsigned char currentText[8];
    unsigned char currentTextHex[16];
    unsigned char currentTextBin[64];
    unsigned char keyHex[16];
    unsigned char xoredWithPlainTextBin[64];
    unsigned char xoredWithPlainTextHex[16];
    unsigned char cipherTextBin[64];
    unsigned char cipherTextChar[8];
    for(int i = 0; i < ceil(float(lenPlainText)/float(8)); i++) {
        fill(currentText, currentText + 8, '\0');
        copy(plainTextFixed+(i*8), plainTextFixed+(i*8) + 8, currentText);
        // cout << currentText << endl;

        charToHex(currentText, currentTextHex);
        hexToBinary(currentTextHex, currentTextBin);

        charToHex(key, keyHex);

        xorOperation(currentTextBin, CBC_XORER, 64, xoredWithPlainTextBin);
        binaryToHex(xoredWithPlainTextBin, xoredWithPlainTextHex);

        unsigned char cipherTextHex[16];
        encryptBlock(xoredWithPlainTextHex, keyHex, cipherTextHex);
        hexToBinary(cipherTextHex, cipherTextBin);
        
        hexToChar(cipherTextHex, cipherTextChar);
        copy(cipherTextChar, cipherTextChar + 8, cipherText+(i*8));
        copy(cipherTextBin, cipherTextBin + 64, CBC_XORER);
    }

    copy(cipherText, cipherText + fix_len, result);
}

void DES::decryptCBC_Continous(unsigned char *cipherText, uint lenCipherText, unsigned char result[])
{
    const unsigned char *key = this->KEY;

    uint fix_len = uint(ceil(float(lenCipherText)/float(8))*8);
    unsigned char cipherTextFixed[fix_len];
    fill(cipherTextFixed, cipherTextFixed + fix_len, '\0');
    memcpy(cipherTextFixed, cipherText, fix_len);

    unsigned char *plainText = new unsigned char[fix_len];
    fill(plainText, plainText + fix_len, '\0');

    unsigned char currentText[8];
    unsigned char currentTextHex[16];
    unsigned char currentTextBin[64];
    unsigned char keyHex[16];
    unsigned char decryptedTextBin[64];
    unsigned char xoredWithPlainTextBin[64];
    unsigned char xoredWithPlainTextHex[16];
    unsigned char plainTextChar[8];
    for(int i = 0; i < ceil(float(lenCipherText)/float(8)); i++) {
        fill(currentText, currentText + 8, '\0');
        memcpy(currentText, cipherTextFixed+(i*8), 8);

        charToHex(currentText, currentTextHex);
        hexToBinary(currentTextHex, currentTextBin);

        charToHex(key, keyHex);
        
        unsigned char decryptedTextHex[16];
        decryptBlock(currentTextHex, keyHex, decryptedTextHex);
        hexToBinary(decryptedTextHex, decryptedTextBin);
        
        xorOperation(decryptedTextBin, CBC_XORER, 64, xoredWithPlainTextBin);
        binaryToHex(xoredWithPlainTextBin, xoredWithPlainTextHex);

        hexToChar(xoredWithPlainTextHex, plainTextChar);
        memcpy(plainText+(i*8), plainTextChar, 8);
        copy(currentTextBin, currentTextBin + 64, CBC_XORER);
    }
    
    copy(plainText, plainText + fix_len, result);
}

void DES::flipRoundKeys()
{
    int size = 16;

    unsigned char temp[48];
    for(int i=0, j=size-1; i<size/2; i++, j--){
        copy(this->compressedKeysBin[i], this->compressedKeysBin[i] + 48, temp);
        copy(this->compressedKeysBin[j], this->compressedKeysBin[j] + 48, this->compressedKeysBin[i]);
        copy(temp, temp + 48, this->compressedKeysBin[j]);
    }
    this->isDecrypting = !this->isDecrypting;
}