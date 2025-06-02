#include <stdio.h>
#include <stdlib.h>

#define MAX_CIPHER_LEN 10000

int main () {
    FILE *cipherText;
    cipherText = fopen("ctext.txt", "r");
    if (cipherText == NULL) {
        printf("File not found\n");
        return 1;
    }

    unsigned char cipherStream[MAX_CIPHER_LEN];
    int cipher_len = 0;
    unsigned char byte;
    while (fscanf(cipherText, '%2hhX', &byte) == 1) {
        cipherStream[cipher_len++] = byte;
        if (cipherText >= MAX_CIPHER_LEN) {
            printf("cipher text is too long, adjust max cipher length\n");
            return 1;
        }
    }
    fclose(cipherText);
}