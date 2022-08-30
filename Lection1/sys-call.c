#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

int main() {
    long id0, id1;

    id0 = syscall(SYS_getgid);
    printf("syscall() pid = %ld\n", id0);

    id1 = getpid();
    printf("getpid() pid = %ld\n", id1);

    return 0;
}