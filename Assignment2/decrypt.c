#include "decrypt.h"
#include <stdio.h>

#define ull unsigned long long int
// uncomment Want_Debug to see debug notes
//#define Want_Debug 
ull mod_exps(ull base);
char base41convert (ull num, char table[]);

void decrypt(char* str, char table[], FILE * output) {

// remove the \n chracter from the string     
strtok(str, "\n");

#if Want_Debug
printf("the incoming string is: %s\n", str);
printf("string length is %d \n", strlen(str));
#endif

int strlength = strlen(str);

// remove every 8th positioned character, start for loop
if (strlength > 7){
    int count = 0;
    for (int i=7; i <strlength; i=i+8){
        #if Want_Debug
        printf("removing character %c \n",str[i]);
        #endif
        // memmove should remove every 8th position character
        memmove(&str[i], &str[i+1], strlen(str)-i);
        i--;
        strlength--;
    }
}
// convert every character to base 41 using table[]. The index found with strchr
// equals to the converted base 41 number.
for (int i = 0; i< strlength; i++){
	char * converted = strchr(table, str[i]);
	if (converted != NULL){    
    		int index = (int)(converted-table);
    		str[i] = index;
		} 
}

// using 141 to as an arbitrary size
ull groups[141];
int gcount = 0;
// Group them to 6s and run exponental expansion and put into groups array
for(int i= 0; i< strlength; i=i+6){
    groups[gcount] = (str[i]*pow(41,5)+str[i+1]*pow(41,4)+str[i+2]*pow(41,3)+str[i+3]*pow(41,2)+str[i+4]*pow(41,1)+str[i+5]*pow(41,0));
    gcount++;
} 
// Run modular_power function
for (int i = 0; i < gcount; i++){
    groups[i] = mod_exps(groups[i]);

}

// Last step, decode groups of 6 using base41 conversion in inverse
for (int i = 0; i < gcount; i++){
    // use temp string to hold the decoded str of 6 char length
    char decodedstr[6];
    //output to string backwards because of how mod_exps output the decoded string from end char to first
    for (int j = 5; j >= 0; j--){
	decodedstr[j] = base41convert(groups[i], table);
	groups[i] = groups[i]/41;
    }
    fprintf(output, decodedstr);
}

} 



// This function was derived from the referenced formula given in 
// http://www.sanfoundry.com/cpp-program-implement-modular-exponentiation-algorithm/

ull mod_exps(ull base){

long long int exponent = 1921821779;
long long int  mod = 4294434817;

ull result = 1;

while (exponent > 0){
    if (exponent %2 == 1)
	result = (result*base) % mod;   
exponent = exponent >>1;
base = (base*base) % mod;
}

return result;

}


// I learned how to convert from base 10 to any base through this website:
// http://mathbits.com/MathBits/CompSci/Introduction/frombase10.htm
// Implimination was done by myself.

char base41convert (ull num, char table[]){
// this should return the character converted to base 41
int remain = num % 41;
char result = table[remain];
#if Want_Debug
printf("num is %u, remainder is %d, result is %c \n", num, remain, result);
#endif
return result;
} 

