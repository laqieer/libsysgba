
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <gba_sys.h>

#define BUFFER_SIZE 20

//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------


	// the vblank interrupt must be enabled for VBlankIntrWait() to work
	// since the default dispatcher handles the bios flags no vblank handler
	// is required
	irqInit();
	irqEnable(IRQ_VBLANK);

	consoleDemoInit();

    fsInit();

    int err = 0;

    char buffer[BUFFER_SIZE] = {0};

    // test 1: access with file descriptor

    int fd = open("sram:", O_RDWR);
    if (fd == -1)
    {
        perror("open: Error");
        goto wait;
    }

    off_t skip = lseek(fd, 10, SEEK_SET);
    if (skip == -1)
    {
        perror("lseek: Error");
        goto wait;
    }

    ssize_t writeNum = write(fd, "sram test 1: OK\n", BUFFER_SIZE);
    if (writeNum == -1)
    {
        perror("write: Error");
        goto wait;
    }
    printf("%d bytes written to sram\n", writeNum);

    lseek(fd, 10, SEEK_SET);

    ssize_t readNum = read(fd, buffer, BUFFER_SIZE);
    if (readNum == -1)
    {
        perror("read: Error");
        goto wait;
    }
    printf("%d bytes read from sram\n", readNum);

    err = close(fd);
    if (err)
    {
        perror("close: Error");
        goto wait;
    }

    puts(buffer);

    // test 2: access with file stream
    
    FILE *fp = fopen("sram:", "r+");
    if (fp == NULL)
    {
        perror("fopen: Error");
        goto wait;
    }

    fputs("sram test 2: OK\n", fp);
    rewind(fp);
    fgets(buffer, BUFFER_SIZE, fp);
    fclose(fp);

    puts(buffer);

wait:
	while (1) {
		VBlankIntrWait();
	}
}


