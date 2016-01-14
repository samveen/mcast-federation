#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

uint32_t urandom(void) {
    int fd;
    uint32_t rnd=0;
    fd=open("/dev/urandom",O_RDONLY|O_CLOEXEC);
    read(fd,(char*)&rnd,sizeof(rnd));
    return(rnd);
}
