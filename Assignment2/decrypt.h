/*Ian Pun
Student number : 301167944
itpun@sfu.ca
CMPT 300 D100
Instructor: Brian Booth
TA: Scott Kristjanson
*/

#include <string.h>
#include <stdio.h>
#include <time.h>
#include "memwatch.h"

#define ull unsigned long long int

ull mod_exps(ull base);
char base41convert (ull num, char table[]);
void decrypt(char* inputdir, char* outputdir);
char * CurrTime(time_t ltime);

// This dynamic array ProcessArr needs to be referenced in both main.c and decrypt.c to free it.
int * ProcessArr;
