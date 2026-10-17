# CS4420_assignment06

# Assignment 06: Majority Voting

## 1. Introduction

The purpose of this assignment is to implement a majority voting system in C using processes and pipes. The program consists of one parent process acting as the judge and N child processes acting as voters. Each voter randomly chooses YES or NO during each round and sends its vote to the judge. The judge collects the votes and determines the result using the majority rule.

## 2. Process Communication Diagram

The judge creates one pipe for each voter and creates the child processes using `fork()`. Each voter uses its own pipe to send votes to the judge.


                  Judge (Parent)
                  /      |      \
                 /       |       \
             Pipe 0    Pipe 1   Pipe N-1
                ↑         ↑         ↑
             Voter 0   Voter 1   Voter N-1


Each pipe provides one-way communication from a voter to the judge. During every round, the judge reads one vote from each pipe, counts the YES and NO votes, and announces the result. The same voter processes participate in every round.

## 3. Implementation

The program first asks the user to enter the number of voters and the maximum number of rounds. It validates the input to ensure there are at least three voters and at least one round.

The program creates all pipes using pipe() before creating child processes using fork(). Each child stores its voter ID and leaves the process-creation loop so that only the parent creates child processes.

During each round, every voter randomly generates a YES or NO vote and sends it through its designated pipe using write(). The judge collects the votes using read() and calculates the totals.

The judge declares YES when more than half of the voters vote YES. Otherwise, the decision is NO. After all rounds are completed, the processes close their pipe descriptors, and the judge waits for the child processes to terminate.

## 4. Compilation and Execution

Run the following commands from the project root directory.

1. Configure the project and generate the Makefile:

cmake -S . -B build -G "Unix Makefiles"

2. Compile the program:

cmake --build build

3. Run the program:

./build/assignment6

When the program starts, enter the number of voters and the maximum number of rounds. The number of voters must be at least three, and the number of rounds must be at least one.

Cleaning the Build

To remove the generated build directory, run:

rm -rf build

To rebuild the program afterward, repeat the configuration, compilation, and execution steps.


Enter the number of voters (at least 3) and the maximum number of rounds when prompted.

## 5. Results and Screenshots

The program was tested with different numbers of voters and rounds. Since votes are generated randomly, the results may differ between executions.

For each round, the judge collects all votes and displays the YES and NO totals. The totals should always add up to the number of voters. YES is declared the winner only when it receives more than half of the votes.

## 6. Conclusion

This assignment demonstrates interprocess communication in C using pipes and processes. It provides experience with pipe(), fork(), read(), write(), and wait(). The program models a voting system in which multiple child processes communicate their decisions to a parent process that determines the final outcome using the majority rule.
