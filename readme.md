# Process Management System

A simple user interface that handles different predefined set of services related to process management.

At the start we will have one process main and multiple threads. Each thread will handle one of the instructions defined in the set of instructions section.

Main process will keep on taking the instructions as input and handover the command to the respective threads (created solely for that particuklar instruction). The threads will run forever waiting for their turn. Whenever an instruction is handed over to them they will handle it and then wait for their respective instruction again.

## Output Information

PRT command can be used to output information about the processes.
CURRENT TIME: time

RUNNING PROCESS:
pid, value, start time, CPU time used so far

BLOCKED PROCESSES:
pid, value, start time, CPU time used so far

PROCESSES READY TO BE EXECUTED:
pid, value, start time, CPU time used so far

## Set of Instructions

CRT ins.txt -> Add a new process to the process pool. ins.txt is a file containing the instructions of the newly created process.

INC -> It will run the next instruction of the running process.

PRT -> Output information on the terminal about running, blocked and ready processes.

UNB -> Unblock the top process in the blocked queue.

end -> End the program.

## Process Instructions

S n -> Set the value of the integer variable to n, where n is an integer.

A n -> Add n to the value of integer variable.

D n -> Subtract n from the value of integer variable.

B -> Block this process.

R filename -> Replace the program of the simulated process with the program in the file filename, and set program counter to the first instruction of this new program.

## Sample Process File

S 1000
A 19
A 20
D 53
B
R filename
A 21
A 0
D 3

## Context Switching

Lets suppose there are two process running, after every 3 seconds there will be a context switching between the two processes. The running process would pause its execution and the other process would start running. If there are more than two processes running, the context switching will happen between all of them after every 3 seconds.