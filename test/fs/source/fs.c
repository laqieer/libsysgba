
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <gba_sys.h>

#define MAX_BUFFER_SIZE 100

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

    /*Char array to store string */
    char str[MAX_BUFFER_SIZE] = {0};

    int err = 0;

    int c;

    int fd = 0;
    fd = open("gbfs:test.txt", O_RDONLY);

    if (fd == -1)
    {
        perror("open: Error");
    }
    else
    {
        puts("open: OK");

        ssize_t count = read(fd, str, sizeof(str));

        if (count == -1)
        {
            perror("read: Error");
        }
        else
        {
            puts("read: OK");
        }

        printf("test.txt:\n%s", str);

        err = close(fd);

        if(err)
        {
            perror("close: Error");
        }
        else
        {
            puts("close: OK");
        }
    }

    FILE *fp = NULL;
    /*Opening the file in "r" mode*/
    fp = fopen("test.txt", "r");

    /*Error handling for file open*/
    if (fp == NULL)
    {
        perror("fopen: Error");
    }
    else
    {
        puts("fopen: OK");

        puts("test.txt:");

        errno = 0;

        fread(str, MAX_BUFFER_SIZE, 1, fp);

        if(errno)
        {
            perror("fread: Error");
        }
        else
        {
            printf("%s",str);

            puts("fread: OK");
        }

        errno = 0;

        err = fseek(fp, 0, SEEK_SET);

        if (err)
        {
            perror("fseek: Error");
        }
        else
        {
            puts("fseek: OK");
        }

        errno = 0;

        puts("test.txt:");

        errno = 0;

        int count = fscanf(fp, "%s", str);

        if (errno) 
        {
            perror("fscanf: Error");
        } 
        else 
        {
            puts("fscanf: OK");
            if (count != EOF)
            {
                printf("%s", str);
            }
        }

        errno = 0;

        do
        {
            c = fgetc(fp);
            if( feof(fp) )
            {
                break ;
            }
            printf("%c", c);
        }while(1);

        if (errno)
        {
            perror("fgetc/feof: Error");
        }
        else
        {
            puts("fgetc/feof: OK");
        }

        err = fseek(fp, 0, SEEK_SET);

        if (err)
        {
            perror("fseek: Error");
        }
        else
        {
            puts("fseek: OK");
        }

        errno = 0;

        puts("test.txt:");

        /*Loop for reading the file till end*/
        for(int i = 0; fgets(str, MAX_BUFFER_SIZE, fp); i++)
        {
            // <<fgets>>---get character string from a file or stream
            // Supporting OS subroutines required: <<close>>, <<fstat>>, <<isatty>>, <<lseek>>, <<read>>, <<sbrk>>, <<write>>.
            printf("%d:%s", i, str);
        }

        if (errno)
        {
            perror("fgets: Error");
        }
        else
        {
            puts("fgets: OK");
        }

        errno = 0;

        /*Closing the input file after reading*/
        err = fclose(fp);

        if (err)
        {
            perror("fclose: Error");
        }
        else
        {
            puts("fclose: OK");
        }
    }

	while (1) {
		VBlankIntrWait();
	}
}


