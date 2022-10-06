#include <bitset>
#include <cstring>
#include <iostream>
#include <string>
#include <bits/stdc++.h>

using namespace std;

const unsigned int N_ROUND = 10;
const unsigned int N_BLOCK = 4;
const unsigned int N_COLUMN = 4;
const unsigned int BLOCK_LEN = 16;

const unsigned char SBOX[16][16] = {
    {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b,
     0xfe, 0xd7, 0xab, 0x76},
    {0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf,
     0x9c, 0xa4, 0x72, 0xc0},
    {0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1,
     0x71, 0xd8, 0x31, 0x15},
    {0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2,
     0xeb, 0x27, 0xb2, 0x75},
    {0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3,
     0x29, 0xe3, 0x2f, 0x84},
    {0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39,
     0x4a, 0x4c, 0x58, 0xcf},
    {0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f,
     0x50, 0x3c, 0x9f, 0xa8},
    {0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21,
     0x10, 0xff, 0xf3, 0xd2},
    {0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d,
     0x64, 0x5d, 0x19, 0x73},
    {0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14,
     0xde, 0x5e, 0x0b, 0xdb},
    {0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62,
     0x91, 0x95, 0xe4, 0x79},
    {0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea,
     0x65, 0x7a, 0xae, 0x08},
    {0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f,
     0x4b, 0xbd, 0x8b, 0x8a},
    {0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9,
     0x86, 0xc1, 0x1d, 0x9e},
    {0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9,
     0xce, 0x55, 0x28, 0xdf},
    {0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f,
     0xb0, 0x54, 0xbb, 0x16}};


unsigned char ** generate2DUC(uint rows, uint col){
    unsigned char **res; 
    res = new unsigned char *[rows];
    for (int i = 0; i < rows; i++){
        res[i] = new unsigned char[col];
    }
    return res;
}

void print2dUC(unsigned char **arr, uint rows, uint col){
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < col; j++){
            printf("%2X ", arr[i][j]);
        }
    }
    cout << endl;
}

void subBytes(unsigned char **input){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            input[i][j] = SBOX[input[i][j] / 16][input[i][j] % 16];
        }
    }
}

void shiftRow(unsigned char **input, int rowNumber)
{
    unsigned char tempRow[4];

    for (int i = 0; i < 4; i++) {
        tempRow[i] = input[rowNumber][(i + rowNumber) % 4];
    }
    for (int i = 0; i < 4; i++) {
        input[rowNumber][i] = tempRow[i];
    }
}

void shiftRows(unsigned char **input)
{
    for (int i = 1; i < 4; i++) {
        shiftRow(input, i);
    }
}

void mixColumns(unsigned char **input)
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
                    // checks whether the leftmost bit is 1
                    temp2 ^= input[k][i] << 1;
                    if (input[k][i] >> 7) {
                        temp2 ^= 0x1b;
                    }

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

void keyExpansion(const unsigned char *key, unsigned char **keyRounds) {
    // cout << this->N_COLUMN << endl;
    unsigned char word[4]; // words
    unsigned char rc[4] = {0, 0, 0, 0};

    // Assign the key to first round of keyRounds
    // alias, keyRounds[0] = key
    keyRounds[0] = (unsigned char*)key;
    // UI i = 0;

    for (int i = 1; i < N_ROUND+1; i++) {
        // Prepare words
        // W = Last W of Key before
        for (int j = 0; j < 4; j++) {
            word[j] = keyRounds[i-1][N_BLOCK*4-(4-j)];
        }

        // ========================================================================
        //   START ::: G Function
        // ========================================================================
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
        // ========================================================================
        //   END ::: G Function
        // ========================================================================

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

void addRoundKey(unsigned char **state, unsigned char *roundKey){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < N_BLOCK; j++){
            state[i][j] ^= roundKey[i + 4 * j];
        }
    }    
}

void encryptBlock(unsigned char *plainText, uint blockNum, const unsigned char *key, unsigned char *cipherText){
    unsigned char **keyRounds = generate2DUC(N_ROUND+1, N_BLOCK*4);
    keyExpansion(key, keyRounds);

    unsigned char **state = generate2DUC(4, N_BLOCK);
    
    // init state
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < N_BLOCK; j++){
            uint index = blockNum * BLOCK_LEN + (i + 4 * j);
            if (index <= strlen((char*)plainText)) {
                state[i][j] = plainText[index];
            }else{
                state[i][j] = '\0';
            }
        }
    }
    print2dUC(state, 4, N_BLOCK);

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

// AES main function for testing
int main()
{
    unsigned char* plainText = (unsigned char*)"hari ini aku ke penyet bersama";
    const unsigned char* key = (const unsigned char*)"nasipadangenakk";

    uint pt_len = strlen((char*)plainText)+1;
    cout << "strlen => " << pt_len << endl;

    // repair the pt_len to divisible by 16
    uint fix_len = uint(ceil(float(pt_len)/float(16))*16);
    unsigned char *cipherText;
    cipherText = new unsigned char[fix_len];

    memset(cipherText, '\0', sizeof(unsigned char) * fix_len);

    cout << "fix_len => " << fix_len << endl;
    cout << "PLAIN TEXT => " << plainText << endl;
    for(uint blockNum = 0; blockNum < fix_len/16; blockNum++){
        encryptBlock(plainText, blockNum, key, cipherText);
    }

    cout << "CIPHER TEXT => ";
    for (uint i = 0; i < fix_len; i++){
        printf("%X ", cipherText[i]);
    }
    cout << endl;

    return 0;

    /*
    */
}