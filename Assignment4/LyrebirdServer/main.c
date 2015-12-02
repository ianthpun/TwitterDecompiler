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
#include <errno.h>

void SetServerAddress(struct sockaddr_in* serveraddress);
void PrintSockAddrPort(int sockfd);
void NewClientConnect(fd_set *listensockfd, fd_set *connectedclients, int listensocket, int connectsocket[], char* clientIP[][50], int* clientnum, FILE* output);
int NewClientMessageCheck(fd_set *connectedclients, int clientnum, int connectsocket[],char* clientIP[][50], char* str, FILE* output);
void remainingwork(int connectsocket[], int clientnum, char* clientIP[][50], FILE* output);

#define Want_Debug

time_t ltime;
int sockaddrlen = sizeof(struct sockaddr);
struct sockaddr_in serveraddress, clientaddress;
const int jobsfinished = 3;



int main (int argc, char *argv[]) {

int listensocket;
int connectsocket[500]; // will make this a malloc later
int clientnum = 0;
char str[2050];
char clientstatus[5];
fd_set listensockfd;
fd_set connectedclients;
FD_ZERO(&listensockfd);
FD_ZERO(&connectedclients);
char *clientIP[500][50]; // will make malloc later
int msgsize;

// timeval is used for the SELECT function
struct timeval tv;
tv.tv_sec = 1;
tv.tv_usec = 0; 



//has to be exactly 2 arguments
if (argc != 3){
	printf( "You have put too many/few arguments.\n");
	exit(-1);}


// start file pointers and open file

FILE *input;
FILE *output;
input = fopen(argv[1], "r");
output = fopen(argv[2], "w");

//if input returns 0 the input file is missing
if (input == 0){
	printf("Input file is missing.\n");
	exit(-2);
}

SetServerAddress(&serveraddress);

listensocket = socket(AF_INET, SOCK_STREAM, 0);
FD_SET(listensocket,&listensockfd);

if(bind(listensocket, (struct sockaddr *) &serveraddress, sizeof(struct sockaddr_in))<0)
	perror("bind");

if(listen(listensocket, 5)<0)
	perror("listen");


PrintSockAddrPort(listensocket);

while(fgets(str, 2050, input)){
	while(1){
	NewClientConnect(&listensockfd, &connectedclients, listensocket, connectsocket, clientIP, &clientnum, output);
	if (NewClientMessageCheck(&connectedclients, clientnum, connectsocket,clientIP, str, output)==1)
		break;}	
}
// close the listening socket, you will not need anymore clients to be connected
close(listensocket);
// end of file. Send a message to all the clients requesting to finish their jobs
puts("writing closing file to everyone");

for (int i =0; i< clientnum; i++){
	if(write(connectsocket[i], &jobsfinished, sizeof(int))<0)
		perror("write");
}


remainingwork(connectsocket,clientnum, clientIP,output);





printf("[%s] lyrebird server: PID %i completed its tasks and is exiting successfully.\n", CurrTime(&ltime), getpid());

close(input);
close(output);

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
        sa = (struct sockaddr_in*) ifa->ifa_addr;
        addr = inet_ntoa(sa->sin_addr);
// address should be IPv4 
    if ((strcmp(addr, "127.0.0.1")==0) && (ifa->ifa_addr->sa_family==AF_INET)){
    	ifa = ifa->ifa_next;
    	sa = (struct sockaddr_in*) ifa->ifa_addr;
        addr = inet_ntoa(sa->sin_addr);
		serveraddress->sin_addr.s_addr = inet_addr(addr);
		serveraddress->sin_family = AF_INET;
		break;
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
else{
    printf("[%s] lyrebird.server: PID %i on host %s, port %i\n", CurrTime(&ltime), getpid(), inet_ntoa(sockbuffer.sin_addr), ntohs(sockbuffer.sin_port));

}
}
/*
void NewClientCheck(int listensocket, int listensockfd, int clientnum, int connectsocket[], int connectedclients){


*/



void NewClientConnect(fd_set *listensockfd, fd_set *connectedclients, int listensocket, int connectsocket[], char* clientIP[][50], int* clientnum, FILE* output){
struct timeval tv;
tv.tv_sec = 1;
tv.tv_usec = 0; 

	if(select(FD_SETSIZE, listensockfd, NULL,NULL,&tv)>0){
		connectsocket[*clientnum] = accept(listensocket, (struct sockaddr*)&clientaddress, &sockaddrlen);
		printf("Successfully connected to Client %s \n", inet_ntoa(clientaddress.sin_addr));
		fprintf(output, "[%s] Successfully connected to lyrebird client %s.\n", CurrTime(&ltime),inet_ntoa(clientaddress.sin_addr));
		// put the client IP address to ClientIP array to recall it later
		strcpy (clientIP[*clientnum],inet_ntoa(clientaddress.sin_addr));
//		printf("clientIP added is %s \n", clientIP[*clientnum]);
//		printf("previous clientIP added is %s \n", clientIP[clientnum-1]);
		// put connectsocket into fdset for incoming client messages
		FD_SET(connectsocket[*clientnum], connectedclients);
		(*clientnum)++;

	}
	else {
	FD_SET(listensocket,listensockfd);} 


}





int NewClientMessageCheck(fd_set *connectedclients, int clientnum, int connectsocket[], char* clientIP[][50], char* str, FILE* output){

struct timeval tv;
tv.tv_sec = 1;
tv.tv_usec = 0; 
int msgtype = 0;
int msgsize = 0;


// check if connected clients are free
int ret = select(FD_SETSIZE, connectedclients, NULL,NULL,&tv);
if(ret>0){
printf("client connected has a message \n");
// find out which client caused it 
	for(int i =0; i<clientnum;i++){
		if(FD_ISSET(connectsocket[i], connectedclients)){
			read(connectsocket[i], &msgtype,sizeof(int));
			switch(msgtype){
	   //  msgtype is a status ready from child, send a status 1 for more output work.			
		      case 1:{ 
		      	printf("Client from %s is ready. Send work. \n", clientIP[i]);

		      	int strlength = strlen(str);
				write(connectsocket[i], &strlength, sizeof(int));
				printf("message being sent is %s\n", str);
				write(connectsocket[i], str, strlen(str));
				// everything below this could be some function we make! =)
				char *inouttemp = strtok(str, " ");
				inouttemp = strtok(inouttemp, "\n");
       		    char *input = inouttemp;
				fprintf(output, "[%s] The lyrebird client %s has been given the task of decrypting %s.\n", CurrTime(&ltime),clientIP[i], input);
				break;}

		// msgtype is an incoming message, receieve msg and output to log		
      		  case 2:{
      		  	printf("Client has a message. Getting msg length \n");
				read(connectsocket[i], &msgsize,sizeof(int));
				char msgbuffer[msgsize+1];
				bzero(&msgbuffer, msgsize+1);
				printf("incoming message is %i long, reading message. \n",msgsize);
				read(connectsocket[i], msgbuffer, msgsize);
				printf("recieved message is :%s \n", msgbuffer);
				fprintf(output,"[%s] The lyrebird client %s has %s\n", CurrTime(&ltime), clientIP[i],msgbuffer);
				// after receving the message, push the new message to the client
				int strlength = strlen(str);
				write(connectsocket[i], &strlength, sizeof(int));
				printf("message being sent is %s\n", str);
				write(connectsocket[i], str, strlen(str));
				// everything below this could be some function we make! =)
				char *inouttemp = strtok(str, " ");
				inouttemp = strtok(inouttemp, "\n");
       		    char *input = inouttemp;
				fprintf(output, "[%s] The lyrebird client %s has been given the task of decrypting %s.\n", CurrTime(&ltime),clientIP[i], input);
      		  	break;}
      	//client possibily dissconnected	  	
      		  default:{
      		  	printf("Client has disconnected \n");
      		  	break;}
   		}
   		// end of if
   			break; // use this break to leave the for loop, need to also break out of while loop.

	}
// end of for
	}
//reset the SELECT for clients
for(int i = 0; i < clientnum; i++){
	FD_SET(connectsocket[i], connectedclients);}
// end of select
// break out of the while loop to get new file.
return 1;

}
else if (ret <0){

	if (strcmp(errno,"EBADF")==0){
	// some one has dissconnected.

	for (int i =0; i< clientnum; i++){
	if(write(connectsocket[i], &jobsfinished, sizeof(int))<0)
		// clientIP[i] has disconnected
		printf("[%s] lyrebird client %s has disconnected unexpectedly \n");
		}
	// get remaining messages.

	}

}


else{//reset the SELECT for clients
for(int i = 0; i < clientnum; i++){
	FD_SET(connectsocket[i], connectedclients);}}

return 0;

}

void remainingwork(int connectsocket[], int clientnum, char* clientIP[][50], FILE* output){
int msgsize;

	for(int i=0; i<clientnum; i++){
	while(1){
	int msgtype;
	printf("checking client %i \n", i);
	if(read(connectsocket[i], &msgtype, sizeof(int))>0){ 
		switch(msgtype){
	   //  msgtype is a status ready from child, send a status 1 for more output work.			
		      case 1:{ 
		      	printf("Child from %s is ready. No work to be given. \n", clientIP[i]);
				break;}

		// msgtype is an incoming message, receieve msg and output to log		
      		  case 2:{
      	printf("Client has a message. Getting msg length \n");
		read(connectsocket[i], &msgsize,sizeof(int));
		if(msgsize > 1){
		char msgbuffer[msgsize+1];
		bzero(&msgbuffer, msgsize+1);
		printf("incoming message is %i long, reading message. \n",msgsize);
		read(connectsocket[i], msgbuffer, msgsize);
		printf("[%s] The lyrebird client %s has %s\n", CurrTime(&ltime), clientIP[i],msgbuffer);
		fprintf(output,"[%s] The lyrebird client %s has %s\n", CurrTime(&ltime), clientIP[i],msgbuffer);
      		  	break;}
      		  	else break; // msg is not a real messsage
      		  }
      	//client possibily dissconnected	  	
      		  default:{
      		  	break;}
   		}
   	}
   	else break;
}
}

}