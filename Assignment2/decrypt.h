#include <string.h>
#include <stdio.h>
#ifndef _DECRYPT_H_
#define _DECRYPT_H_
#define ull unsigned long long int

ull mod_exps(ull base);
char base41convert (ull num, char table[]);
void decrypt(char* str, char table[],FILE * output);

#endif 

