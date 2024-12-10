#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

// Shared variables
int BankAccount = 0;
int Turn = 0;

void parent_process() {
    int account, balance;

    for (int i = 0; i < 25; i++) {
        // Sleep for a random time between 0 - 5 seconds
        sleep(rand() % 6);

        // Copy BankAccount to local variable
        account = BankAccount;

        // Wait until it's the parent's turn (Turn == 0)
        while (Turn != 0) {
            // Busy-waiting loop for synchronization
            usleep(1000);
        }

        // Check if the account is <= 100, else print a message
        if (account <= 100) {
            balance = rand() % 200 + 1;  // Random deposit between 1 and 200
            if (balance % 2 == 0) {  // Even number: deposit
                BankAccount += balance;
                printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, BankAccount);
            } else {  // Odd number: no deposit
                printf("Dear old Dad: Doesn't have any money to give\n");
            }
        } else {
            printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
        }

        // Set Turn to 1 to allow the child to proceed
        Turn = 1;
    }
}

void child_process() {
    int account, balance;

    for (int i = 0; i < 25; i++) {
        // Sleep for a random time between 0 - 5 seconds
        sleep(rand() % 6);

        // Copy BankAccount to local variable
        account = BankAccount;

        // Wait until it's the child's turn (Turn == 1)
        while (Turn != 1) {
            // Busy-waiting loop for synchronization
            usleep(1000);
        }

        // Randomly generate the balance the student needs
        balance = rand() % 50 + 1;  // Random amount needed by the student
        printf("Poor Student needs $%d\n", balance);

        // If the student needs less than or equal to the account, withdraw
        if (balance <= account) {
            BankAccount -= balance;
            printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, BankAccount);
        } else {
            // Otherwise, print that the student doesn't have enough cash
            printf("Poor Student: Not Enough Cash ($%d)\n", account);
        }

        // Set Turn to 0 to allow the parent to proceed
        Turn = 0;
    }
}

int main() {
    srand(time(NULL));  // Seed the random number generator

    pid_t pid = fork();  // Create a child process

    if (pid == 0) {
        // Child process
        child_process();
    } else if (pid > 0) {
        // Parent process
        parent_process();

        // Wait for the child to finish
        wait(NULL);
    } else {
        // Fork failed
        perror("Fork failed");
        return 1;
    }

    return 0;
}
