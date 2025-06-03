#include <stdio.h>
#include <stdlib.h>

#define MAX_CIPHER_LEN 10000

int score_stream(unsigned char *stream, int length, unsigned char key_byte)
{
    int score = 0;
    for (int i = 0; i < length; i++)
    {
        unsigned char decrypted = stream[i] ^ key_byte;
        if (decrypted >= 32 && decrypted <= 126)
        {
            score++;
        }
    }
    return score;
}

int main()
{
    FILE *cipherText;
    cipherText = fopen("ctext.txt", "r");
    if (cipherText == NULL)
    {
        printf("File not found\n");
        return 1;
    }

    unsigned char cipherStream[MAX_CIPHER_LEN];
    int cipher_len = 0;
    unsigned char byte;
    while (fscanf(cipherText, "%2hhX", &byte) == 1)
    {
        cipherStream[cipher_len++] = byte;
        if (cipher_len >= MAX_CIPHER_LEN)
        {
            printf("cipher text is too long, adjust max cipher length\n");
            return 1;
        }
    }
    fclose(cipherText);

    for (int guessed_key_length = 1; guessed_key_length < 100; guessed_key_length++)
    {
        unsigned char streams[guessed_key_length][MAX_CIPHER_LEN];
        int streams_lens[guessed_key_length];

        for (int i = 0; i < guessed_key_length; i++)
        {
            streams_lens[i] = 0;
        }

        for (int i = 0; i < cipher_len; i++)
        {
            int stream_idx = i % guessed_key_length;
            int pos = streams_lens[stream_idx];
            streams[stream_idx][pos] = cipherStream[i];
            streams_lens[stream_idx]++;
        }

        unsigned char key[guessed_key_length];

        for (int i = 0; i < guessed_key_length; i++)
        {
            int best_score = -1;
            unsigned char best_key_byte = 0;

            for (int j = 0; j <= 255; j++)
            {
                int score = score_stream(streams[i], streams_lens[i], (unsigned char)j);
                if (score > best_score)
                {
                    best_score = score;
                    best_key_byte = (unsigned char)j;
                }
            }
            key[i] = best_key_byte;
        }

        printf("KEY LENGTH GUESS: %d\n", guessed_key_length);
        for (int i = 0; i < cipher_len; i++)
        {
            unsigned char plaintext = cipherStream[i] ^ key[i % guessed_key_length];
            printf("%c", (plaintext >= 32 && plaintext <= 126) ? plaintext : '.');
        }
        printf("\n");
    }
}
