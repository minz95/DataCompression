#include <stdio.h>

#define MAX_INPUT 1000

encode(char symbo1, float* cum-freq) {
	float range = high - low;
	float high = low + range * cum-freq[symbol-1]
	float low = low + range*cum-freq[symbol]
}

decode(float* cum-freq) {
	find symbol such that 
	cum-freq[symbol] <= (value-low)/(high-low) < cum-freq[symbol-1]
	/* This ensures that value lies within the new  */
	/* (low, high) range that will be calculated by */
	/* the following lines of code. */
				 
	range = high - low
	high = low + range*cum-freq[symbol-1]
	low = low + range*cum-freq[symbol]
	return symbol
}

int main() {
    FILE *fp;
    // initialize arrays for characters and cumulations
    char arr[MAX_INPUT];
    int cum[MAX_INPUT];
    int i;
    for(i = 0; i < MAX_INPUT; i++) {
        arr[i] = NULL;
        cum[i] = 0;
    }
    int count = 0;
    return -1;
}
