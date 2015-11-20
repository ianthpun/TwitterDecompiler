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

int listensocket,connectsocket;
int sockaddrlen = sizeof(struct sockaddr);
struct sockaddr_in serveraddress;
int processstatus = 2;
fd_set listenfd;
FD_ZERO(&listenfd);


struct timeval tv;
tv.tv_sec = 2;
tv.tv_usec = 0; 


//has to be exactly 2 arguments
if (argc != 3){
	printf( "You have put too many/few arguments.\n");
	exit(-1);}


listensocket = socket(AF_INET, SOCK_STREAM, 0);
FD_SET(listensocket,&listenfd);

// set server address to the user choices
serveraddress.sin_addr.s_addr = inet_addr(argv[1]);
serveraddress.sin_port = htons(atoi(argv[2]));


connectsocket = connect(listensocket, &serveraddress, sizeof(struct sockaddr_in));
printf("connected to client \n");
// my write is blocked
write(listensocket,&processstatus, sizeof(processstatus));
printf("wrote some stuff to socket\n");

while(1);

//while(1);
/*
while(1){

if(select(FD_SETSIZE, &listenfd, NULL,NULL,&tv)>0){
printf("Recieving msg \n");
char recvBuff[2050];
if(recv(listensocket, recvBuff, sizeof(recvBuff)-1, 0)<=0){
break;}

printf("recieved msg from server: %s\n", recvBuff);
write(listensocket,processstatus, strlen(processstatus));
}

else FD_SET(listensocket,&listenfd);

}




/*
1) we will read from the socket the message incoming which directories.
2) parse them, and throw them into pipe encryption, which always runs FCFS
3) Each time a decryption finishes, pipe back up to parent its done, then parent will 
send that ready status to server through a socket
4) server will have all the sockets that are connected in a fd_set, which is run on a select.
SELECT will find which one is available and send over.
*/








exit(0);

}
