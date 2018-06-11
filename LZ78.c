#include <stdio.h>
#include <stdlib.h>

#define MAX_ARR

int bit_buffer = 0, bit_mask = 128;
unsigned long codecount = 0, textcount = 0;

typedef struct Dictionary {
    unsigned int i;
    char *c;
    int size;
}dictionary;

FILE *infile, *outfile;

void error(void)
{
    printf("Output error\n");  exit(1);
}

void putbit1(void)
{
    bit_buffer |= bit_mask;
    if ((bit_mask >>= 1) == 0) {
        if (fputc(bit_buffer, outfile) == EOF) error();
        bit_buffer = 0;  bit_mask = 128;  codecount++;
    }
}

void putbit0(void)
{
    if ((bit_mask >>= 1) == 0) {
        if (fputc(bit_buffer, outfile) == EOF) error();
        bit_buffer = 0;  bit_mask = 128;  codecount++;
    }
}

void flush_bit_buffer(void)
{
    if (bit_mask != 128) {
        if (fputc(bit_buffer, outfile) == EOF) error();
        codecount++;
    }
}

int exist(dictionary *dic, char* c, int size) {
    int i, j;
    for(i = 0; i < size; i++) {
        int ex = -1;
        for(j = 0; j < dic[i].size; j++) {
            if(dic[i].c[j] != c[j]) 
                break;
            else if(dic[i].c[j] == c[j] && j == dic[i].size-1)
                ex = 1;
        }
        if(ex > 0) return i;
    }
    return -1;
}

void encode() {
    dictionary *dic;
    int count = 0; // number of elem in dictionary
    char* F;
    int length = 0;
    int curr_idx = 0;
    dic[0].i = 0;
    dic[0].size = 0;

    char ch = (char)fgetc(infile);
    while(ch != EOF) {
        F[length] = ch;
        int idx = exist(dic, F, count);
        if(idx >= 0) {
            length++;
            curr_idx = idx;
        } else {
            int i;
            fputc(dic[curr_idx].i, outfile);
            fputc(ch, outfile);
            dic[count].c = F;
            dic[count].i = dic[count-1].i + 1;
            dic[count].size = length;
            count++;
            curr_idx = count;
            dic[count].i = 0;
            dic[count].size = 0;
            F = "";
            length = 0;
        }
    }
}

int main(int argc, char* argv[]) {
    int enc;
    char *s;
                            
    if (argc != 3) {
        printf("Usage: lz78 infile outfile\n");
        return 1;
    }

    if ((infile  = fopen(argv[1], "rb")) == NULL) {
        printf("? %s\n", argv[1]);  return 1;
    }
    if ((outfile = fopen(argv[2], "wb")) == NULL) {
        printf("? %s\n", argv[2]);  return 1;
    }
    
    encode();
    fclose(infile);  fclose(outfile);
                                                                                return 0;
}

