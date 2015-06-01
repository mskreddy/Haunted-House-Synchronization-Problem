										ReadMe.txt
									=============================	
Project Name 
=======================
Operating System Project3 - Synchronization

Team Members
=======================
Sai Kiran Reddy Malikireddy ( U63053381)
Kiranmayi Karri (U81100483)

Overview
=======================
In this project, we implemented functionality for synchronization challenge using semaphores. This functionality has been tested on C4 Lab Machine.
The input file provided with various number of visitors to enter the haunted house.
In this code, we dealt with the problems such as deadlock, starvation, synchronization, FIFO order.

In the implementation of the program we have abide to the given requirements as such as the first entered visitor should exit first. 
Visitor entering in one direction should stay for some time in the house and exit from the other direction. 
Here we have taken care that when ever the visitor in the house is in one direction the visitors in the opposite direction should wait. 
At most three visitors can stay in the house at any specific time.

To compile the codes 
=======================
use makefile (we have used gcc compiler)
'make' command
If any issues with the make file, program can be compiled using

gcc -o visitor visitor.c -lpthread -std=c99 -w

To execute the codes 
=======================
visitor.c 
-------------------------------------------------------------------
./visitor input.txt 2


Note: Here the arguments(Input file names and time should be changed accordingly)
where input.txt is the first argument with all the sequence of visitors entering.
Second argument is the time that each visitor should stay in the haunted house(in this case our critical section).

Note: This code has been tested using 15 input files( where each input file has different sequence of visitors).

The outputs can be found in the Output_Tested Scenarios folder.

Included items:
Code (visitor.c)
Makefile
ReadMe
Input files
Output files