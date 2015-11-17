/*Ian Pun
Student number : 301167944
itpun@sfu.ca
CMPT 300 D100
Instructor: Brian Booth
TA: Scott Kristjanson

Description:
Lyrebird is a program that will analyse and decrypt any encrypted text message that was sent from these "no-so-nice-people". It will use the methods that was discovered by the mathematician:

1) remove every 8th character 
2)change all groups of 6 characters to base 41 and then expand it positionally
3) Run Modular Exponentiation using exponent 1921821779 and mod 4294434817
4) Run the inverse of step 2 to get the decrypted text.


Due to the amount of twitter messages needed to be decrypted, Lyrebird can now run its decryption with an optimized number of processes with your selection of process management algorithm. 
It will do this by first taking in the algorithm in the first line of the config file, then taking in a list text with directories of all the decrypted messages and its designated output directory. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/wait.h>
#include "memwatch.h"
#include <unistd.h>
#include <sys/types.h>
#include "utilfunc.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

void SetServerAddress(struct sockaddr_in* serveraddress);
void PrintSockAddrPort(int sockfd);


#define Want_Debug

time_t ltime;


int main (int argc, char *argv[]) {

int sockfd;
struct sockaddr_in serveraddress;



//has to be exactly 2 arguments
if (argc != 3){
	printf( "You have put too many/few arguments.\n");
	exit(-1);}

// start file pointers and open file
/*
FILE *input;
FILE *output;
input = fopen(argv[1], "r");
output = fopen(argv[1], "w");
*/



SetServerAddress(&serveraddress);

sockfd = socket(AF_INET, SOCK_STREAM, 0);

bind(sockfd, (struct sockaddr *) &serveraddress, sizeof(struct sockaddr_in));

listen(sockfd, 5);

PrintSockAddrPort(sockfd);


exit(0);

}



// SetServerAddress gets the connect server address and puts it into sockaddr_in serveraddress for binding.
// Refered to the answer from stackflow here : http://stackoverflow.com/questions/4139405/how-to-know-ip-address-for-interfaces-in-c
void SetServerAddress(struct sockaddr_in* serveraddress){
struct sockaddr_in *sa;
struct ifaddrs *ifap, *ifa;
char *addr;

// get current IP address
getifaddrs (&ifap);
// go through the ifap linklist and find the correct address.
for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
// address should be IPv4 with interface en0.
    if ((!strcmp(ifa->ifa_name, "en0")) && (ifa->ifa_addr->sa_family==AF_INET)){
        sa = (struct sockaddr_in*) ifa->ifa_addr;
        addr = inet_ntoa(sa->sin_addr);
		serveraddress->sin_addr.s_addr = inet_addr(addr);
    }
}
freeifaddrs(ifap);

}

// I used a bit of this source here to set up my getsockname call
//http://stackoverflow.com/questions/5759031/getsockname-always-returning-0-0-0-0

void PrintSockAddrPort(int sockfd){

struct sockaddr_in sockbuffer;
int len = sizeof(struct sockaddr);

if (getsockname(sockfd, (struct sockaddr *)&sockbuffer, &len) == -1)
    perror("getsockname");
else
    printf("[%s] lyrebird.server: PID %i on host %s, port %i\n", CurrTime(&ltime), getpid(), inet_ntoa(sockbuffer.sin_addr), ntohs(sockbuffer.sin_port));

}






