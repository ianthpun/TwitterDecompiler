Ian Pun
Student number : 301167944
itpun@sfu.ca
CMPT 300 D100
Instructor: Brian Booth
TA: Scott Kristjanson

DESCRIPTION:
Lyrebird is a program that will analyse and decrypt any encrypted text messages that was sent from these "no-so-nice-people". It will use the methods that was discovered by the mathematician:

1) remove every 8th character 
2)change all groups of 6 characters to base 41 and then expand it positionally
3) Run Modular Exponentiation using exponent 1921821779 and mod 4294434817
4) Run the inverse of step 2 to get the decrypted text.

Due to the amount of twitter messages needed to be decrypted, Lyrebird can now run its decryption with an optimized number of processes with your selection of process management algorithm. It will do this by first taking in the algorithm in the first line of the config file, then taking in a list text with directories of all the decrypted messages and its designated output directory. 

HOW TO COMPILE:

Using terminal, go to the directory with the source files and enter "make all". The makefile should automatically compile it into an executable called Lyrebird.

HOW TO USE:

To execute the program, save the config text into the same directory as Lyrebird. Using terminal, go to the directory and enter this command:

"./lyrebird listnamehere.txt"

The first line in your config file should be either "round robin" or "fcfs", which will indicate to Lyrebird which algorithm you want to use. Anything else as the first line will cause an error.

After that, each line in listnamehere.txt will include the input directory and the output directory parted by a space.

SOURCES:

mod_exps(ull base) function was derived from the referenced formaula given in 
http://www.sanfoundry.com/cpp-program-implement-modular-exponentiation-algorithm/

I learned how to convert from base 10 to any other base through this website:
http://mathbits.com/MathBits/CompSci/Introduction/frombase10.htm

I learned a bit about how to implement the time function from this stackoverflow article:
http://stackoverflow.com/questions/26900122/c-program-to-print-current-time

I learned how to use the waitpid() function to catch any errors from this site:
http://man7.org/linux/man-pages/man2/wait.2.html

Learning about select() and how to use it to work with FCFS
https://www.gnu.org/software/libc/manual/html_node/Waiting-for-I_002fO.html

Creating pipes: 
http://stackoverflow.com/questions/12683282/c-how-do-you-use-select-with-multiple-pipes
 
http://www.gnu.org/software/libc/manual/html_node/Creating-a-Pipe.html
 
Core Number:
http://stackoverflow.com/questions/4586405/get-number-of-cpus-in-linux-using-c

