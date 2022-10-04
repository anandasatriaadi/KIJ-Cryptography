#include <string>
#include <cstring>
#include <iostream>
#include <bitset>

using namespace std;

void shiftRow(unsigned char input[4][4], int rowNumber) {
    unsigned char tempRow[4];

    for (int i = 0; i < 4; i++) {
        tempRow[i] = input[rowNumber][(i + rowNumber) % 4];
    }
    for (int i = 0; i < 4; i++) {
        input[rowNumber][i] = tempRow[i];
    }
}

void shiftRows(unsigned char input[4][4]) {
    for (int i = 1; i < 4; i++) {
        shiftRow(input, i);
    }
}

void mixColumns(unsigned char input[4][4]) {
    unsigned int mul_constant[4][4] = {
        {2, 3, 1, 1},
        {1, 2, 3, 1},
        {1, 1, 2, 3},
        {3, 1, 1, 2}
    };

    unsigned char temp[4][4];
    memset(temp, 0, sizeof(temp));

    // First loops through the initial state
    for (int i = 0; i < 4; i++) {
        // printf("COL %d\n", i);
        // Then loops through the multiplication constant
        for (int j = 0; j < 4; j++) {
            unsigned char temp2 = 0;
            // cout << "|";
            for (int k = 0; k < 4; k++) {
                if(mul_constant[j][k] == 1) {
                    temp2 ^= input[k][i];
                } else {
                    // checks whether the leftmost bit is 1
                    if(input[k][i] >= 0x80) {
                        temp2 ^= (input[k][i] << 1) ^ 0x1b;
                    } else {
                        temp2 ^= input[k][i] << 1;
                    }

                    if(mul_constant[j][k] == 3) {
                        temp2 ^= input[k][i];
                    }
                }

                // printf("%2X.{%d}", input[k][i], mul_constant[j][k]);
                // if(k < 3) {
                //     printf(" + ");
                // }
            }
            temp[j][i] = temp2;
            // printf(" -> %2X", temp2);
            // cout << "| " << endl;
        }
        // cout << endl << endl;
    }
    
    memcpy(input, temp, sizeof(temp));
    
}

// AES main function for testing
int main() {
    unsigned char input[4][4] = {
        {0xea, 0x04, 0x65, 0x85},
        {0x83, 0x45, 0x5d, 0x96},
        {0x5c, 0x33, 0x98, 0xb0},
        {0xf0, 0x2d, 0xad, 0xc5}
    };
    mixColumns(input);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%2X ", input[i][j]);
        }
        cout << endl;
    }
    cout << endl;
    
    shiftRows(input);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%2X ", input[i][j]);
        }
        cout << endl;
    }
    return 0;
}