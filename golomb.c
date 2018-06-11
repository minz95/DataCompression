#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

FILE *infile, *outfile;

int bit_buffer = 0, bit_mask = 128;
unsigned long codecount = 0, textcount = 0;

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

int RiceEncodeFile(const unsigned char k)
{
	//bit_file_t *bOutFile;               /* encoded output */
	unsigned char unary;                /* unary portion */
	unsigned char binary;               /* binary portion */
	unsigned char mask;                 /* mask for binary portion */
	int c;

	/* validate input and output files */
/*
	if ((NULL == inFile) || (NULL == outFile))
	{
		errno = ENOENT;
	    return -1;
    }

	bOutFile = MakeBitFile(outFile, BF_WRITE);

	if (NULL == bOutFile)
    {
        perror("Making Output File a BitFile");
		return -1;
	}
*/
	mask =  0xFF >> (CHAR_BIT - k);

	/* encode input file one byte at a time */
    while ((c = fgetc(infile)) != EOF)
	{
	    /* compute the unary portion */
		unary = (unsigned char)c;
		unary >>= k;

		while (unary > 0)
	    {
			/* write out unary worth of 1s */
            unary--;
            putbit1();
			//BitFilePutBit(1, bOutFile);
		}

		/* write an ending 0 */
        putbit0();
	    //BitFilePutBit(0, bOutFile);

		/* binary portion */
	    binary = (unsigned char)c & mask;
	    binary <<= (CHAR_BIT - k);      /* right justify bits */
        flush_bit_buffer();
        if (fputc(binary, outfile) == EOF) error();
		//BitFilePutBits(bOutFile, &binary, k);
	}

	/* pad fill with 1s so decode will run into EOF */
	//BitFileFlushOutput(bOutFile, 1);
    flush_bit_buffer();
	return 0;
}

int main(int argc, char* argv[])
{
    if(argc != 4) {
        printf("usage: golomb m infile outfile\n");
        return 1;
    }

    if((infile = fopen(argv[2], "rb")) == NULL) return -1;
    else if((outfile = fopen(argv[3], "wb")) == NULL) return -1;

    RiceEncodeFile(atoi(argv[1]));

    fclose(infile);
    fclose(outfile);

    return 0;
}
