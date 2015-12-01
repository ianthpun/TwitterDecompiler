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
void NewClientConnect(fd_set *listensockfd, fd_set *connectedclients, int listensocket, int connectsocket[], int clientIP[], int* clientnum, FILE* output);
void NewClientMessageCheck(fd_set *connectedclients, int clientnum, int connectsocket[],int clientIP[], char* str, FILE* output);


#define Want_Debug

time_t ltime;
int sockaddrlen = sizeof(struct sockaddr);
struct sockaddr_in serveraddress, clientaddress;



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
char (*clientIP[500])[50]; // will make malloc later
int msgsize;
const int jobsfinished = 3;

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
	//NewClientConnect(&listensockfd, &connectedclients, listensocket, connectsocket, clientIP, &clientnum, output);
	//NewClientMessageCheck(&connectedclients, clientnum, connectsocket,clientIP, str, output);
	
	// check if any New clients have showed up
	if(select(FD_SETSIZE, &listensockfd, NULL,NULL,&tv)>0){
		connectsocket[clientnum] = accept(listensocket, (struct sockaddr*)&clientaddress, &sockaddrlen);
		printf("Successfully connected to Client %s \n", inet_ntoa(clientaddress.sin_addr));
		fprintf(output, "[%s] Successfully connected to lyrebird client %s.\n", CurrTime(&ltime),inet_ntoa(clientaddress.sin_addr));
		// put the client IP address to ClientIP array to recall it later
		clientIP[clientnum] = inet_ntoa(clientaddress.sin_addr);
		// put connectsocket into fdset for incoming client messages
		FD_SET(connectsocket[clientnum], &connectedclients);
		clientnum++;

	}
	else {
	FD_SET(listensocket,&listensockfd);} // reset FDSet 




int msgtype = 0;

	// check if connected clients are free
	if(select(FD_SETSIZE, &connectedclients, NULL,NULL,&tv)>0){
	printf("client connected has a message \n");
	// find out which client caused it 
		for(int i =0; i<clientnum;i++){
			if(FD_ISSET(connectsocket[i], &connectedclients)){
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
		FD_SET(connectsocket[i], &connectedclients);}
	// end of select
	// break out of the while loop to get new file.
	break;

	}
	else{//reset the SELECT for clients
	for(int i = 0; i < clientnum; i++){
		FD_SET(connectsocket[i], &connectedclients);}}


		}	
}

// end of file. Send a message to all the clients requesting to finish their jobs
puts("writing closing file to everyone");

for (int i =0; i<= clientnum; i++){
	if(write(connectsocket[i], &jobsfinished, sizeof(int))<0)
		perror("write");
}

puts("here now");

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
		char msgbuffer[msgsize+1];
		bzero(&msgbuffer, msgsize+1);
		printf("incoming message is %i long, reading message. \n",msgsize);
		read(connectsocket[i], msgbuffer, msgsize);
		printf("recieved message is :%s \n", msgbuffer);
		fprintf(output,"[%s] The lyrebird client %s has %s\n", CurrTime(&ltime), clientIP[i],msgbuffer);
      		  	break;}
      	//client possibily dissconnected	  	
      		  default:{
      		  	printf("Client has disconnected \n");
      		  	break;}
   		}
   	}
   	else break;
}
}



printf("[%s] lyrebird server: PID %i completed its tasks and is exiting successfully.\n", CurrTime(&ltime), getpid());



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
// havent tested this yet!!!!!!!!!!!!!!!!!!!!
    if ((strcmp(ifa->ifa_addr, "127.0.0.1")!=0) && (ifa->ifa_addr->sa_family==AF_INET)){
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
else{
    printf("[%s] lyrebird.server: PID %i on host %s, port %i\n", CurrTime(&ltime), getpid(), inet_ntoa(sockbuffer.sin_addr), ntohs(sockbuffer.sin_port));

}
}
/*
void NewClientCheck(int listensocket, int listensockfd, int clientnum, int connectsocket[], int connectedclients){


*/



void NewClientConnect(fd_set *listensockfd, fd_set *connectedclients, int listensocket, int connectsocket[], int clientIP[], int* clientnum, FILE* output){
struct timeval tv;
tv.tv_sec = 1;
tv.tv_usec = 0; 

	if(select(FD_SETSIZE, listensockfd, NULL,NULL,&tv)>0){
		connectsocket[*clientnum] = accept(listensocket, (struct sockaddr*)&clientaddress, &sockaddrlen);
		printf("Successfully connected to Client %s \n", inet_ntoa(clientaddress.sin_addr));
		fprintf(output, "[%s] Successfully connected to lyrebird client %s.\n", CurrTime(&ltime),inet_ntoa(clientaddress.sin_addr));
		// put the client IP address to ClientIP array to recall it later
		clientIP[*clientnum] = inet_ntoa(clientaddress.sin_addr);
		// put connectsocket into fdset for incoming client messages
		FD_SET(connectsocket[*clientnum], connectedclients);
		(*clientnum)++;

	}
	else {
	FD_SET(listensocket,listensockfd);} // reset FDSet 


}





void NewClientMessageCheck(fd_set *connectedclients, int clientnum, int connectsocket[],int clientIP[], char* str, FILE* output){

struct timeval tv;
tv.tv_sec = 1;
tv.tv_usec = 0; 
int msgtype = 0;
int msgsize = 0;


// check if connected clients are free
if(select(FD_SETSIZE, connectedclients, NULL,NULL,&tv)>0){
printf("client connected has a message \n");
printf("clientnum is %i\n", clientnum);
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
	}
// end of for
	}
//reset the SELECT for clients
for(int i = 0; i < clientnum; i++){
	FD_SET(connectsocket[i], connectedclients);}
// end of select
}
else{
//reset the SELECT for clients
for(int i = 0; i < clientnum; i++){
	FD_SET(connectsocket[i], connectedclients);}
}


}

