#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>

// Shared variables
int *BankAccount;
int *Turn;

void dearOldDad() {
    for (int i = 0; i < 25; i++) {
        sleep(rand() % 6); // Sleep 0-5 seconds
        int account = *BankAccount;

        // Wait for turn
        while (*Turn != 0) {
            // Busy wait (no-op)
        }

        if (account <= 100) {
            int balance = (rand() % 101); // Randomly generate amount between 0-100
            if (balance % 2 == 0) {
                account += balance;
                printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account);
            } else {
                printf("Dear old Dad: Doesn't have any money to give\n");
            }
        } else {
            printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
        }

        *BankAccount = account;
        *Turn = 1; // Give turn to child
    }
}

void poorStudent() {
    for (int i = 0; i < 25; i++) {
        sleep(rand() % 6); // Sleep 0-5 seconds
        int account = *BankAccount;

        // Wait for turn
        while (*Turn != 1) {
            // Busy wait (no-op)
        }

        int balance = (rand() % 51); // Randomly generate amount between 0-50
        printf("Poor Student needs $%d\n", balance);
        if (balance <= account) {
            account -= balance;
            printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
        } else {
            printf("Poor Student: Not Enough Cash ($%d)\n", account);
        }

        *BankAccount = account;
        *Turn = 0; // Give turn to parent
    }
}

int main() {
    // Seed random number generator
    srand(time(NULL));

    // Allocate shared memory
    BankAccount = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                       MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    Turn = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // Initialize shared variables
    *BankAccount = 0;
    *Turn = 0;

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process
        poorStudent();
    } else {
        // Parent process
        dearOldDad();
        wait(NULL); // Wait for child to finish
    }

    // Clean up shared memory
    munmap(BankAccount, sizeof(int));
    munmap(Turn, sizeof(int));

    return 0;
}

