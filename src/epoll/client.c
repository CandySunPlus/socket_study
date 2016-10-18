// osx not support epoll, use kqueue
#include <sys/event.h>
#include "../config.h"

int main(int argc, char *argv[])
{
    
    return 0;
}
