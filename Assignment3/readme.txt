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

Due to the amount of twitter messages needed to be decrypted, Lyrebird can now run its decryption with multiple processes. It will do this by taking in a list text with directories of all the decrypted messages and its designated output directory. 

HOW TO COMPILE:

Using terminal, go to the directory with the source files and enter "make". The makefile should automatically compile it into an executable called Lyrebird.

HOW TO USE:

To execute the program, save the list text into the same directory as Lyrebird. Using terminal, go to the directory and enter this command:

"./lyrebird listnamehere.txt"

Each line in listnamehere.txt will include the input directory and the output directory parted by a space.

SOURCES:

mod_exps(ull base) function was derived from the referenced formaula given in 
http://www.sanfoundry.com/cpp-program-implement-modular-exponentiation-algorithm/

I learned how to convert from base 10 to any other base through this website:
http://mathbits.com/MathBits/CompSci/Introduction/frombase10.htm

I learned a bit about how to implement the time function from this stackoverflow article:
http://stackoverflow.com/questions/26900122/c-program-to-print-current-time

I learned how to use the waitpid() function to catch any errors from this site:
http://man7.org/linux/man-pages/man2/wait.2.html


