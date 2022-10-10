#include <bitset>
#include <cstring>
#include <iostream>
#include <string>
#include <bits/stdc++.h>

using namespace std;

class DES {
    private:
        string compressedKeysBin[16];
        string compressedKeysHex[16]; 

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
        void encryptBlock(unsigned char *plainText, const unsigned char *text, bool isDecrypting);

        // untility class for key encryption
        string hexToBinary(const unsigned char *text); 
        string binaryToHex(string text); 
        string permuteKeyWithParity(const unsigned char *text); 
        string shiftLeft(string temp, int round);
        string permuteKeyWithKeyComposition(const unsigned char *text); 
        string permuteWithExpTab(const unsigned char *text);
        string xorOperation(string text1, string text2);
        string permuteWithPermutationTab(const unsigned char *text);
        string permuteWithFinalPermutationTab(const unsigned char *text);
        void decryptBlock(unsigned char *cipherText, const unsigned char *key);

        // Decrypt
       string permuteTextWithParity(const unsigned char *text); 
};

DES::DES() {

}

/* ======== START ::: UTILITY METHOD ======== */
string DES::hexToBinary(const unsigned char *text)
{
    map<unsigned char, string> map_hex;
    map_hex['0'] = "0000";
    map_hex['1'] = "0001";
    map_hex['2'] = "0010";
    map_hex['3'] = "0011";
    map_hex['4'] = "0100";
    map_hex['5'] = "0101";
    map_hex['6'] = "0110";
    map_hex['7'] = "0111";
    map_hex['8'] = "1000";
    map_hex['9'] = "1001";
    map_hex['A'] = "1010";
    map_hex['B'] = "1011";
    map_hex['C'] = "1100";
    map_hex['D'] = "1101";
    map_hex['E'] = "1110";
    map_hex['F'] = "1111";

    string res = "";
    for(int i=0; i<16; i++){
        res += map_hex[text[i]];
    }

    return res;
} 
string DES::binaryToHex(string text)
{
    map<string, string> map_bin;
    map_bin["0000"] = "0";
	map_bin["0001"] = "1";
	map_bin["0010"] = "2";
	map_bin["0011"] = "3";
	map_bin["0100"] = "4";
	map_bin["0101"] = "5";
	map_bin["0110"] = "6";
	map_bin["0111"] = "7";
	map_bin["1000"] = "8";
	map_bin["1001"] = "9";
	map_bin["1010"] = "A";
	map_bin["1011"] = "B";
	map_bin["1100"] = "C";
	map_bin["1101"] = "D";
	map_bin["1110"] = "E";
	map_bin["1111"] = "F";

    string res = "";
    string temp = "";
    for(int i=0; i<text.length(); i++){
        temp += text[i];
        if((i+1)%4 == 0){
            res += map_bin[temp];
            temp = "";
        }
    }
    return res;
} 
/* ======== END ::: UTILITY METHOD ======== */

/* ======== START ::: ENCRYPT KEY ======== */
string DES::permuteKeyWithParity(const unsigned char *key)
{
    string res = "";
    for(int i=0; i<56; i++){
        res += key[this->KEY_PARITY[i]-1];
    }

    return res;
} 
string DES::shiftLeft(string temp, int round)
{
    string res = "";
    for(int i=0; i<this->AMOUNT_SHIFT_PER_ROUND[round]; i++){
        for(int j=1; j<28; j++){
            res += temp[j];
        }
        // make the new res as new temp 
        res += temp[0];
        temp = res;
        res = "";
    }

    return temp;
} 
string DES::permuteKeyWithKeyComposition(const unsigned char *key)
{
    string res = "";
    for(int i=0; i<48; i++){
        res += key[this->INITIAL_PERMUTATION_TABLE[i]-1];
    }

    return res;
} 
/* ======== END ::: ENCRYPT KEY ======== */

/* ======== START ::: ENCRYPT & DECRYPT ======== */
string DES::permuteTextWithParity(const unsigned char *text)
{
    string res = "";
    for(int i=0; i<64; i++){
        res += text[this->TEXT_PARITY[i]-1];
    }

    return res;
} 
string DES::permuteWithExpTab(const unsigned char *text)
{
    string res = "";
    for(int i=0; i<48; i++){
        res += text[this->EXPANSION_TABLE[i]-1];
    }

    return res;
} 
string DES::xorOperation(string text1, string text2)
{
    string res = "";
    int size = text1.size();

    for(int i=0; i<size; i++){
        if(text1[i] != text2[i])
            res += "1";
        else
            res += "0";
    }

    return res;
}
string DES::permuteWithPermutationTab(const unsigned char *text)
{
    string res = "";
    for(int i=0; i<32; i++){
        res += text[this->PERMUTATION_TABLE[i]-1];
    }

    return res;
} 
string DES::permuteWithFinalPermutationTab(const unsigned char *text)
{
    string res = "";
    for(int i=0; i<64; i++){
        res += text[this->INVERSE_PERMUTATION_TABLE[i]-1];
    }

    return res;
} 
/* ======== END ::: ENCRYPT & DECRYPT ======== */

void DES::generateSubKeys(const unsigned char *key)
{
    // 1. make key from hex to binary 
    string binaryKey = hexToBinary(key);

    // 2. initial permutation 
    string ipRes = permuteKeyWithParity((unsigned char*)binaryKey.c_str()); 

    // 3. splitting to LPT and RPT 
    string lpt = ipRes.substr(0, 28);
    string rpt = ipRes.substr(28, 28);

    // 4. key transformation
    for(int i=0; i<16; i++){
        // shift left 
        lpt = shiftLeft(lpt, i);
        rpt = shiftLeft(rpt, i);

        string combine = lpt + rpt;

        // get round keys from compression permutation (from 56 bits to 48 bits)
        string compressedKey = permuteKeyWithKeyComposition((unsigned char*)combine.c_str());
        this->compressedKeysBin[i] = compressedKey;
        this->compressedKeysHex[i] = binaryToHex(compressedKey);
    }
}
void DES::encryptBlock(unsigned char *plainText, const unsigned char *key, bool isDecrypting)
{
    if(!isDecrypting)
        generateSubKeys(key);

    // 1. make text from hex to binary 
    string binaryText = hexToBinary(plainText);
    
    // 2. initial permutation for text 
    string permutatedText = permuteTextWithParity((unsigned char*)binaryText.c_str()); 
    
    // 3. split text 
    string lpt = permutatedText.substr(0,32);
    string rpt = permutatedText.substr(32,32);

    for(int i=0; i<16; i++){
        // 4. expand with D-Box (from 32 bits to 48 bits)
        string rptExpanded = permuteWithExpTab((unsigned char*)rpt.c_str());
        // cout << "rptExp = " << rptExpanded << "\n";

        // xor with compressed binary key from same round 
        string xored = xorOperation(this->compressedKeysBin[i], rptExpanded);
        // cout << "rkb = " << this->compressedKeysBin[i] << "\n";
        // cout << "xor = " << xored << "\n";
        
        
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
        // cout << "sbox = " << sBoxRes << "\n";

        string permutedSBox = permuteWithPermutationTab((unsigned char*)sBoxRes.c_str());
        // cout << "sbox perm = " << permutedSBox << "\n";

        xored = xorOperation(permutedSBox, lpt);
        // cout << "sbox perm xored = " << xored << "\n";

        lpt = xored;

        if(i != 15){
            lpt.swap(rpt);
        }
        // cout << "lpt = " << lpt << "\n";
        // cout << "rpt = " << rpt << "\n";
        // cout << "lpt = " << binaryToHex(lpt) << "\n";
        // cout << "rpt = " << binaryToHex(rpt) << "\n";
        // cout<< "\n";
    }
 
    string fullText = lpt + rpt;

    string cipherTextBin = permuteWithFinalPermutationTab((unsigned char*)fullText.c_str());
    string cipherText = binaryToHex(cipherTextBin);

    // cout << fullText << "\n";
    // cout << cipherTextBin << "\n";
    cout << cipherText << "\n";

    // for(int i=0; i<16; i++){
    //     cout << this->compressedKeysBin[i] << "\n";
    //     cout << this->compressedKeysHex[i] << "\n";
    // }
}
void DES::decryptBlock(unsigned char *cipherText, const unsigned char *key)
{
    int size = sizeof(this->compressedKeysBin) / sizeof(string);
    // for(int i=0; i<16; i++){
    //     cout << this->compressedKeysBin[i] << "\n";
    //     // cout << this->compressedKeysHex[i] << "\n";
    // }

    // reverse round keys 
    for(int i=0, j=size-1; i<size/2; i++, j--){
        string temp = this->compressedKeysBin[i];
        this->compressedKeysBin[i] = this->compressedKeysBin[j];
        this->compressedKeysBin[j] = temp;

        string temp2 = this->compressedKeysHex[i];
        this->compressedKeysHex[i] = this->compressedKeysHex[j];
        this->compressedKeysHex[j] = temp2;
    }

    // cout << "\nreversed" << "\n";
    // for(int i=0; i<16; i++){
    //     cout << this->compressedKeysBin[i] << "\n";
    //     // cout << this->compressedKeysHex[i] << "\n";
    // }

    encryptBlock(cipherText, key, true);
}