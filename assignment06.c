
/*
 * Assignment 06: Majority Voting
 *
 * One parent process acts as the judge.
 * N child processes act as voters.
 * Each voter has its own pipe to send votes to the judge.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main(void)
{
    int N;
    int maxRounds;

    // Step 1: Ask the user for the number of voters and rounds.
    printf("Enter the number of voters (at least 3): ");
    fflush(stdout);

    if (scanf("%d", &N) != 1 || N < 3) {
        fprintf(stderr, "Error: Number of voters must be at least 3.\n");
        return 1;
    }

    printf("Enter the maximum number of rounds: ");
    fflush(stdout);

    if (scanf("%d", &maxRounds) != 1 || maxRounds < 1) {
        fprintf(stderr, "Error: Number of rounds must be at least 1.\n");
        return 1;
    }

    // Each pipe has two file descriptors:
    // pipeFD[i][0] is the read end        pipeFD[i][1] is the write end.
    int (*pipeFD)[2] = malloc(N * sizeof(*pipeFD));

    if (pipeFD == NULL) {
        perror("malloc");
        return 1;
    }

    // The judge's ID is -1.
    // Each child stores its own voter ID.
    int voterID = -1;

    // Step 2: Create N pipes before creating any children.
    for (int i = 0; i < N; i++) {
        if (pipe(pipeFD[i]) == -1) {
            perror("pipe");
            return 1;
        }
    }

    // Step 3: Create N child processes.
    // Only the judge forks. Each child breaks out of this loop
    // so it cannot create additional child processes.
    for (int i = 0; i < N; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            return 1;
        }

        if (pid == 0) {
            // This process is voter i.
            voterID = i;
            break;
        }
    }

    if (voterID == -1) {
        // The parent is the judge.
        // The judge only reads from the pipes.
        for (int i = 0; i < N; i++) {
            close(pipeFD[i][1]);
        }
    } else {
        // A child voter only writes to its own pipe.
        for (int i = 0; i < N; i++) {
            close(pipeFD[i][0]);  // Close every read end.

            if (i != voterID) {
                close(pipeFD[i][1]); // Close other voters' write ends.
            }
        }
    }

    // Step 4: All processes participate in each round.
    for (int round = 1; round <= maxRounds; round++) {

        if (voterID == -1) {
            // JUDGE: Collect one vote from every voter.
            int yesVotes = 0;
            int noVotes = 0;

            printf("\n--- Round %d ---\n", round);

            for (int i = 0; i < N; i++) {
                int vote;

                // Read the vote sent by voter i.
                ssize_t bytesRead = read(
                    pipeFD[i][0], &vote, sizeof(vote)
                );

                if (bytesRead != sizeof(vote)) {
                    perror("read");
                    return 1;
                }

                if (vote == 1) {
                    yesVotes++;
                } else {
                    noVotes++;
                }
            }

            // The judge announces the result after collecting
            // all N votes.
            printf("Judge: YES = %d, NO = %d\n",
                   yesVotes, noVotes);

            if (yesVotes > N / 2) {
                printf("Judge's decision: YES (majority)\n");
            } else {
                printf("Judge's decision: NO (no YES majority)\n");
            }

            fflush(stdout);

        } else {
            // VOTER: Randomly choose YES (1) or NO (0).
            // Seed each voter separately.
            if (round == 1) {
                srand((unsigned int)time(NULL)
                      ^ ((unsigned int)getpid() << 16));
            }

            int vote = rand() % 2;

            printf("Voter %d votes %s in round %d\n",
                   voterID, vote == 1 ? "YES" : "NO", round);
            fflush(stdout);

            // Send the vote using this voter's own pipe.
            // No loop is needed to select the pipe.
            if (write(pipeFD[voterID][1],
                      &vote, sizeof(vote)) != sizeof(vote)) {
                perror("write");
                return 1;
            }
        }
    }

    // Close the file descriptors that this process uses.
    if (voterID == -1) {
        for (int i = 0; i < N; i++) {
            close(pipeFD[i][0]);
        }

        // Wait for all child voters to finish.
        for (int i = 0; i < N; i++) {
            wait(NULL);
        }

        printf("\nJudge: All voters have finished.\n");
    } else {
        close(pipeFD[voterID][1]);
    }

    free(pipeFD);
    return 0;
}
