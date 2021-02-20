#include "ssufs-ops.h"

int main()
{
    char str[] = "!-------------48 Bytes of Data-------------!!-------------48 Bytes of Data-------------!";
    char str_2[] = "!-------32 Bytes of Data-------!!-------------48 Bytes of Data-------------!";
    ssufs_formatDisk();

    ssufs_create("f1.txt");
    int fd1 = ssufs_open("f1.txt");
    ssufs_create("f2.txt");
    int fd2 = ssufs_open("f2.txt");

    printf("Write Data: %d\n", ssufs_write(fd1, str, BLOCKSIZE + 24));
    printf("Write Data: %d\n", ssufs_write(fd2, str, BLOCKSIZE + 24));
    printf("Write Data: %d\n", ssufs_write(fd1, str_2, BLOCKSIZE + 24));

    char str_3[BLOCKSIZE * 4] = {0};
    printf("Seek: %d\n", ssufs_lseek(fd1, -BLOCKSIZE - 24));
    printf("Read Data: %d = ", ssufs_read(fd1, str_3, BLOCKSIZE));
    printf("%s\n", str_3);

    ssufs_dump();
    ssufs_delete("f2.txt");
    ssufs_delete("f1.txt");
    ssufs_dump();
}
