// Simple pingpong. 'ping-pong' a byte between two processes over a pair of pipes, one for each direction.
// The parent should send a byte to the child; the child should print "<pid>: received ping", where <pid> is its process ID, 
// write the byte on the pipe to the parent, and exit; the parent should read the byte from the child, print "<pid>: received pong", and exit.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int p1[2]; // parent -> child
    int p2[2]; // child -> parent
    if(pipe(p1) < 0 || pipe(p2) < 0){
        // pipe failed
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    char byte = 'A';
    int pid = fork();
    if(pid < 0){
        // child process failed
        fprintf(2, "fork failed\n");
        exit(1);
    } else if(pid == 0){
        // child process
        close(p1[1]); // child only use p1[0] to read and p2[1] to write
        close(p2[0]);

        read(p1[0], &byte, 1); // read ping from parent
        printf("%d: received ping\n", getpid());

        write(p2[1], &byte, 1); // write pong to parent
        close(p1[0]);
        close(p2[1]);
        exit(0);
    }
    else{
        // parent process
        close(p1[0]); // parent only writes to p1
        close(p2[1]); // child only reads from p2
        write(p1[1], &byte, 1); // send ping to child
        
        read(p2[0], &byte, 1); // wait response from child
        printf("%d: received pong\n", getpid());
        close(p1[1]);
        close(p2[0]);
        wait(0); // wait for child to exit
    }

    exit(0);
}