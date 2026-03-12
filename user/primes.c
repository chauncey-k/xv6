// Simple primes, use pipes and forks to create a chain of processes to filter out prime numbers.
// The parent process should send the numbers 2..35 to the first child process.
// create one process that reads from its left neighbor over a pipe and writes to its right neighbor over another pipe.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void
primes(int fd) { //current process reads thd read port of pipe
    int prime;
    if (read(fd, &prime, sizeof(int)) == 0) { // if no number, close fd and exit
        close(fd);
        exit(0);
    }
    printf("prime %d\n", prime); // print the prime number

    int p[2]; // create a new pipe for the next process, write to p[1], read from p[0]
    if (pipe(p) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    int pid = fork(); // create a new process
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    } else if (pid == 0) {
        // child process
        close(p[1]); // close write end of pipe(chlid only reads)
        primes(p[0]); // recursive call to primes with read end of pipe
        close(p[0]); // close read end of pipe
        exit(0);
    } else {
        // parent process
        close(p[0]); // close read end of pipe(parent only writes)
        int num;
        while (read(fd, &num, sizeof(int)) > 0) { // read data from fd
            if (num % prime != 0) { // if the data is not multiply of prime, write to p[1](child process)
                write(p[1], &num, sizeof(int));
            }
        }
        
        close(p[1]); // tell child no more numbers
        wait(0); // wait for child to exit
        close(fd); // close read end of fd
        exit(0);
    }
}

int
main(int argc, char *argv[])
{
    int p[2];

    if(pipe(p) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    for(int num = 2; num <= 35; num++) {
        write(p[1], &num, sizeof(int)); // write source to pipe
    }
    close(p[1]); // close write end of pipe
    
    primes(p[0]); // start the chain of processes, primes need p[0] fd to read data
    close(p[0]); // close read end of pipe
    
    exit(0);
}