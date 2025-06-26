#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define NUM_CIPHERTEXTS 7
#define CIPHER_HEX_LEN 62
#define CIPHER_LENGTH 31

int main() {
    FILE *cipherText = fopen("ctext.txt", "r");
    if (cipherText == NULL) {
        printf("File not found");
        return 1;
    }
    

    unsigned char cipherStreams[NUM_CIPHERTEXTS][CIPHER_LENGTH];
    char hexLine[CIPHER_HEX_LEN + 2];
    int line = 0;

    while (fgets(hexLine, sizeof(hexLine), cipherText) != NULL && line < NUM_CIPHERTEXTS) {
        for (int i = 0; i < CIPHER_LENGTH; i++) {
            unsigned int byte;
            sscanf(&hexLine[i * 2], "%2x", &byte);
            cipherStreams[line][i] = (unsigned char)byte;
        }
        line++;
    }

    fclose(cipherText);

    int possibleSpaceCounts[NUM_CIPHERTEXTS][CIPHER_LENGTH] = {0};

    for (int i = 0; i < NUM_CIPHERTEXTS; i++) {
        for (int j = 0; j < NUM_CIPHERTEXTS; j++) {
            if (i == j) continue;
            for (int k = 0; k < CIPHER_LENGTH; k++) {
                unsigned char xorByte = cipherStreams[i][k] ^ cipherStreams[j][k];
                if (isalpha(xorByte)) {
                    possibleSpaceCounts[i][k]++;
                }
            }
        }
    }

    unsigned char recoveredKey[CIPHER_LENGTH];
    int keyKnown[CIPHER_LENGTH] = {0};

    for (int i = 0; i < NUM_CIPHERTEXTS; i++) {
        for (int k = 0; k < CIPHER_LENGTH; k++) {
            if (possibleSpaceCounts[i][k] > 5) {
                recoveredKey[k] = cipherStreams[i][k] ^ 0x20;
                keyKnown[k] = 1;
            }
        }
    }

    printf("\n--- Decrypted Plaintexts (Partial) ---\n");
    for (int i = 0; i < NUM_CIPHERTEXTS; i++) {
        printf("Message %d: ", i + 1);
        for (int k = 0; k < CIPHER_LENGTH; k++) {
            if (keyKnown[k]) {
                unsigned char plainChar = cipherStreams[i][k] ^ recoveredKey[k];
                if (isprint(plainChar))
                    printf("%c", plainChar);
                else
                    printf("*");
            } else {
                printf("_");
            }
        }
        printf("\n");
    }

    return 0;
}