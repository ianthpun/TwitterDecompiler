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


// BUGS : when all work is sent, it seems to close work being done.

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

void LyrebirdPipe(int EDPipe[], int PRPipe[]);
void CreateChildProcess(int EncryptionDirectoryPipe[], int ProcessReadyPipe[], int ProcessArr[],fd_set* rfds, int i);
void FCFSDecryption(char* job, int jobcount, int ProcessTotal, int EDPipe[][2], int PRPipe[][2], fd_set* rfds);


int main (int argc, char *argv[]) {
int listensocket;
int sockaddrlen = sizeof(struct sockaddr);
struct sockaddr_in serveraddress;
int readystatus = 1;
fd_set listenfd;
FD_ZERO(&listenfd);
int jobcount = 0;
const int incomingmessage = 2;
char fulldirectory[2050];


// from CreateChildProcess
int CoreNumber = sysconf(_SC_NPROCESSORS_ONLN);
int ProcessTotal = CoreNumber-1;
int EncryptionDirectoryPipe[ProcessTotal][2];
int ProcessReadyPipe[ProcessTotal][2]; 
int ProcessArr[ProcessTotal];
fd_set rfds;
FD_ZERO(&rfds);

struct timeval tv;
tv.tv_sec = 2;
tv.tv_usec = 0; 

char* dir;
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
// create children for processing
for(int i = 0; i < ProcessTotal; i++){
    LyrebirdPipe(EncryptionDirectoryPipe[i], ProcessReadyPipe[i]);
    CreateChildProcess(EncryptionDirectoryPipe[i], ProcessReadyPipe[i], ProcessArr[i], &rfds, i);
}

// write that I have a child ready !!!!!!!!!!! 
write(listensocket,&readystatus, sizeof(readystatus));
int dirsize = 0;
while(1){
// get the incoming job from Server
puts("checking for new messages from server");
if(select(FD_SETSIZE, &listenfd, NULL,NULL,&tv)>0){
	read(listensocket, &dirsize,sizeof(int));
	// check to see if socket was closed = jobs are done.
	if(dirsize == 0){
		//printf("return of read is 0, leaving while loop. \n");
		break;
	}
	printf("new message incoming from server \n");
	char dirbuffer[dirsize+1];
	bzero(&dirbuffer, dirsize+1);
	read(listensocket, dirbuffer, dirsize);
	printf("recieved message is :%s \n", dirbuffer);
	// give it to one of the children that is ready.
// try this new method!!!!!!!!!!!!!!!!!!
// copy the buffer to a full directory for processing later
strcpy(fulldirectory, dirbuffer);

if (jobcount < ProcessTotal){
printf("job count is %i\n", jobcount);
dir = strtok(dirbuffer, "\n");
dirsize = strlen(dir);
write(EncryptionDirectoryPipe[jobcount][1], &dirsize, sizeof(int));
write(EncryptionDirectoryPipe[jobcount][1], dir, strlen(dir));
jobcount++;
// write that i have a child ready so we can get fill up the children
write(listensocket,&readystatus, sizeof(readystatus));
}
// add the socket back to the fd_set
FD_SET(listensocket, &listenfd);
}
else {
    puts("no new msgs from server");
    puts(dir);
    FD_SET(listensocket, &listenfd);}

// check if there are any new messages from the children! 
puts("checking for new messages from children");
if(select(FD_SETSIZE, &rfds, NULL,NULL,&tv)>0){
puts("new message from child");
    for (int i = 0; i< ProcessTotal;i++){
            if (FD_ISSET(ProcessReadyPipe[i][0], &rfds)){
                // get the msglen coming in from the child process
                int msglen;
                read(ProcessReadyPipe[i][0], &msglen,sizeof(int));
                if (msglen == 0)
                    break;
                // get the msg incoming
                char msgbuffer[msglen+1];
                bzero(&msgbuffer, msglen+1);
                read(ProcessReadyPipe[i][0], msgbuffer,msglen);
           //     printf("msgbuffer is %s\n", msgbuffer);
                // output the msg to the socket back to home server.
                write(listensocket,&incomingmessage, sizeof(int));
                write(listensocket,&msglen, sizeof(int));
                write(listensocket,&msgbuffer,msglen);
                // write the next encryption directory to the responding child
                dir = strtok(fulldirectory, "\n");
                dirsize = strlen(dir);
                printf("dirlength before sending to child is :%i\n",dirsize);
                write(EncryptionDirectoryPipe[i][1], &dirsize, sizeof(int));
                printf("dir before sending to child is :%s\n",dir);
                write(EncryptionDirectoryPipe[i][1], dir, dirsize);
                
                // reset the FDset
                for (int i= 0; i < ProcessTotal; i++){
                FD_SET(ProcessReadyPipe[i][0],&rfds);} 
                break;
            }
            }
}
else {
puts("no new messages from children");
for (int i= 0; i < ProcessTotal; i++){
FD_SET(ProcessReadyPipe[i][0],&rfds);}}
}







printf("no more messages, exiting \n");
close(listensocket);
/*
// jobs are all given out, close off all your encryptiondirectorypipes and
//clean the rest up.
for (int i = 0; i< ProcessTotal; i++){
close(EncryptionDirectoryPipe[i][1]);}

ProcessCleanup(ProcessTotal, ProcessArr);
*/

exit(0);

}


void FCFSDecryption(char* job, int jobcount, int ProcessTotal, int  EDPipe[][2], int  PRPipe[][2], fd_set* rfds){

int directorysize;
// timeval is used for the SELECT function
struct timeval tv;
tv.tv_sec = 100;
tv.tv_usec = 0; 

// direct all the children with a first set of work first
if (jobcount < ProcessTotal){
char* dir = strtok(job, "\n");
directorysize = strlen(dir);
printf("job count is %i\n", jobcount);
write(EDPipe[jobcount][1], &directorysize, sizeof(int));
write(EDPipe[jobcount][1], dir, strlen(dir));
}
// next, wait on select to see when the child has a msg using SELECT, then 
//check to see which process it is, send the msg to server, then send the next job to it.
/*else {


}*/

}




void LyrebirdPipe(int EDPipe[], int PRPipe[]){

if(pipe(EDPipe)||pipe(PRPipe)){
    printf("[%s] Parent #%i failed to pipe. Exiting.\n",CurrTime(ltime), getpid());
    exit(-1);}

}





void CreateChildProcess(int EncryptionDirectoryPipe[], int ProcessReadyPipe[], int ProcessArr[], fd_set* rfds, int i){


int ProcessCount = 0;
// timeval is used for the SELECT function
struct timeval tv;
tv.tv_sec = 1;
tv.tv_usec = 0; 
 

int ProcessStatus = 1;
int RRcount = 0;
char* directory[2050];
int directorysize;
time_t ltime;
// stuff
const int incomingmessage = 2;

int ChildID = fork();

        if (ChildID){ // parent process
        FD_SET(ProcessReadyPipe[0],rfds);
        // close the writing end of the pipe, we willl be reading from the child for its status
        close(ProcessReadyPipe[1]);
        // close reading end, we will be writing to the child processes
        close(EncryptionDirectoryPipe[0]);
        // add the child into the ProcessArr to keep track of processes created
        ProcessArr = ChildID;
}

        else if (ChildID == 0){         // child process
        // close appropriate ends of the pipe for the child process
        close(ProcessReadyPipe[0]);
        close(EncryptionDirectoryPipe[1]);

        while(1){
        // read from the pipe the length of the incoming msg into dirsize. If its null, the pipe is empty, 
        // so finish the loop with a break.
        int dirsize = 0;
        if(read(EncryptionDirectoryPipe[0], &dirsize,sizeof(int))==0)
            break;
        // get the incoming msg using the size dirsize given in the privious msg
        // create character array with null termination, then zero it out
        char directorybuffer[dirsize+1];
        bzero(&directorybuffer, dirsize+1);
        read(EncryptionDirectoryPipe[0], directorybuffer,dirsize);
        // parse the directory into input and output pointers respectfully
        char *inouttemp = strtok(directorybuffer, " ");
        char *input = inouttemp;
        inouttemp = strtok(NULL, " ");
        char *output = inouttemp;
        output = strtok(output, "\n");
        // run decryption
        printf("[%s] Child Process ID #%i will decrypt %s. \n",CurrTime(ltime),getpid(),input);
        if(decrypt(input,output) > 0){
        printf("[%s] Process ID #%i decrypted %s successfully. \n",CurrTime(ltime),getpid(),input);
        char* pipemsg;
        asprintf(&pipemsg, "successfully decrypted %s in process %i.", input, getpid());
        int pipemsglen = strlen(pipemsg);
        printf("Pipe message is %i long", pipemsglen);
        write(ProcessReadyPipe[1],&pipemsglen,sizeof(int));
        write(ProcessReadyPipe[1], pipemsg, strlen(pipemsg));
        }
        else{
        printf("decryption failed \n");
        char* pipemsg;
        asprintf(&pipemsg, "encountered an error: unable to open file %s in process %i.", input, getpid());
        int pipemsglen = strlen(pipemsg);
        write(ProcessReadyPipe[1],&pipemsglen,sizeof(int));
        write(ProcessReadyPipe[1], pipemsg, strlen(pipemsg));
        }

        }
        // Process is done all jobs. Close its writing pipe.
        printf("closing all work, jobs are done \n");
        close(ProcessReadyPipe[1]);
        exit(0);
        }
        else{
        // Parent fails to fork. Clean up previously created children prior to crash
        printf("[%s] Process ID #%i failed to create a child. \n",CurrTime(ltime), getpid());
        ProcessCleanup(i, ProcessArr); 
        exit(-1);
        }
}










