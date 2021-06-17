
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

#define BUFFER_SIZE 100

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
    char str[BUFFER_SIZE] = {0};

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
//        puts("open: OK");

        ssize_t count = read(fd, str, sizeof(str));

        if (count == -1)
        {
            perror("read: Error");
        }
        else
        {
            puts("read: OK");
        }

        printf("%s", str);

        off_t filesize = lseek(fd, 0, SEEK_END);

        if (filesize == -1)
        {
            perror("lseek: Error");
        }
        else
        {
//            puts("lseek: OK");
            printf("size: %lld\n", filesize);
        }

        lseek(fd, 10, SEEK_SET);
        read(fd, str, 10);
        printf("lseek 10 -> read 10:\n%s\n", str);

        err = close(fd);

        if(err)
        {
            perror("close: Error");
        }
        else
        {
//            puts("close: OK");
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
//        puts("fopen: OK");

        //puts("test.txt:");
        
        //setbuf(fp, NULL);
        setvbuf(fp, NULL, _IONBF, 0);

        errno = 0;

        fread(str, BUFFER_SIZE, 1, fp);

        if(errno)
        {
            perror("fread: Error");
        }
        else
        {
            puts("fread: OK");

            printf("%s",str);
        }

        errno = 0;
        
        int filesize = ftell(fp);

        if (filesize == -1)
        {
            perror("ftell: Error");
        }
        else
        {
//            puts("ftell: OK");
            printf("size: %d\n", filesize);
        }

        err = fseek(fp, 0, SEEK_SET);

        if (err)
        {
            perror("fseek: Error");
        }
        else
        {
//            puts("fseek: OK");
        }

        memset(str, 0, BUFFER_SIZE);
        err = fseek(fp, 10, SEEK_SET);
        if (err)
        {
            perror("fseek: Error");
        }
        errno = 0;
        fread(str, 10, 1, fp);
        //fread(str, 1, 10, fp);
        if (errno)
        {
            perror("fread: Error");
        }
        printf("fseek %ld -> fread 10:\n%s\n", ftell(fp), str);

        memset(str, 0, BUFFER_SIZE);
        err = fseek(fp, 10, SEEK_SET);
        if (err)
        {
            perror("fseek: Error");
        }
        read(fp->_file, str, 10);
        printf("fseek 10 -> read 10:\n%s\n", str);

        errno = 0;

        //puts("test.txt:");

        errno = 0;

        int count = fscanf(fp, "%s", str);

        if (errno) 
        {
            perror("fscanf: Error");
        } 
        else 
        {
//            puts("fscanf: OK");
            if (count != EOF)
            {
                printf("%s", str);
            }
        }

        err = fseek(fp, 0, SEEK_SET);

        if (err)
        {
            perror("fseek: Error");
        }
        else
        {
            //puts("fseek: OK");
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
            perror("fgetc: Error");
        }
        else
        {
//            puts("fgetc: OK");
        }

        err = fseek(fp, 0, SEEK_SET);

        if (err)
        {
            perror("fseek: Error");
        }
        else
        {
            //puts("fseek: OK");
        }

        errno = 0;

        //puts("test.txt:");

        /*Loop for reading the file till end*/
        for(int i = 0; fgets(str, filesize < BUFFER_SIZE? filesize: BUFFER_SIZE, fp); i++)
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
//            puts("fgets: OK");
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
//            puts("fclose: OK");
        }
    }

	while (1) {
		VBlankIntrWait();
	}
}


