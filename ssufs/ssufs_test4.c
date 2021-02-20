#include "ssufs-ops.h"

int main(){
  ssufs_formatDisk();
  ssufs_create("fileabc");
  int fd = ssufs_open("fileabc");
  char str[] = "!-----------------------64 Bytes of Data-----------------------!";
  printf("Write Data: %d\n", ssufs_write(fd, str, BLOCKSIZE));
  printf("Seek: %d\n", ssufs_lseek(fd, BLOCKSIZE));
  printf("Write Data: %d\n", ssufs_write(fd, str, BLOCKSIZE));
  printf("Seek: %d\n", ssufs_lseek(fd, BLOCKSIZE));
  char buf[BLOCKSIZE * 2 + 1];
  buf[BLOCKSIZE * 2] = '\0';
  printf("Seek: %d\n", ssufs_lseek(fd, -BLOCKSIZE * 2));
  printf("Read Data %d\n", ssufs_read(fd, buf, BLOCKSIZE * 2));
  printf("Data: %s\n", buf);
  ssufs_close(fd);
  ssufs_dump();
}
