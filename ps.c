// 8c60c451ba0933cf2b4c7e40967bfa38
#include "types.h"
#include "fcntl.h"
#include "stat.h"
#include "user.h"

int main(int argc, char** argv) {
    if (argc != 1) {
        printf(2, "Invalid number of arguments\n");
        exit();
    }
    procdump();
    exit();
}