#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

typedef enum Errors {
    INVALID_MEMORY,
    INVALID_ARG,
    INVALID_FILE,
    INVALID_DATA,
    INVALID_LIMIT,
    INVALID_WRITTEN,
    INVALID_READ,
    INVALID_PATH,
    INVALID_SYSCALL,
    OK
} Errors;

Errors list_directory(const char *path) {
    int fd = open(path, O_RDONLY | O_DIRECTORY);
    if (fd == -1) {
        return INVALID_PATH;
    }

    char buffer[1024];
    ssize_t bytes_read;

    while ((bytes_read = syscall(SYS_getdents64, fd, buffer, sizeof(buffer))) > 0) {
        for (char *ptr = buffer; ptr < buffer + bytes_read; ptr += *(unsigned short *)(ptr + 16)) { //de_reclen
            char *d_name = ptr + 19;
            ino64_t inode = *(ino64_t *)ptr; //inode файла
            unsigned char d_type = *(ptr + 18); //тип файла

            if (*d_name != '.') {   
                if (d_type == 0) {
                    printf("   unknown: ");
                } else if (d_type == 1) {
                    printf("   fifo/pipe: ");
                } else if (d_type == 2) {
                    printf("   char device: ");
                } else if (d_type == 4) {
                    printf("   directory: ");
                } else if (d_type == 6) {
                    printf("   block device: ");
                } else if (d_type == 8) {
                    printf("   file: ");
                } else if (d_type == 10) {
                    printf("   symlink: ");
                } else if (d_type == 12) {
                    printf("   socket: ");
                } else {
                    printf("   other: ");
                }
                
                printf("%s [inode: %lu]\n", d_name, inode);
                
            }
        }
    }

    if (bytes_read == -1) {
        close(fd);
        return INVALID_SYSCALL;
    }

    close(fd);
    return OK;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Write: %s <directory> ...? <directory>\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        printf("Directory: %s\n", argv[i]);
        Errors err = list_directory(argv[i]);
        if (err == INVALID_PATH) {
            printf("   INVALID_PATH\n");
        } else if(err == INVALID_SYSCALL) {
            printf("   INVALID_SYSCALL\n");
        }
    }

    return 0;
}