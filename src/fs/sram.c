#include <sys/iosupport.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <gba.h>
#include <string.h>
#include <fcntl.h>

typedef struct {
    volatile unsigned char *base;
    volatile unsigned char *cur;
    u32 len;
} SramFileStruct;

static int sramInitialised = 0;

#define SRAM_SIZE 0x8000
#define SRAM_EXT_SIZE 0x10000

IWRAM_CODE
void sram_memcpy(volatile unsigned char *dst, const volatile unsigned char *src, size_t size) {
  for (;size > 0;--size) *dst++ = *src++;
}

IWRAM_CODE
signed int sram_memcmp(const volatile unsigned char *dst, const volatile unsigned char *src, size_t size) {
  for (;size > 0;--size) {

    unsigned int a = *dst++;
    unsigned int b = *src++;
    if (a < b) return -1;
    if (a > b) return 1;
  }
  return 0;
}

//---------------------------------------------------------------------------------
ssize_t sram_write(struct _reent *r,void *fileStruct ,const char *ptr,size_t len) {
//---------------------------------------------------------------------------------
    if (!sramInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    if (ptr == NULL)
    {
        r->_errno = EINVAL;
        return -1;
    }

    if (fileStruct == NULL)
    {
        r->_errno = EBADF;
        return -1;
    }

    SramFileStruct *fs = fileStruct;

    if (fs->cur == NULL)
    {
        r->_errno = EIO;
        return -1;
    }

    size_t length = fs->len - (fs->cur - fs->base);

    if (length < len)
    {
        r->_errno = ENOSPC;
        return -1;
    }

    length = len;

    sram_memcpy(fs->cur, (const volatile unsigned char *)ptr, length);

    if (sram_memcmp(fs->cur, (const volatile unsigned char *)ptr, length))
    {
        r->_errno = EIO;
        return -1;
    }

    fs->cur += length;

	return length;
}

//---------------------------------------------------------------------------------
int sram_close(struct _reent *r,void *fileStruct) {
//---------------------------------------------------------------------------------
    if (!sramInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    SramFileStruct *fs = fileStruct;
    fs->cur = fs->base;

    free(fileStruct);

	return 0;
}

//---------------------------------------------------------------------------------
ssize_t sram_read(struct _reent *r,void *fileStruct,char *ptr,size_t len) {
//---------------------------------------------------------------------------------
    if (!sramInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    if (ptr == NULL)
    {
        r->_errno = EINVAL;
        return -1;
    }

    if (fileStruct == NULL)
    {
        r->_errno = EBADF;
        return -1;
    }

    SramFileStruct *fs = fileStruct;

    if (fs->cur == NULL)
    {
        r->_errno = EIO;
        return -1;
    }

    size_t length = fs->len - (fs->cur - fs->base);

    if (length > len)
    {
        length = len;
    }

    sram_memcpy((volatile unsigned char *)ptr, fs->cur, length);

    ptr[length] = '\0';

    fs->cur += length;

	return length;
}

//---------------------------------------------------------------------------------
int sram_open(struct _reent *r, void *fileStruct, const char *path, int flags, int mode) {
//---------------------------------------------------------------------------------
    if (!sramInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    SramFileStruct *fs = fileStruct;

    fs->base = (volatile unsigned char *)SRAM;
    fs->len = SRAM_SIZE;
    fs->cur = fs->base;

    return 0;
}

//---------------------------------------------------------------------------------
int sram_ext_open(struct _reent *r, void *fileStruct, const char *path, int flags, int mode) {
//---------------------------------------------------------------------------------
    if (!sramInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    SramFileStruct *fs = fileStruct;

    fs->base = (volatile unsigned char *)SRAM;
    fs->len = SRAM_EXT_SIZE;
    fs->cur = fs->base;

    return 0;
}

off_t sram_seek(struct _reent *r, void *fileStruct, off_t pos, int dir) {
    if (!sramInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    SramFileStruct *fs = fileStruct;

    if (fs->cur == NULL || fs->base == NULL)
    {
        r->_errno = EIO;
        return -1;
    }

    switch (dir)
    {
        case SEEK_SET:
            fs->cur = fs->base;
            break;
        case SEEK_CUR:
            break;
        case SEEK_END:
            fs->cur = fs->base;
            fs->cur += fs->len;
            break;
        default:
            r->_errno = EINVAL;
            return -1;
    }

    fs->cur += pos;

    off_t off = fs->cur - fs->base;

    if (off < 0 || off > fs->len)
    {
        r->_errno = EINVAL;
        return -1;
    }

    return off;
}

int sram_fstat(struct _reent *r, void *fileStruct, struct stat *st) {
    if (!sramInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    st->st_mode = S_IFREG;

    SramFileStruct *fs = fileStruct;
    st->st_size = fs->len;

    return 0;
}

int sram_stat(struct _reent *r, const char *file, struct stat *st) {
    if (!sramInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    st->st_mode = S_IFREG;
    return 0;
}

const devoptab_t dotab_sram = {
	"sram",
	sizeof(SramFileStruct),
	sram_open,
	sram_close,
	sram_write,
	sram_read,
	sram_seek,
    sram_fstat,
    sram_stat,
	NULL
};

const devoptab_t dotab_sram_64KB = {
	"sram_64KB",
	sizeof(SramFileStruct),
	sram_ext_open,
	sram_close,
	sram_write,
	sram_read,
	sram_seek,
    sram_fstat,
    sram_stat,
	NULL
};

const devoptab_t dotab_sram_512Kb = {
	"sram_512Kb",
	sizeof(SramFileStruct),
	sram_ext_open,
	sram_close,
	sram_write,
	sram_read,
	sram_seek,
    sram_fstat,
    sram_stat,
	NULL
};

void sramInit()
{
    if (!sramInitialised)
    {
        sramInitialised = 1;
        AddDevice(&dotab_sram);
        AddDevice(&dotab_sram_64KB);
        AddDevice(&dotab_sram_512Kb);
    }
}
