// 8c60c451ba0933cf2b4c7e40967bfa38

#include "types.h"
#include "fcntl.h"
#include "stat.h"
#include "user.h"

int main(int argc, char** argv) {

    if (argc <= 1) {
        printf(2, "Invalid number of arguments \n");
        exit();
    }

    int pid = fork();
    if (pid < 0) {
        printf(2, "Forking error \n");
        exit();
    } else if (pid == 0) {
        if (exec(argv[1], argv + 1) < 0) {
            printf(2, "Unable to run command\n");
            exit();
        }
    } else {
        int wtime, rtime;
        int wid = waitx(&wtime, &rtime);
        printf(1, "%s\nProcess ID: %d\nWaiting time: %d\nRunning time: %d\n\n", argv[1], wid, wtime, rtime);
        exit();
    }
}