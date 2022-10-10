#include <bitset>
#include <cstring>
#include <iostream>
#include <string>
#include <bits/stdc++.h>

using namespace std;

class DES {
    private:
        unsigned char **compressedKeysBin;

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

    public:
        explicit DES();
        void generateSubKeys(const unsigned char *text);
        unsigned char *encryptBlock(unsigned char *plainText, const unsigned char *text, bool isDecrypting);

        // untility class for key encryption
        unsigned char *hexToBinary(unsigned char *text); 
        unsigned char *binaryToHex(unsigned char *text); 
        unsigned char *permuteKeyWithParity(const unsigned char *text); 
        unsigned char *shiftLeft(unsigned char *temp, int round);
        unsigned char *permuteKeyWithKeyComposition(const unsigned char *text); 
        unsigned char *permuteWithExpTab(unsigned char *text);
        unsigned char *xorOperation(unsigned char *text1, unsigned char *text2, uint size);
        unsigned char *permuteWithPermutationTab(const unsigned char *text);
        unsigned char *permuteWithFinalPermutationTab(unsigned char *text);
        unsigned char *decryptBlock(unsigned char *cipherText, const unsigned char *key);

        // Decrypt
        unsigned char *permuteTextWithParity(const unsigned char *text);

        // Utility class for text conversion
        unsigned char **generate2DUC(uint rows, uint col);
        unsigned char *charToHex(const unsigned char *text);
        unsigned char *hexToChar(unsigned char *text);
};

DES::DES() {
    this->compressedKeysBin = generate2DUC(16, 48);
}

/* ======== START ::: UTILITY METHOD ======== */
unsigned char ** DES::generate2DUC(uint rows, uint col){
    unsigned char **res; 
    res = new unsigned char *[rows];
    for (int i = 0; i < rows; i++){
        res[i] = new unsigned char[col];
    }
    return res;
}

unsigned char *DES::hexToBinary(unsigned char *text)
{
    map<unsigned char, unsigned char *> map_hex;
    map_hex['0'] = (unsigned char *)"0000";
    map_hex['1'] = (unsigned char *)"0001";
    map_hex['2'] = (unsigned char *)"0010";
    map_hex['3'] = (unsigned char *)"0011";
    map_hex['4'] = (unsigned char *)"0100";
    map_hex['5'] = (unsigned char *)"0101";
    map_hex['6'] = (unsigned char *)"0110";
    map_hex['7'] = (unsigned char *)"0111";
    map_hex['8'] = (unsigned char *)"1000";
    map_hex['9'] = (unsigned char *)"1001";
    map_hex['A'] = (unsigned char *)"1010";
    map_hex['B'] = (unsigned char *)"1011";
    map_hex['C'] = (unsigned char *)"1100";
    map_hex['D'] = (unsigned char *)"1101";
    map_hex['E'] = (unsigned char *)"1110";
    map_hex['F'] = (unsigned char *)"1111";

    unsigned char *res = new unsigned char[64];
    for(int i=0; i<16; i++){
        memcpy(res + i*4, map_hex[text[i]], 4);
    }

    return res;
} 
unsigned char *DES::binaryToHex(unsigned char *text)
{
    map<unsigned long, unsigned char> map_bin;
    map_bin[0] = (unsigned char)'0';
	map_bin[1] = (unsigned char)'1';
	map_bin[2] = (unsigned char)'2';
	map_bin[3] = (unsigned char)'3';
	map_bin[4] = (unsigned char)'4';
	map_bin[5] = (unsigned char)'5';
	map_bin[6] = (unsigned char)'6';
	map_bin[7] = (unsigned char)'7';
	map_bin[8] = (unsigned char)'8';
	map_bin[9] = (unsigned char)'9';
	map_bin[10] = (unsigned char)'A';
	map_bin[11] = (unsigned char)'B';
	map_bin[12] = (unsigned char)'C';
	map_bin[13] = (unsigned char)'D';
	map_bin[14] = (unsigned char)'E';
	map_bin[15] = (unsigned char)'F';

    unsigned char *temp = new unsigned char[5];
    unsigned char *res = new unsigned char[17];
    memset(temp, '\0', sizeof(unsigned char) * 5);
    for(int i = 0; i < 16; i++){
        memcpy(temp, text + i*4, 4);
        res[i] = map_bin[bitset<4>(temp).to_ulong()];
    }
    return res;
} 
/* ======== END ::: UTILITY METHOD ======== */

/* ======== START ::: ENCRYPT KEY ======== */
unsigned char *DES::permuteKeyWithParity(const unsigned char *key)
{
    unsigned char *res = new unsigned char[56];
    for(int i=0; i<56; i++){
        res[i] = key[this->KEY_PARITY[i]-1];
    }

    return res;
} 
unsigned char *DES::shiftLeft(unsigned char *temp, int round)
{
    unsigned char *res = new unsigned char[28];
    
    for(int i = 0; i < this->AMOUNT_SHIFT_PER_ROUND[round]; i++){
        // cout << "BEFORE SHIFT " << i << " ::: " << temp << endl;
        int j = 1;
        for(; j < 28; j++){
            res[j-1] = temp[j];
        }
        // make the new res as new temp 
        res[j-1] = temp[0];
        memset(temp, '\0', sizeof(unsigned char) * 28);
        memcpy(temp, res, 28);
    }
    return res;
} 
unsigned char *DES::permuteKeyWithKeyComposition(const unsigned char *key)
{
    unsigned char *res = new unsigned char[48];
    for(int i=0; i<48; i++){
        res[i] = key[this->INITIAL_PERMUTATION_TABLE[i]-1];
    }

    return res;
} 
/* ======== END ::: ENCRYPT KEY ======== */

/* ======== START ::: ENCRYPT & DECRYPT ======== */
unsigned char *DES::permuteTextWithParity(const unsigned char *text)
{
    unsigned char *res = new unsigned char[64];
    for(int i=0; i<64; i++){
        res[i] = text[this->TEXT_PARITY[i]-1];
    }

    return res;
} 
unsigned char *DES::permuteWithExpTab(unsigned char *text)
{
    unsigned char *res = new unsigned char[48];
    for(int i=0; i<48; i++){
        res[i] = text[this->EXPANSION_TABLE[i]-1];
    }

    return res;
} 
unsigned char *DES::xorOperation(unsigned char *text1, unsigned char *text2, uint size)
{
    unsigned char *res = new unsigned char[size];

    for(int i=0; i<size; i++){
        res[i] = text1[i] != text2[i] ? '1' : '0';
    }

    return res;
}
unsigned char *DES::permuteWithPermutationTab(const unsigned char *text)
{
    unsigned char *res = new unsigned char[32];
    for(int i=0; i<32; i++){
        res[i] = text[this->PERMUTATION_TABLE[i]-1];
    }

    return res;
} 
unsigned char *DES::permuteWithFinalPermutationTab(unsigned char *text)
{
    unsigned char *res = new unsigned char[64];
    for(int i=0; i<64; i++){
        res[i] = text[this->INVERSE_PERMUTATION_TABLE[i]-1];
    }

    return res;
} 
/* ======== END ::: ENCRYPT & DECRYPT ======== */

void DES::generateSubKeys(const unsigned char *key)
{
    // 1. make key from hex to binary 
    unsigned char *binaryKey = hexToBinary((unsigned char *)key);

    // 2. initial permutation 
    unsigned char *ipRes = permuteKeyWithParity(binaryKey); 

    // 3. splitting to LPT and RPT 
    unsigned char *lpt = new unsigned char[28];
    unsigned char *rpt = new unsigned char[28];
    memcpy(lpt, ipRes, 28);
    memcpy(rpt, ipRes + 28, 28);

    // 4. key transformation
    for(int i=0; i<16; i++){
        // shift left 
        lpt = shiftLeft(lpt, i);
        rpt = shiftLeft(rpt, i);

        unsigned char *combine = new unsigned char[56];
        memcpy(combine, lpt, 28);
        memcpy(combine + 28, rpt, 28);

        // get round keys from compression permutation (from 56 bits to 48 bits)
        unsigned char *compressedKey = permuteKeyWithKeyComposition(combine);
        this->compressedKeysBin[i] = compressedKey;
    }
}
unsigned char *DES::encryptBlock(unsigned char *plainText, const unsigned char *key, bool isDecrypting)
{

    cout << plainText << "   " << key << endl;
    if(!isDecrypting)
        generateSubKeys(key);

    // 1. make text from hex to binary 
    unsigned char *binaryText = hexToBinary(plainText);
    
    // 2. initial permutation for text 
    unsigned char *permutatedText = permuteTextWithParity(binaryText);

    // 3. split text 
    unsigned char *lpt = new unsigned char[32];
    memcpy(lpt, permutatedText, 32);

    unsigned char *rpt = new unsigned char[32];
    memcpy(rpt, permutatedText + 32, 32);

    for(int i=0; i<16; i++){
        // 4. expand with D-Box (from 32 bits to 48 bits)
        unsigned char *rptExpanded = permuteWithExpTab(rpt);

        // xor with compressed binary key from same round 
        unsigned char *xored = xorOperation(this->compressedKeysBin[i], rptExpanded, 48);        
        
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

        unsigned char *permutedSBox = permuteWithPermutationTab((unsigned char*)sBoxRes.c_str());

        xored = xorOperation(permutedSBox, lpt, 32);

        lpt = xored;

        if(i != 15){
            unsigned char *temp = lpt;
            lpt = rpt;
            rpt = temp;
        }
    }
 
    unsigned char *fullText = new unsigned char[64];
    memcpy(fullText, lpt, 32);
    memcpy(fullText + 32, rpt, 32);

    unsigned char *cipherTextBin = permuteWithFinalPermutationTab(fullText);
    unsigned char *cipherText = binaryToHex(cipherTextBin);

    return cipherText;
}
unsigned char *DES::decryptBlock(unsigned char *cipherText, const unsigned char *key)
{
    int size = 16;
    // for(int i=0; i<16;i++){
    //     cout << this->compressedKeysBin[i] << endl;
    // }
    // reverse round keys 
    unsigned char *temp = new unsigned char[48];
    for(int i=0, j=size-1; i<size/2; i++, j--){
        memcpy(temp, this->compressedKeysBin[i], 48);
        memcpy(this->compressedKeysBin[i], this->compressedKeysBin[j], 48);
        memcpy(this->compressedKeysBin[j], temp, 48);
    }
    // cout<< "reversed" << endl;
    // for(int i=0; i<16;i++){
    //     cout << this->compressedKeysBin[i] << endl;
    // }

    return encryptBlock(cipherText, key, true);
}

unsigned char *DES::charToHex(const unsigned char *text) {
    unsigned char *hex = new unsigned char[16];
    unsigned char *binary = new unsigned char[64];
    for (int i = 0; i < 8; i++) {
        memcpy(binary + (i * 8), bitset<8>(text[i]).to_string().c_str(), 8);
    }
    return binaryToHex(binary);
}

unsigned char *DES::hexToChar(unsigned char *text) {
    unsigned char *binary = hexToBinary(text);
    
    string temp = "";
    unsigned char *res = new unsigned char[8];
    for (int i = 0; i < 64; i++) {
        temp += binary[i];
        if((i + 1) % 8 == 0) {
            res[i / 8] = (unsigned char)bitset<8>(temp).to_ulong();
            temp = "";
        }
    }
    return res;
}
