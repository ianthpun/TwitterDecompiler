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

#define Want_Debug

time_t ltime;


int main (int argc, char *argv[]) {

int corenumber = sysconf(_SC_NPROCESSORS_ONLN);
int listensocket;
int sockaddrlen = sizeof(struct sockaddr);
struct sockaddr_in serveraddress;
int readystatus = 1;
fd_set listenfd;
FD_ZERO(&listenfd);

struct timeval tv;
tv.tv_sec = 2;
tv.tv_usec = 0; 


//has to be exactly 2 arguments
if (argc != 3){
	printf( "You have put too many/few arguments.\n");
	exit(-1);}

// set up socket 
listensocket = socket(AF_INET, SOCK_STREAM, 0);
FD_SET(listensocket,&listenfd);

// set server address to the user choices
serveraddress.sin_addr.s_addr = inet_addr(argv[1]);
serveraddress.sin_port = htons(atoi(argv[2]));

connect(listensocket, &serveraddress, sizeof(struct sockaddr_in));
printf("connected to server \n");
while(1){
// write that im ready
write(listensocket,&readystatus, sizeof(readystatus));
// read incoming diretory message
if(select(FD_SETSIZE, &listenfd, NULL,NULL,&tv)>0){
	int dirsize = 0;
	read(listensocket, &dirsize,sizeof(int));
	// check to see if socket was closed = jobs are done.
	if(dirsize == 0){
		//printf("return of read is 0, leaving while loop. \n");
		break;
	}
	printf("new message incoming from server \n");
	char dirbuffer[dirsize+1];
	bzero(&dirbuffer, dirsize+1);
	printf("incoming message is %i long, reading message. \n",dirsize);
	read(listensocket, dirbuffer, dirsize);
	printf("recieved message is :%s \n", dirbuffer);
	FD_SET(listensocket, &listenfd);
}
else FD_SET(listensocket, &listenfd);
}

printf("no more messages, exiting \n");
close(listensocket);

exit(0);

}
