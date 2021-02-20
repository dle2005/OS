#include "ssufs-ops.h"

int main()
{
    ssufs_formatDisk();
    ssufs_create("fileabc");
    int fd = ssufs_open("fileabc");
    char str[] = "!-----------------------128 Bytes of Data----------------------!!-----------------------128 Bytes of Data----------------------!";
    printf("Write Data: %d\n", ssufs_write(fd, str, BLOCKSIZE*2));
    printf("Seek: %d\n", ssufs_lseek(fd, BLOCKSIZE*2));
    printf("Write Data: %d\n", ssufs_write(fd, str, BLOCKSIZE*2));
    char buf[BLOCKSIZE*MAX_FILE_SIZE+1];
    buf[BLOCKSIZE*MAX_FILE_SIZE] = '\0';
    printf("Seek: %d\n", ssufs_lseek(fd, -BLOCKSIZE*2));
    printf("Read Data %d\n", ssufs_read(fd, buf, BLOCKSIZE * MAX_FILE_SIZE));
    printf("Data: %s\n", buf);
    ssufs_close(fd);
    ssufs_dump();
}
