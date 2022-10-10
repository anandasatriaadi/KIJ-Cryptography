#include <bitset>
#include <cstring>
#include <iostream>
#include <string>
#include <bits/stdc++.h>

using namespace std;

class AES {
    private:
        const uint N_BLOCK      = 4;
        const uint BLOCK_LEN    = 16;
        const uint N_COLUMN     = 4;
        const uint N_ROUND      = 10;
        unsigned char *IV = (unsigned char*)"B3TUTUG1L1M4NUK";
        unsigned char *KEY = (unsigned char*)"4Y4MB4K4RB3TUTU";
        // const unsigned char *IV = new unsigned char [0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76];
        // const unsigned char *KEY = new unsigned char [0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8];

        const unsigned char SBOX[16][16] = {
            {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76},
            {0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0},
            {0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15},
            {0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75},
            {0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84},
            {0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf},
            {0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8},
            {0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2},
            {0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73},
            {0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb},
            {0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79},
            {0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08},
            {0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a},
            {0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e},
            {0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf},
            {0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16}
        };
        
        const unsigned char INVERSE_SBOX[16][16] = {
            {0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb},
            {0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb},
            {0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e},
            {0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25},
            {0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92},
            {0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84},
            {0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06},
            {0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b},
            {0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73},
            {0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e},
            {0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b},
            {0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4},
            {0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f},
            {0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef},
            {0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61},
            {0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d}
        };

    public:
        explicit AES();
        unsigned char **generate2DUC(uint rows, uint col);
        void print2dUC(unsigned char **arr, uint rows, uint col);
        unsigned char mulBy2(unsigned char a);
        unsigned char invMultiply(unsigned char a, unsigned char b);
        void keyExpansion(const unsigned char *key, unsigned char **keyRounds);
        void addRoundKey(unsigned char **state, unsigned char *roundKey);

        // Encrypt Utils
        void subBytes(unsigned char **input);
        void shiftRow(unsigned char **input, int rowNumber, int shift);
        void shiftRows(unsigned char **input);
        void mixColumns(unsigned char **input);
        void encryptBlock(unsigned char *plainText, uint blockNum, unsigned char **keyRounds, unsigned char *cipherText, uint plainLen);

        // Decrypt Utils
        void invSubBytes(unsigned char **input);
        void invShiftRows(unsigned char **input);
        void invMixColumns(unsigned char **input);
        void decryptBlock(unsigned char *cipherText, uint blockNum, unsigned char **keyRounds, unsigned char *plainText);

        // The operation mode
        void xorBlock(unsigned char *blockA, uint blockNum, unsigned char blockB[]);
        unsigned char *encryptECB(unsigned char *plainText, uint lenPlainText);
        unsigned char *decryptECB(unsigned char *cipherText, uint lenCipherText);
        unsigned char *encryptCBC(unsigned char *plainText, uint lenPlainText);
        unsigned char *decryptCBC(unsigned char *cipherText, uint lenCipherText);
};
    
AES::AES() {

}

// ======== START ::: UTILITY FUNCTIONS ========
unsigned char ** AES::generate2DUC(uint rows, uint col){
    unsigned char **res; 
    res = new unsigned char *[rows];
    for (int i = 0; i < rows; i++){
        res[i] = new unsigned char[col];
    }
    return res;
}

void AES::print2dUC(unsigned char **arr, uint rows, uint col){
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < col; j++){
            printf("%2X ", arr[i][j]);
        }
    // cout << endl;
    }
    cout << endl;
}

unsigned char AES::mulBy2(unsigned char a){
    unsigned char res = 0x00;
    res ^= a << 1;
    if (a >> 7){
        res ^= 0x1b;
    }
    return res;
}

unsigned char AES::invMultiply(unsigned char a, unsigned char b){
    int res = 0x00;
    /* 
    x*9=(((x*2)*2)*2)+x
    x*11=((((x*2)*2)+x)*2)+x
    x*13=((((x*2)+x)*2)*2)+x
    x*14=((((x*2)+x)*2)+x)*2
    Source :: https://crypto.stackexchange.com/a/2570
    */
    if(b == 0x09) {
        res = mulBy2(mulBy2(mulBy2(a))) ^ a;
    } else if(b == 0x0b) {
        res = mulBy2(mulBy2(mulBy2(a)) ^ a) ^ a;
    } else if(b == 0x0d) {
        res = mulBy2(mulBy2(mulBy2(a)^a)) ^ a;
    } else if(b == 0x0e) {
        res = mulBy2(mulBy2(mulBy2(a)^a) ^ a);
    }

    return res;
}
// ======== END ::: UTILITY FUNCTIONS ========

// ======== START ::: ENCRYPTION PART ========
void AES::subBytes(unsigned char **input){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            input[i][j] = SBOX[input[i][j] / 16][input[i][j] % 16];
        }
    }
}

void AES::shiftRow(unsigned char **input, int rowNumber, int shift)
{
    unsigned char tempRow[4];

    for (int i = 0; i < 4; i++) {
        tempRow[i] = input[rowNumber][(i + shift) % 4];
    }
    for (int i = 0; i < 4; i++) {
        input[rowNumber][i] = tempRow[i];
    }
}

void AES::shiftRows(unsigned char **input)
{
    for (int i = 1; i < 4; i++) {
        shiftRow(input, i, i);
    }
}

void AES::mixColumns(unsigned char **input)
{
    unsigned char mul_constant[4][4] = {
        {0x02, 0x03, 0x01, 0x01},
        {0x01, 0x02, 0x03, 0x01},
        {0x01, 0x01, 0x02, 0x03},
        {0x03, 0x01, 0x01, 0x02}};

    // First loops through the initial state
    unsigned char **result = generate2DUC(4, 4);
    for (int i = 0; i < 4; i++) {
        // printf("COL %d\n", i);
        // Then loops through the multiplication constant
        for (int j = 0; j < 4; j++) {
            unsigned char temp2 = 0;
            // cout << "|";
            for (int k = 0; k < 4; k++) {
                if (mul_constant[j][k] == 0x01) {
                    temp2 ^= input[k][i];
                } else {
                    temp2 ^= mulBy2(input[k][i]);
                    
                    if (mul_constant[j][k] == 0x03) {
                        temp2 ^= input[k][i];
                    }
                }
            }
            result[j][i] = temp2;
        }
    }

    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            input[i][j] = result[i][j];
        }
    }
}
// ======== END ::: ENCRYPTION PART ========

// ======== START ::: DECRYPTION PART ========
void AES::invSubBytes(unsigned char **input){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            input[i][j] = INVERSE_SBOX[input[i][j] / 16][input[i][j] % 16];
        }
    }
}

void AES::invShiftRows(unsigned char **input) {
    for (int i = 1; i < 4; i++) {
        shiftRow(input, i, 4-i);
    }
}

void AES::invMixColumns(unsigned char **input) {
    // BME
    // print2dUC(input, 4, 4);
    unsigned char mul_constant[4][4] = {
        {0x0E, 0x0B, 0x0D, 0x09},
        {0x09, 0x0E, 0x0B, 0x0D},
        {0x0D, 0x09, 0x0E, 0x0B},
        {0x0B, 0x0D, 0x09, 0x0E},
    };

    // First loops through the initial state
    unsigned char **result = generate2DUC(4, 4);
    for (int i = 0; i < 4; i++) {
        // printf("COL %d\n", i);
        // Then loops through the multiplication constant
        for (int j = 0; j < 4; j++) {
            unsigned char temp = 0;
            for (int k = 0; k < 4; k++) {
                temp ^= invMultiply(input[k][i], mul_constant[j][k]);
            }
            result[j][i] = temp;
        }
    }

    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            input[i][j] = result[i][j];
        }
    }

}


// ======== END ::: DECRYPTION PART ========

void AES::keyExpansion(const unsigned char *key, unsigned char **keyRounds) {
    // cout << this->N_COLUMN << endl;
    unsigned char word[4]; // words
    unsigned char rc[4] = {0, 0, 0, 0};

    // Assign the key to first round of keyRounds
    // alias, keyRounds[0] = key
    keyRounds[0] = (unsigned char*)key;
    // UI i = 0;

    for (int i = 1; i < N_ROUND + 1; i++) {
        // Prepare words
        // W = Last W of Key before
        for (int j = 0; j < 4; j++) {
            word[j] = keyRounds[i - 1][N_BLOCK * 4 - (4 - j)];
        }

        // ======== START ::: G-FUNCTION ========
        {
            unsigned char temp = word[0];
            word[0] = word[1];
            word[1] = word[2];
            word[2] = word[3];
            word[3] = temp;
        }

        // S-Box-in 
        {
            for (int j = 0; j < 4; j++) {
                word[j] = SBOX[word[j] / 16][word[j] % 16];
            }
        }

        // Cari RCON
        {
            const unsigned char RC_LIST[N_ROUND+1]{0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36};
            rc[0] = RC_LIST[i-1];
        }

        // W sama RC di XOR
        {
            for (int j = 0; j < 4; j++) {
                word[j] = word[j] ^ rc[j];
            }
        }
        // ======== END ::: G-FUNCTION ========

        // Masukkin ke keyRounds rounds sekarang ( YANG DI G FUNCTION CM W0 aja )
        for(int j = 0; j < 4; j++) {
            keyRounds[i][j] = keyRounds[i-1][j] ^ word[j];
        }

        // ======== START ::: XOR word 1 - 3 with previous xor results ========
        for (int j = 1; j <= 3; j++)
        {
            for(int k = 0; k < 4; k++) {
                keyRounds[i][j*4+k] = keyRounds[i-1][j*4+k] ^ keyRounds[i][(j-1)*4+k];
            }
        }
    }
}

void AES::addRoundKey(unsigned char **state, unsigned char *roundKey){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < N_BLOCK; j++){
            state[i][j] ^= roundKey[i + 4 * j];
        }
    }    
}

void AES::encryptBlock(unsigned char *plainText, uint blockNum, unsigned char **keyRounds, unsigned char *cipherText, uint plainLen){
    unsigned char **state = generate2DUC(4, N_BLOCK);
    
    // init state
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < N_BLOCK; j++){
            uint index = blockNum * BLOCK_LEN + (i + 4 * j);
            if (index <= plainLen) {
                state[i][j] = plainText[index];
            }else{
                state[i][j] = '\0';
            }
        }
    }
    
    // do addition layer first
    addRoundKey(state, keyRounds[0]);
    // for each round do SubBytes, ShiftRows, MixColumns, AddRoundKey
    for (int round = 1; round < N_ROUND; round++){
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, keyRounds[round]);
    }

    subBytes(state);
    shiftRows(state);
    addRoundKey(state, keyRounds[10]);

    // write to cipherText
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < N_BLOCK; j++){
            uint index = blockNum * BLOCK_LEN + (i + 4 * j);
            cipherText[index] = state[i][j];
        }
    }
}

void AES::decryptBlock(unsigned char *cipherText, uint blockNum, unsigned char **keyRounds, unsigned char *plainText){
    unsigned char **state = generate2DUC(4, N_BLOCK);
    
    // init state
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < N_BLOCK; j++){
            uint index = blockNum * BLOCK_LEN + (i + 4 * j);
            state[i][j] = cipherText[index];
        }
    }
    
    addRoundKey(state, keyRounds[10]);

    for (int round = N_ROUND-1; round > 0; round--){
        invShiftRows(state);
        invSubBytes(state);
        addRoundKey(state, keyRounds[round]);
        invMixColumns(state);
    }

    invShiftRows(state);
    invSubBytes(state);
    addRoundKey(state, keyRounds[0]);

    // write to plainText
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < N_BLOCK; j++){
            uint index = blockNum * BLOCK_LEN + (i + 4 * j);
            plainText[index] = state[i][j];
        }
    }
}

// ======== START ::: OPERATION MODE ========

void AES::xorBlock(unsigned char *blockA, uint blockNum, unsigned char blockB[]){
    for (uint i = blockNum * BLOCK_LEN; i < (blockNum + 1 ) * BLOCK_LEN; i++){
        blockA[i] ^= blockB[i % 16];
    }
}

unsigned char *AES::encryptECB(unsigned char *plainText, uint lenPlainText){
    uint fix_len = uint(ceil(float(lenPlainText)/float(16))*16);
    unsigned char *cipherText = new unsigned char[fix_len];

    memset(cipherText, '\0', sizeof(unsigned char) * fix_len);

    unsigned char **keyRounds = generate2DUC(N_ROUND+1, N_BLOCK*4);
    keyExpansion(KEY, keyRounds);

    for(uint blockNum = 0; blockNum < fix_len/16; blockNum++){
        encryptBlock(plainText, blockNum, keyRounds, cipherText, lenPlainText);
    }
    
    delete[] keyRounds;
    return cipherText;
}

unsigned char *AES::decryptECB(unsigned char *cipherText, uint lenCipherText){
    unsigned char **keyRounds = generate2DUC(N_ROUND+1, N_BLOCK*4);
    keyExpansion(KEY, keyRounds);
    
    uint fix_len = uint(ceil(float(lenCipherText)/float(16))*16);
    unsigned char *decryptedCT = new unsigned char[fix_len];
    memset(decryptedCT, '\0', sizeof(unsigned char) * fix_len);

    for(uint blockNum = 0; blockNum < fix_len/16; blockNum++){
        decryptBlock(cipherText, blockNum, keyRounds, decryptedCT);
    }
    
    delete[] keyRounds;
    return decryptedCT;
}

unsigned char *AES::encryptCBC(unsigned char *plainText, uint lenPlainText){
    // Copy plainText
    unsigned char plainTextCopy[lenPlainText];
    memcpy(plainTextCopy, plainText, sizeof(unsigned char) * lenPlainText);

    unsigned char **keyRounds = generate2DUC(N_ROUND+1, N_BLOCK*4);
    keyExpansion(KEY, keyRounds);

    unsigned char *xorer = new unsigned char[BLOCK_LEN];
    memcpy(xorer, IV, BLOCK_LEN);

    uint fix_len = uint(ceil(float(lenPlainText)/float(16))*16);
    unsigned char *cipherText = new unsigned char[fix_len];
    memset(cipherText, '\0', sizeof(unsigned char) * fix_len);
    
    unsigned char *temp = new unsigned char[BLOCK_LEN];
    for(uint blockNum = 0; blockNum < fix_len/16; blockNum++){
        xorBlock(plainTextCopy, blockNum, xorer);        
        encryptBlock(plainTextCopy, blockNum, keyRounds, cipherText, lenPlainText);
        memcpy(xorer, cipherText + (blockNum * BLOCK_LEN), BLOCK_LEN);
    }
    
    delete[] keyRounds;
    return cipherText;
}

unsigned char *AES::decryptCBC(unsigned char *cipherText, uint lenCipherText){
    unsigned char **keyRounds = generate2DUC(N_ROUND+1, N_BLOCK*4);
    keyExpansion(KEY, keyRounds);

    unsigned char *xorer = new unsigned char[BLOCK_LEN];
    memcpy(xorer, IV, BLOCK_LEN);

    uint fix_len = uint(ceil(float(lenCipherText)/float(16))*16);
    unsigned char *decryptedCT = new unsigned char[fix_len];
    memset(decryptedCT, '\0', sizeof(unsigned char) * fix_len);

    for(uint blockNum = 0; blockNum < fix_len/16; blockNum++){
        decryptBlock(cipherText, blockNum, keyRounds, decryptedCT);
        xorBlock(decryptedCT, blockNum, xorer);
        memcpy(xorer, cipherText + (blockNum * BLOCK_LEN), BLOCK_LEN);
    }
    
    delete[] keyRounds;
    return decryptedCT;
}



// ======== END ::: OPERATION MODE ========
