#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define NUM_CIPHERTEXTS 7
#define CIPHER_HEX_LEN 62
#define CIPHER_LENGTH 31

unsigned char cipherStreams[NUM_CIPHERTEXTS][CIPHER_LENGTH];
unsigned char recoveredKey[CIPHER_LENGTH];
int keyKnown[CIPHER_LENGTH] = {0}; 

void print_decrypted_messages() {
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
}

void inject_key_from_plaintext(int messageIndex, int offset, const char *plaintext) {
    int len = strlen(plaintext);
    for (int i = 0; i < len; i++) {
        int pos = offset + i;
        if (pos >= CIPHER_LENGTH) break;
        recoveredKey[pos] = cipherStreams[messageIndex][pos] ^ plaintext[i];
        keyKnown[pos] = 1;
    }
}

int main() {
    FILE *cipherText = fopen("ctext.txt", "r");
    if (cipherText == NULL) {
        printf("File not found\n");
        return 1;
    }

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

    for (int i = 0; i < NUM_CIPHERTEXTS; i++) {
        for (int k = 0; k < CIPHER_LENGTH; k++) {
            if (possibleSpaceCounts[i][k] > 5) {
                recoveredKey[k] = cipherStreams[i][k] ^ 0x20;
                keyKnown[k] = 1;
            }
        }
    }

    print_decrypted_messages();

    char input[100];
    while (1) {
        printf("\nInject known plaintext? (y/n): ");
        fgets(input, sizeof(input), stdin);
        if (input[0] != 'y' && input[0] != 'Y') break;

        printf("Enter message index (1–7): ");
        int msgIndex;
        fgets(input, sizeof(input), stdin);
        sscanf(input, "%d", &msgIndex);
        msgIndex--;

        printf("Enter offset (starting index in message): ");
        int offset;
        fgets(input, sizeof(input), stdin);
        sscanf(input, "%d", &offset);

        printf("Enter known plaintext (no spaces at the end): ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        inject_key_from_plaintext(msgIndex, offset, input);
        print_decrypted_messages();
    }

    return 0;
}
