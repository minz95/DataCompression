#include <stdio.h>
#include <stdlib.h>

#define MAX_ARR 500000

int bit_buffer = 0, bit_mask = 128;
unsigned long codecount = 0, textcount = 0;

typedef struct Dictionary {
    unsigned int i;
    char *c;
    int size;
}dictionary;

FILE *infile, *outfile;
dictionary dic[MAX_ARR];
char F[MAX_ARR];

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

int exist(int size, int length) {
    int i, j;
    for(i = 0; i < size; i++) {
        int ex = -1;
		if (dic[i].size != length) continue;
        for(j = 0; j < dic[i].size; j++) {
            if(dic[i].c[j] != F[j]) 
                break;
            else if(dic[i].c[j] == F[j] && j == length-1)
                ex = 1;
        }
        if(ex > 0) return i;
    }
    return -1;
}

void encode() {
	for (int i = 0; i < MAX_ARR; i++) {
		dic[i].i = 0;
		dic[i].size = 0;
		dic[i].c = "";
		F[i] = "";
	}
    int count = 0; // number of elem in dictionary
    //char* F = "";
    int length = 0;
    int curr_idx = 0;
    //dic[0].i = 0;
    //dic[0].size = 0;

    char ch = (char)fgetc(infile);
    while(ch != EOF) {
		F[length] = ch;
        int idx = exist(count, length+1);
        if(idx >= 0) {
            length++;
            curr_idx = idx;
        } else {
            int i;
            fputc(dic[curr_idx].i, outfile);
            fputc(ch, outfile);
            dic[count].c = F;
			if (count > 0)
				dic[count].i = dic[count - 1].i + 1;
			else
				dic[count].i = 1;
			length++;
            dic[count].size = length;
			curr_idx = count;
            count++;
            for (int i = 0; i < length; i++) {
				F[i] = "";
			}
            length = 0;
        }
		ch = (char)fgetc(infile);
    }
}

int main(int argc, char* argv[]) {
    /*int enc;
    char *s;*/
                            
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

