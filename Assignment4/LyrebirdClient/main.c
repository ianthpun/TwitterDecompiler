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
int childprocessmsgcheck(fd_set *rfds, int PRP[][2],int EDP[][2], int listensocket, int ProcessTotal);
void childCleanup(fd_set *rfds,int listensocket, int PRP[][2], int closedchildcheck, int ProcessTotal);


const int readystatus = 1;
const int incomingmessage = 2;
const int jobsfinished = 3;
char fulldirectory[2050];



int main (int argc, char *argv[]) {
int listensocket;
int sockaddrlen = sizeof(struct sockaddr);
struct sockaddr_in serveraddress;
fd_set listenfd;
FD_ZERO(&listenfd);
int jobcount = 0;


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
printf("[%s] lyrebird client: PID %i connected to server %s on port %i. \n", CurrTime(&ltime), getpid(), argv[1],atoi(argv[2]));
// create children for processing
for(int i = 0; i < ProcessTotal; i++){
    LyrebirdPipe(EncryptionDirectoryPipe[i], ProcessReadyPipe[i]);
    CreateChildProcess(EncryptionDirectoryPipe[i], ProcessReadyPipe[i], ProcessArr[i], &rfds, i);
}
write(listensocket,&readystatus, sizeof(readystatus));

int dirsize = 0;
while(1){ 
    if(childprocessmsgcheck(&rfds,ProcessReadyPipe,EncryptionDirectoryPipe,listensocket,ProcessTotal)<0)
        break; 
}

for(int i =0; i< ProcessTotal; i++){
    close(EncryptionDirectoryPipe[i][1]);
}

fd_set s;
FD_ZERO(&s);
for (int i=0; i< ProcessTotal; i++){
    while(1){
    int msglen;
    FD_SET(ProcessReadyPipe[i][0],&s); 
    if(select(FD_SETSIZE, &s, NULL,NULL,&tv)>0){
        if(read(ProcessReadyPipe[i][0],&msglen,sizeof(int))>0){
        // get the last messages from the children
            puts("new message from child");
            char msgbuffer[msglen+1];
            bzero(&msgbuffer, msglen+1);
            // read the message
            read(ProcessReadyPipe[i][0], msgbuffer,msglen); 
            // output the msg to the socket back to home server.
            write(listensocket,&incomingmessage, sizeof(int));
            write(listensocket,&msglen, sizeof(int));
            write(listensocket,&msgbuffer,msglen);}
        else break;
        }

    }
FD_ZERO(&s);
}

// run ProcessCleanup to make sure that all chidlren closed correctly.
ProcessCleanup(ProcessTotal, ProcessArr);

close(listensocket);

// parent can now terminate.
printf("[%s] Lyrebird client: PID %i completed its tasks and is exiting succesfully.\n", CurrTime(&ltime), getpid());
exit(0);


}


int childprocessmsgcheck(fd_set *rfds, int PRP[][2], int EDP[][2], int listensocket, int ProcessTotal){

char * dir;
int dirsize = 0;
struct timeval tv;
tv.tv_sec = 2;
tv.tv_usec = 0; 
puts("checking for new messages from children");
if(select(FD_SETSIZE, rfds, NULL,NULL,&tv)>0){
puts("new message from child");
    for (int i = 0; i< ProcessTotal;i++){
            if (FD_ISSET(PRP[i][0], rfds)){
                // get the msglen coming in from the child process
                int msglen;
                read(PRP[i][0], &msglen,sizeof(int));

                if (msglen == 0)
                    break;

                // inital open the children.
                else if (msglen == 1){
                // tell server there is a child ready
                write(listensocket,&readystatus, sizeof(readystatus));
                msglen = 0;
                // read new message from server
                read(listensocket, &dirsize,sizeof(int));

                char dirbuffer[dirsize+1];
                bzero(&dirbuffer, dirsize+1);
                read(listensocket, dirbuffer, dirsize);
                // write message to children
                dir = strtok(dirbuffer, "\n");
                dirsize =strlen(dir);
                write(EDP[i][1], &dirsize, sizeof(int)); //issue is here
                write(EDP[i][1], dir, dirsize);

                }
                else{
                // children are mature, msg from children are going to be real messages
                // get the msg incoming
                char msgbuffer[msglen+1];
                bzero(&msgbuffer, msglen+1);
                read(PRP[i][0], msgbuffer,msglen);
           //     printf("msgbuffer is %s\n", msgbuffer);
                // output the msg to the socket back to home server.
                write(listensocket,&incomingmessage, sizeof(int));
                write(listensocket,&msglen, sizeof(int));
                write(listensocket,&msgbuffer,msglen);
                // read from server, wait for response.
                read(listensocket, &dirsize,sizeof(int));
                if(dirsize == 3){
                printf("Server has contacted client to finish up remaining jobs\n");
                return -1;
                }

                char dirbuffer[dirsize+1];
                bzero(&dirbuffer, dirsize+1);
                read(listensocket, dirbuffer, dirsize);
                // write message to children
                dir = strtok(dirbuffer, "\n");
                dirsize =strlen(dir);
                write(EDP[i][1], &dirsize, sizeof(int));
                write(EDP[i][1], dir, dirsize);
                }

            }
        }
// reset the FDset
for (int j= 0; j < ProcessTotal; j++){
FD_SET(PRP[j][0],rfds);} 
}
else {
puts("no new messages from children");
for (int i= 0; i < ProcessTotal; i++){
FD_SET(PRP[i][0],rfds);}}

return 0;

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
        // write an initial ready status to parent
        write(ProcessReadyPipe[1],&readystatus,sizeof(int));
        printf("child %i ready\n", getpid());
        while(1){
        // read from the pipe the length of the incoming msg into dirsize. If its null, the pipe is empty, 
        // so finish the loop with a break.
        //printf("child %i is checking for more messages\n", getpid());
        int dirsize = 0;
        printf("child %i checking for jobs\n", getpid());
        if(read(EncryptionDirectoryPipe[0], &dirsize,sizeof(int))==0){
            puts("EncryptionDirectoryPipe is closed.");
            break;}
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
//        printf("Pipe message is %i long", pipemsglen);
        write(ProcessReadyPipe[1],&pipemsglen,sizeof(int));
        write(ProcessReadyPipe[1], pipemsg, strlen(pipemsg));
        }
        else{
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








