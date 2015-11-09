


/* Assignment 3 pseudo

1) look for function that waits on pipe from parent, wait until pipe is closed from parent

2) consider different implementations for the 2 algos

3) run fgets(str, 2050, input) once, this will get the type of algo

4) we are only going to be using X processes, so they will stay until all processes are done.



fgets(str, 2050, input);

if (str = "round rousey") {

while (fgets(str, 2050, input)){   

pipefd[2];

if(pipe(pipfd) == -1){
printf("pipe was not instantiated correctly. Exiting. \n");
exit(1);}

while (processcount < corenumber -1){
	
int child = fork();


if (child == 0){
// we in the parent process
processcount++;

// put the child in an array of corenumber size.

ProcessArr[processcount] = child;


}
}




}



}

else if (str = "FIFS") {
	

}

else {	
printf("The specified algorithm is not supported. Exiting. \n");
exit(1);}


4) get num of cores, then use that to create X-1 children


int corenumber = getcorefunction();

