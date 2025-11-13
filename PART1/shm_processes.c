#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>

void DadProcess(int *BankAccount, sem_t *mutex);
void StudentProcess(int *BankAccount, sem_t *mutex);
void MomProcess(int *BankAccount, sem_t *mutex);

int main(int argc, char *argv[]) {

    int numParents, numKids;
    int ShmID;
    int *BankAccount;
    sem_t *mutex;
    int i;

    if (argc < 3) {
        printf("Usage: %s <#parents(1=dad,2=dad+mom)> <#students>\n", argv[0]);
        exit(1);
    }

    numParents = atoi(argv[1]);
    numKids = atoi(argv[2]);

    srand(time(NULL) ^ getpid());

    /* --------------------------------------------------------
       Create shared memory for ONE integer (BankAccount)
       -------------------------------------------------------- */
    ShmID = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0) {
        perror("shmget");
        exit(1);
    }

    BankAccount = (int *) shmat(ShmID, NULL, 0);
    if ((long)BankAccount == -1) {
        perror("shmat");
        exit(1);
    }

    *BankAccount = 0;

    /* --------------------------------------------------------
       Create semaphore
       -------------------------------------------------------- */
    mutex = sem_open("bankSem", O_CREAT, 0644, 1);
    if (mutex == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    /* --------------------------------------------------------
       Fork Mom (optional)
       -------------------------------------------------------- */
    if (numParents == 2) {
        pid_t momPID = fork();
        if (momPID == 0) {
            MomProcess(BankAccount, mutex);
            exit(0);
        }
    }

    /* --------------------------------------------------------
       Fork Dad
       -------------------------------------------------------- */
    if (fork() == 0) {
        DadProcess(BankAccount, mutex);
        exit(0);
    }

    /* --------------------------------------------------------
       Fork N Students
       -------------------------------------------------------- */
    for (i = 0; i < numKids; i++) {
        pid_t kidPID = fork();
        if (kidPID == 0) {
            StudentProcess(BankAccount, mutex);
            exit(0);
        }
    }

    /* Parent stays alive forever */
    while (1) {
        sleep(1);
    }

    return 0;
}

/* ------------------------------------------------------------
   Dear Old Dad Process
   ------------------------------------------------------------ */
void DadProcess(int *BankAccount, sem_t *mutex) {

    int r, localBalance, amount;

    while (1) {
        sleep(rand() % 6);
        printf("Dear Old Dad: Attempting to Check Balance\n");

        r = rand();
        sem_wait(mutex);

        localBalance = *BankAccount;

        if (r % 2 == 0) {  /* even */
            if (localBalance < 100) {
                amount = rand() % 101; /* 0–100 */
                localBalance += amount;
                printf("Dear old Dad: Deposits $%d / Balance = $%d\n",
                       amount, localBalance);
                *BankAccount = localBalance;
            } else {
                printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n",
                       localBalance);
            }
        } else {
            printf("Dear Old Dad: Last Checking Balance = $%d\n", localBalance);
        }

        sem_post(mutex);
    }
}

/* ------------------------------------------------------------
   Poor Student Process
   ------------------------------------------------------------ */
void StudentProcess(int *BankAccount, sem_t *mutex) {

    int r, localBalance, need;

    while (1) {
        sleep(rand() % 6);
        printf("Poor Student: Attempting to Check Balance\n");

        r = rand();
        sem_wait(mutex);

        localBalance = *BankAccount;

        if (r % 2 == 0) {  /* withdraw */
            need = rand() % 51;   /* 0–50 */
            printf("Poor Student needs $%d\n", need);

            if (need <= localBalance) {
                localBalance -= need;
                printf("Poor Student: Withdraws $%d / Balance = $%d\n",
                       need, localBalance);
                *BankAccount = localBalance;
            } else {
                printf("Poor Student: Not Enough Cash ($%d)\n", localBalance);
            }

        } else { /* odd: just check */
            printf("Poor Student: Last Checking Balance = $%d\n", localBalance);
        }

        sem_post(mutex);
    }
}

/* ------------------------------------------------------------
   Lovable Mom Process (Extra Credit)
   ------------------------------------------------------------ */
void MomProcess(int *BankAccount, sem_t *mutex) {

    int localBalance, amount;

    while (1) {
        sleep(rand() % 11);
        printf("Lovable Mom: Attempting to Check Balance\n");

        sem_wait(mutex);

        localBalance = *BankAccount;

        if (localBalance <= 100) {
            amount = rand() % 126;  /* 0–125 */
            localBalance += amount;
            printf("Lovable Mom: Deposits $%d / Balance = $%d\n",
                   amount, localBalance);
            *BankAccount = localBalance;
        }

        sem_post(mutex);
    }
}
