#include <sys/iosupport.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <gba.h>
#include <string.h>
#include <fcntl.h>

#include "libgbfs/gbfs.h"

#define GBFS_FILENAME_MAX_LENGTH 255

typedef struct {
    const char *name;
    const void *obj;
    const char *cur;
    const GBFS_FILE *file;
    u32 len;
} GbfsFileStruct;

static int gbfsInitialised = 0;

//---------------------------------------------------------------------------------
ssize_t gbfs_write(struct _reent *r,void *fileStruct ,const char *ptr,size_t len) {
//---------------------------------------------------------------------------------
    if (!gbfsInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    r->_errno = EROFS;
	return -1;
}

//---------------------------------------------------------------------------------
int gbfs_close(struct _reent *r,void *fileStruct) {
//---------------------------------------------------------------------------------
    if (!gbfsInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    GbfsFileStruct *fs = fileStruct;
    fs->cur = fs->obj;

    free(fileStruct);

	return 0;
}

//---------------------------------------------------------------------------------
ssize_t gbfs_read(struct _reent *r,void *fileStruct,char *ptr,size_t len) {
//---------------------------------------------------------------------------------
    if (!gbfsInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    GbfsFileStruct *fs = fileStruct;

    if (fs->cur == NULL)
    {
        r->_errno = EIO;
        return -1;
    }

    if (ptr == NULL)
    {
        r->_errno = EINVAL;
        return -1;
    }

    size_t length = fs->len - (fs->cur - (char *)fs->obj);

    if (length > len)
    {
        length = len;
    }

    memcpy(ptr, fs->cur, length);

    fs->cur += length;

	return 0;
}

//---------------------------------------------------------------------------------
int gbfs_open(struct _reent *r, void *fileStruct, const char *path, int flags, int mode) {
//---------------------------------------------------------------------------------
    if (!gbfsInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    if (path == NULL || strlen(path) == 0)
    {
        r->_errno = EINVAL;
        return -1;
    }

    if (strlen(path) > GBFS_FILENAME_MAX_LENGTH)
    {
        r->_errno = ENAMETOOLONG;
        return -1;
    }

    if ((flags & O_WRONLY) || (flags & O_RDWR))
    {
        r->_errno = EROFS;
        return -1;
    }

    GbfsFileStruct *fs = fileStruct;

    char *separator = strchr(path, ':');

    if (separator)
    {
        fs->name = separator + 1;
    }
    else
    {
        fs->name = path;
    }

    fs->file = find_first_gbfs_file(find_first_gbfs_file);

    while(1) {
        fs->obj = gbfs_get_obj(fs->file, fs->name, &fs->len);

        if(fs->obj != NULL)
        {
            fs->cur = fs->obj;
            return 0;
        }
        
        fs->file = find_first_gbfs_file(skip_gbfs_file(fs->file));

        if(fs->file == NULL)
        {
            r->_errno = ENOENT;
            return -1;
        }
    }
}

off_t gbfs_seek(struct _reent *r, void *fileStruct, off_t pos, int dir) {
    if (!gbfsInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    GbfsFileStruct *fs = fileStruct;

    if (fs->cur == NULL || fs->obj == NULL)
    {
        r->_errno = EIO;
        return -1;
    }

    switch (dir)
    {
        case SEEK_SET:
            fs->cur = fs->obj;
            break;
        case SEEK_CUR:
            break;
        case SEEK_END:
            fs->cur = fs->obj;
            fs->cur += fs->len;
            break;
        default:
            r->_errno = EINVAL;
            return -1;
    }

    fs->cur += pos;

    off_t off = fs->cur - (const char *)fs->obj;

    if (off < 0 || off > fs->len)
    {
        r->_errno = EINVAL;
        return -1;
    }

    return off;
}

int gbfs_fstat(struct _reent *r, void *fileStruct, struct stat *st) {
    if (!gbfsInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    st->st_mode = S_IFREG;
    //st->st_mode = S_IFCHR;

    GbfsFileStruct *fs = fileStruct;
    st->st_size = fs->len;

    return 0;
}

int gbfs_stat(struct _reent *r, const char *file, struct stat *st) {
    if (!gbfsInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    if (file == NULL || strlen(file) == 0)
    {
        r->_errno = EINVAL;
        return -1;
    }

    if (strlen(file) > GBFS_FILENAME_MAX_LENGTH)
    {
        r->_errno = ENAMETOOLONG;
        return -1;
    }

    st->st_mode = S_IFREG;
    //st->st_mode = S_IFCHR;
    return 0;
}

const devoptab_t dotab_gbfs = {
	"gbfs",
	sizeof(GbfsFileStruct),
	gbfs_open,
	gbfs_close,
	gbfs_write,
	gbfs_read,
	gbfs_seek,
    gbfs_fstat,
    gbfs_stat,
	NULL
};

void gbfsInit()
{
    if (!gbfsInitialised)
    {
        gbfsInitialised = 1;
        setDefaultDevice(AddDevice(&dotab_gbfs));
    }
}
