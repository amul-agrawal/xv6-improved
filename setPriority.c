// 8c60c451ba0933cf2b4c7e40967bfa38
#include "types.h"
#include "fcntl.h"
#include "stat.h"
#include "user.h"

int myAtoi(char* str)
{
    // Initialize result
    int res = 0;
    // printf(2, "%s \n", str);
    // Iterate through all characters
    // of input string and update result
    // take ASCII character of corosponding digit and
    // subtract the code from '0' to get numerical
    // value and multiply res by 10 to shuffle
    // digits left to update running total
    for (int i = 0; str[i] != '\0'; ++i) {
        if(str[i]>='0' && str[i]<='9') {
            res = res * 10 + str[i] - '0';
        } else {
            return -1;
        }
    }
 
    // return result.
    return res;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf(2, "Invalid number of arguments\n");
        exit();
    }

    // printf(2, "priority: %s pid: %s\n", argv[1], argv[2]);

    int priority = myAtoi(((char *)argv[1]));
    int pid = myAtoi((char *)argv[2]);

    if(priority < 0 || pid < 0 ) {
        printf(2, "Your are rspble for your mistakes, don't blame devs.\n");
        exit();
    }
    
    // printf(2, "priority: %d pid: %d\n", priority, pid);
    
    if(set_priority(priority, pid) < 0) {
        printf(2, "set_priority() error.\n");
    } 
    exit();
}