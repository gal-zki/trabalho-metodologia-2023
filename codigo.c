#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <openssl/aes.h>
#include "ascon.h"

// O tamanho da chave é 16 bytes (128 bits)
// tanto para o AES quanto para o Ascon
#define KEY_LENGTH 16

char* gerar(int tamanho)
{
    int i;   unsigned tmp;
    char* string = (char*)malloc(sizeof(char)*(tamanho+1));
    string[tamanho+1] = '\0';
    for (i = 0; i < tamanho; i++)
    {
        while (1)
        {
            tmp = random()%(126-32)+32;
            if (tmp != 34 && tmp != 92 && tmp != 37) break;
        }
        string[i] = tmp;
    }
    return string;
}

void preencher(char* palavra, int tamanho)
{
    int i;  unsigned tmp;
    palavra[tamanho] = '\0';
    for (i = 0; i < tamanho; i++)
    {
        while (1)
        {
            tmp = random()%(126-32)+32;
            if (tmp != 34 && tmp != 92 && tmp != 37) break;
        }
        palavra[i] = tmp;
    }
}

void encryptAES_ECB(const unsigned char *plaintext, int plaintext_len, const unsigned char *key, unsigned char *ciphertext) {
    AES_KEY aesKey;
    AES_set_encrypt_key(key, 128, &aesKey);
    
    for (int i = 0; i < plaintext_len; i += AES_BLOCK_SIZE) {
        AES_encrypt(plaintext + i, ciphertext + i, &aesKey);
    }
}

void decryptAES_ECB(const unsigned char *ciphertext, int ciphertext_len, const unsigned char *key, unsigned char *deciphertext) {
    AES_KEY aesKey;
    AES_set_decrypt_key(key, 128, &aesKey);

    for (int i = 0; i < ciphertext_len; i += AES_BLOCK_SIZE) {
        AES_decrypt(ciphertext + i, deciphertext + i, &aesKey);
    }
    // deciphertext[ciphertext_len] = '\0';
}

void encrypt_Ascon();

int main (int argc, char* argv[])
{
    srandom(time(NULL));
    int tamanho = atoi(argv[1]);
    int amostras = atoi(argv[2]);
    int i;
    // char* texto_original = gerar(tamanho);
    char* texto_original = malloc(sizeof(char)*(tamanho+1));

    int tamanho_texto_original = tamanho;
    int tamanho_texto_cifrado_AES = ((tamanho_texto_original - 1) / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
    int tamanho_texto_cifrado_Ascon = tamanho;

    unsigned char* texto_cifrado_AES = (char*)malloc(sizeof(char)*(tamanho_texto_cifrado_AES));
    unsigned char* texto_decifrado_AES = (char*)malloc(sizeof(char)*(tamanho_texto_original+1));
    unsigned char* texto_cifrado_Ascon = (char*)malloc(sizeof(char)*(tamanho_texto_cifrado_Ascon));
    unsigned char* texto_decifrado_Ascon = (char*)malloc(sizeof(char)*(tamanho_texto_original+1));

    clock_t inicio, fim;
    double tempo_de_cifragem_AES, tempo_de_decifragem_AES;
    double tempo_de_cifragem_Ascon, tempo_de_decifragem_Ascon;

    double tempos_cifragem_AES[amostras], tempos_decifragem_AES[amostras];
    double tempos_cifragem_Ascon[amostras], tempos_decifragem_Ascon[amostras];

    // Chave
    unsigned char key[16] = 
    {
        '0', '1', '2', '3', '4', '5', '6', '7', 
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
    };

    unsigned char nonce[ASCON_AEAD_NONCE_LEN] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7', 
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
    };

    char tag[ASCON_AEAD_TAG_MIN_SECURE_LEN];

    for (i = 0; i < amostras; i++)
    {
        printf("Amostra %d\n", i+1);
        
        // memset(texto_original, 0, tamanho + 1);
        // memset(texto_cifrado_AES, 0, strlen(texto_cifrado_AES));
        // memset(texto_decifrado_AES, 0, strlen(texto_decifrado_AES));
        // memset(texto_cifrado_Ascon, 0, strlen(texto_cifrado_Ascon));
        // memset(texto_decifrado_Ascon, 0, strlen(texto_decifrado_Ascon));

        memset(texto_original, 0, tamanho + 1);
        memset(texto_cifrado_AES, 0, tamanho_texto_cifrado_AES);
        memset(texto_decifrado_AES, 0, tamanho + 1);
        memset(texto_cifrado_Ascon, 0, tamanho_texto_cifrado_Ascon);
        memset(texto_decifrado_Ascon, 0, tamanho + 1);

        preencher(texto_original, tamanho);

        // Cifragem usando AES
        inicio = clock();
        encryptAES_ECB(texto_original, tamanho_texto_original, key, texto_cifrado_AES);
        fim = clock();
        tempo_de_cifragem_AES = (double)(fim - inicio) / CLOCKS_PER_SEC;

        // Decifragem usando AES
        inicio = clock();
        decryptAES_ECB(texto_cifrado_AES, tamanho_texto_cifrado_AES, key, texto_decifrado_AES);
        fim = clock();
        tempo_de_decifragem_AES = (double)(fim - inicio) / CLOCKS_PER_SEC;

        // Cifragem usando Ascon
        inicio = clock();
        ascon_aead128_encrypt
        (
            texto_cifrado_Ascon, tag, 
            key, nonce, NULL, texto_original, 
            0, tamanho_texto_original, 
            ASCON_AEAD_TAG_MIN_SECURE_LEN
        );
        fim = clock();
        tempo_de_cifragem_Ascon = (double)(fim - inicio) / CLOCKS_PER_SEC;

        // Decifragem usando Ascon
        inicio = clock();
        ascon_aead128_decrypt
        (
            texto_decifrado_Ascon, key, 
            nonce, NULL, texto_cifrado_Ascon, 
            tag, 0, tamanho_texto_cifrado_Ascon,
            ASCON_AEAD_TAG_MIN_SECURE_LEN
        );
        fim = clock();
        tempo_de_decifragem_Ascon = (double)(fim - inicio) / CLOCKS_PER_SEC;

        if (tamanho <= 50) 
        {
            int i;
            printf("Texto original:\n%s\n", texto_original);
            printf("Texto cifrado com AES:\n%s\n", texto_cifrado_AES);
            for (i = 0; i < tamanho_texto_cifrado_AES; i++)
                printf("%x ", texto_cifrado_AES[i]); printf("\n");
            printf("Texto decifrado com AES:\n%s\n", texto_decifrado_AES);
            printf("Texto cifrado com Ascon:\n%s\n", texto_cifrado_Ascon);
            for (i = 0; i < tamanho_texto_cifrado_Ascon; i++)
                printf("%x ", texto_cifrado_Ascon[i]); printf("\n");
            printf("Texto decifrado com Ascon:\n%s\n", texto_decifrado_Ascon);
        }

        tempos_cifragem_AES[i] = tempo_de_cifragem_AES;
        tempos_decifragem_AES[i] = tempo_de_decifragem_AES;
        tempos_cifragem_Ascon[i] = tempo_de_cifragem_Ascon;
        tempos_decifragem_Ascon[i] = tempo_de_decifragem_Ascon;

        printf("Tempo de cifragem com AES: %f segundos\n", tempo_de_cifragem_AES);
        printf("Tempo de decifragem com AES: %f segundos\n", tempo_de_decifragem_AES);
        printf("Tempo de cifragem com Ascon: %f segundos\n", tempo_de_cifragem_Ascon);
        printf("Tempo de decifragem com Ascon: %f segundos\n", tempo_de_decifragem_Ascon);
        printf("---------------------------------------------------------\n");
    }

    for (i = 0; i < amostras; i++)
    {
        printf("%f ", tempos_cifragem_AES[i]);
    }
    printf("\n");

    for (i = 0; i < amostras; i++)
    {
        printf("%f ", tempos_decifragem_AES[i]);
    }
    printf("\n");

    for (i = 0; i < amostras; i++)
    {
        printf("%f ", tempos_cifragem_Ascon[i]);
    }
    printf("\n");

    for (i = 0; i < amostras; i++)
    {
        printf("%f ", tempos_decifragem_Ascon[i]);
    }
    printf("\n");            
    printf("---------------------------------------------------------\n");


    double media_cifragem_AES = 0, media_decifragem_AES = 0;
    double media_cifragem_Ascon = 0, media_decifragem_Ascon = 0;

    for (i = 0; i < amostras; i++)
    {
        media_cifragem_AES += tempos_cifragem_AES[i];
        media_decifragem_AES += tempos_decifragem_AES[i];
        media_cifragem_Ascon += tempos_cifragem_Ascon[i];
        media_decifragem_Ascon += tempos_decifragem_Ascon[i];
    }

    media_cifragem_AES /= amostras;
    media_decifragem_AES /= amostras;
    media_cifragem_Ascon /= amostras;
    media_decifragem_Ascon /= amostras;

    printf("Media de tempo de cifragem com AES: %lf\n", media_cifragem_AES);
    printf("Media de tempo de decifragem com AES: %lf\n", media_decifragem_AES);
    printf("Media de tempo de cifragem com Ascon: %lf\n", media_cifragem_Ascon);
    printf("Media de tempo de decifragem com Ascon: %lf\n", media_decifragem_Ascon);

    double desvio_padrao_cifragem_AES = 0, desvio_padrao_decifragem_AES = 0;
    double desvio_padrao_cifragem_Ascon = 0, desvio_padrao_decifragem_Ascon = 0;

    for (i = 0; i < amostras; i++)
    {
        desvio_padrao_cifragem_AES += (tempos_cifragem_AES[i]-media_cifragem_AES)*(tempos_cifragem_AES[i]-media_cifragem_AES);
        desvio_padrao_decifragem_AES += (tempos_decifragem_AES[i]-media_decifragem_AES)*(tempos_decifragem_AES[i]-media_decifragem_AES);
        desvio_padrao_cifragem_Ascon += (tempos_cifragem_Ascon[i]-media_cifragem_Ascon)*(tempos_cifragem_Ascon[i]-media_cifragem_Ascon);
        desvio_padrao_decifragem_Ascon += (tempos_decifragem_Ascon[i]-media_decifragem_Ascon)*(tempos_decifragem_Ascon[i]-media_decifragem_Ascon);
    }

    desvio_padrao_cifragem_AES /= amostras;         desvio_padrao_cifragem_AES = sqrt(desvio_padrao_cifragem_AES);
    desvio_padrao_decifragem_AES /= amostras;       desvio_padrao_decifragem_AES = sqrt(desvio_padrao_decifragem_AES);
    desvio_padrao_cifragem_Ascon /= amostras;       desvio_padrao_cifragem_Ascon = sqrt(desvio_padrao_cifragem_Ascon);
    desvio_padrao_decifragem_Ascon /= amostras;     desvio_padrao_decifragem_Ascon = sqrt(desvio_padrao_decifragem_Ascon);   

    printf("Desvio padrao do tempo de cifragem com AES: %.12lf\n", desvio_padrao_cifragem_AES);
    printf("Desvio padrao do tempo de decifragem com AES: %.12lf\n", desvio_padrao_decifragem_AES);
    printf("Desvio padrao do tempo de cifragem com Ascon: %.12lf\n", desvio_padrao_cifragem_Ascon);
    printf("Desvio padrao do tempo de decifragem com Ascon: %.12lf\n", desvio_padrao_decifragem_Ascon);

    free(texto_original);
    free(texto_cifrado_AES);
    free(texto_decifrado_AES);
    free(texto_cifrado_Ascon);
    free(texto_decifrado_Ascon);

    return 0;
}