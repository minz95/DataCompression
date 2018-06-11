/* SimpleHuffman.c - a simple program that uses Huffman trees to encode files
** see left404.com/2011/07/25/simplehuffman for more information.
**
** Copyright (C) 7/25/2011  Dara Hazeghi
**
** This software may be freely modified and redistributed.  It has no warranty.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_RANGE  257     /* max number of character values */
#define FAKE_EOF    256     /* special value to signify end-of-file */
#define CHAR_BITS   8

/* Huffman tree structure */
typedef struct htree
{
    struct htree *left;
    struct htree *right;
    int letter;
    int freq;
} htree;

/* compare two Huffman trees based on frequency, descending order */
int CmpTrees(const void *a, const void *b)
{
    const htree **x = (const htree **) a, **y = (const htree **) b;
    if((*x)->freq == (*y)->freq) return 0;
    else return ((*x)->freq < (*y)->freq) ? 1 : -1;
}

/* create a new string with given letter concatenated on to the prefix */
char *Concat(char *prefix, char letter)
{
    char *result = (char *)malloc(strlen(prefix) + 2);
    sprintf(result, "%s%c", prefix, letter);
    return result;
}

/* print specified error message and quite */
void Error(const char *msg)
{
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

/* build and return a Huffman tree based on a frequency table */
htree *BuildTree(int frequencies[])
{
    int i, len = 0;
    htree *queue[CHAR_RANGE];
    
    /* create trees for each character, add to the queue */
    for(i = 0; i < CHAR_RANGE; i++)
    {
        if(frequencies[i])
        {
            htree *toadd = (htree *)calloc(1, sizeof(htree));
            toadd->letter = i;
            toadd->freq = frequencies[i];

            queue[len++] = toadd;
        }
    }
    
    while(len > 1)
    {
        htree *toadd = (htree *)malloc(sizeof(htree));
        
        /* sort - smallest frequency trees are last */
        qsort(queue, len, sizeof(htree *), CmpTrees);
        
        /* dequeue two lowest frequency trees, build new tree from them */
        toadd->left = queue[--len];
        toadd->right = queue[--len];
        toadd->freq = toadd->left->freq + toadd->right->freq;
        
        queue[len++] = toadd; /* insert back in the queue */
    }
    
    return queue[0]; /* last tree in the queue is the full Huffman tree */
}

/* deallocate given Huffman tree */
void FreeTree(htree *tree)
{
    if(tree)
    {
        FreeTree(tree->left);
        FreeTree(tree->right);
        free(tree);
    }
}

/* traverse the Huffman tree to build up a table of encodings */
void TraverseTree(htree *tree, char **table, char *prefix)
{
    if(!tree->left && !tree->right) table[tree->letter] = prefix;
    else
    {
        if(tree->left) TraverseTree(tree->left, table, Concat(prefix, '0'));
        if(tree-> right) TraverseTree(tree->right, table, Concat(prefix, '1'));
        free(prefix);
    }
}

/* build a table of Huffman encodings given a set of frequencies */
char **BuildTable(int frequencies[])
{
    static char *table[CHAR_RANGE];
    char *prefix = (char *)calloc(1, sizeof(char));
    htree *tree = BuildTree(frequencies);
    TraverseTree(tree, table, prefix);
    FreeTree(tree);
    
    return table;
}

/* deallocate table of Huffman encodings */
void FreeTable(char *table[])
{
    int i;
    for(i = 0; i < CHAR_RANGE; i++) if(table[i]) free(table[i]);
}

/* output the Huffman header for an encoded file */
void WriteHeader(FILE *out, int frequencies[])
{
    int i, count = 0;
    
    for(i = 0; i < CHAR_RANGE; i++) if(frequencies[i]) count++;
    fprintf(out, "%d\n", count);
    
    for(i = 0; i < CHAR_RANGE; i++)
        if(frequencies[i]) fprintf(out, "%d %d\n", i, frequencies[i]);
}

/* read in the header of a Huffman encoded file */
int *ReadHeader(FILE *in)
{
    static int frequencies[CHAR_RANGE];
    int i, count, letter, freq;
    
    if(fscanf(in, "%d", &count) != 1) Error("invalid input file.");
    
    for(i = 0; i < count; i++)
    {
        if((fscanf(in, "%d %d", &letter, &freq) != 2)
           || letter < 0 || letter >= CHAR_RANGE) Error("invalid input file.");
        
        frequencies[letter] = freq;
    }
    fgetc(in); /* discard last newline */
    
    return frequencies;
}

/* write the given bit encoding to the output file */
void WriteBits(const char *encoding, FILE *out)
{
    /* buffer holding raw bits and number of bits filled */
    static int bits = 0, bitcount = 0;
    
    while(*encoding)
    {
        /* push bits on from the right */
        bits = bits * 2 + *encoding - '0';
        bitcount++;
        
        /* when we have filled the char, output as a single character */
        if(bitcount == CHAR_BITS)
        {
            fputc(bits, out);
            bits = 0;
            bitcount = 0;
        }
        
        encoding++;
    }
}

/* read a single bit from the input file */
int ReadBit(FILE *in)
{
    /* buffer holding raw bits and size of MSB filled */
    static int bits = 0, bitcount = 0;
    int nextbit;
    
    if(bitcount == 0)
    {
        bits = fgetc(in);
        bitcount = (1 << (CHAR_BITS - 1));
    }
    
    nextbit = bits / bitcount;
    bits %= bitcount;
    bitcount /= 2;
    
    return nextbit;
}

/* decode and return a single character from the input using the given Huffman
 * tree */
int DecodeChar(FILE *in, htree *tree)
{
    while(tree->left || tree->right)
    {
        if(ReadBit(in)) tree = tree->right;
        else tree = tree->left;
        
        if(!tree) Error("invalid input file.");
    }
    return tree->letter;
}

/* decode the Huffman-encoded file in and save the results to out */
void Decode(FILE *in, FILE *out)
{
    int *frequencies, c;
    htree *tree;
    
    frequencies = ReadHeader(in);
    tree = BuildTree(frequencies);
    
    while((c = DecodeChar(in, tree)) != FAKE_EOF)
        fputc(c, out);
    
    FreeTree(tree);
}

/* create a Huffman encoding for the file in and save the encoded version to
 * out */
void Encode(FILE *in, FILE *out)
{
    int c, frequencies[CHAR_RANGE] = { 0 };
    char **table;
    
    while((c = fgetc(in)) != EOF) frequencies[c]++;
    
    frequencies[FAKE_EOF] = 1;
    rewind(in);
    
    table = BuildTable(frequencies);
    //WriteHeader(out, frequencies);
    
    while((c = fgetc(in)) != EOF)
        WriteBits(table[c], out);
    
    /* use FAKE_EOF to indicate end of input */
    WriteBits(table[FAKE_EOF], out);
    
    /* write an extra 8 blank bits to flush the output buffer */
    WriteBits("0000000", out);
    
    FreeTable(table);
}

/* program to encode and decode files using Huffman trees */
int main(int argc, char *argv[])
{
    FILE *in, *out;
    
    if(argc != 4 || (strcmp(argv[1], "-c") && strcmp(argv[1], "-d")))
    {
        fprintf(stderr, "Usage: %s [-c,-d] infile outfile\n", argv[0]);
        exit(0);
    }
    
    if(!(in = fopen(argv[2], "rb")))
        Error("input file couldn't be opened.");
    else if((out = fopen(argv[3], "rb")))
        Error("output file already exists.");
    else if(!(out = fopen(argv[3], "wb")))
        Error("output file couldn't be opened.");
    
    if(!strcmp(argv[1], "-c")) Encode(in, out);
    else Decode(in, out);
    
    fclose(in);
    fclose(out);
    
    return 0;
}