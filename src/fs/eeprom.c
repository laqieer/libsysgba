#include <sys/iosupport.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <tonc.h>
#include <string.h>
#include <fcntl.h>

typedef struct {
    unsigned char *base;
    unsigned char *cur;
    u32 len;
} eepromFileStruct;

static int eepromInitialised = 0;

#define EEPROM (unsigned char *)0x0D000000
#define EEPROM_SIZE 0x200

void eeprom_memcpy(void *dst, const void *src, size_t size)
{
    u16 tmp = REG_IME;
    REG_IME = 0;
    REG_WAITCNT = (REG_WAITCNT & (~0x700)) | WS_ROM2_N8;
    DMA_TRANSFER(dst, src, size, 3, DMA_ENABLE);
    while(REG_DMA3CNT_H & 0x8000);
    REG_IME = tmp;
}

int _eeprom_read(struct _reent *r, u32 addr, u8 *data)
{
    u16 buffer[68];
    u16 d[4];

    if (addr > EEPROM_SIZE / 8 - 1)
    {
        r->_errno = EINVAL;
        return -1;
    }

    buffer[0] = 1;
    buffer[1] = 1;
    buffer[8] = 0;
    for (int i = 7; i >= 2; i--)
    {
        buffer[i] = addr & 1;
        addr = addr >> 1;
    }
    eeprom_memcpy(EEPROM, buffer, 9);

    eeprom_memcpy(buffer, EEPROM, 68);

    for (int i = 3; i >= 0; i--)
    {
        d[i] = 0;
        for (int j = 0; j < 16; j++)
        {
            d[i] += buffer[4 + 16 * (3 - i) + j] << (15 - j);
        }
    }

    data[0] = d[0] & 0xff;
    data[1] = d[0] >> 8;
    data[2] = d[1] & 0xff;
    data[3] = d[1] >> 8;
    data[4] = d[2] & 0xff;
    data[5] = d[2] >> 8;
    data[6] = d[3] & 0xff;
    data[7] = d[3] >> 8;

    return 0;
}

int _eeprom_write(struct _reent *r, u32 addr, u8 *data)
{
    u16 buffer[73];
    u16 d[4];

    if (addr > EEPROM_SIZE / 8 - 1)
    {
        r->_errno = EINVAL;
        return -1;
    }

    d[0] = data[0] + (data[1] << 8);
    d[1] = data[2] + (data[3] << 8);
    d[2] = data[4] + (data[5] << 8);
    d[3] = data[6] + (data[7] << 8);

    buffer[0] = 1;
    buffer[1] = 0;
    buffer[72] = 0;
    for (int i = 7; i >= 2; i--)
    {
        buffer[i] = addr & 1;
        addr = addr >> 1;
    }
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            buffer[71 - 16 * i - j] = (d[i] >> j) & 1;
        }
    }
    eeprom_memcpy(EEPROM, buffer, 73);

    while(!((*EEPROM) & 1));

    return 0;
}


signed int eeprom_memcmp(const unsigned char *dst, const unsigned char *src, size_t size)
{
    return 0;
}

//---------------------------------------------------------------------------------
ssize_t eeprom_write(struct _reent *r,void *fileStruct ,const char *ptr,size_t len) {
//---------------------------------------------------------------------------------
    u8 buffer[8];
    int err;

    if (!eepromInitialised)
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

    eepromFileStruct *fs = fileStruct;

    if (fs->cur == NULL)
    {
        r->_errno = EIO;
        return -1;
    }

    u32 cur = fs->cur - fs->base;

    size_t length = fs->len - cur;

    if (length < len)
    {
        r->_errno = ENOSPC;
        return -1;
    }

    length = len;

    if (cur % 8 == 0 && length == 8)
    {
        err = _eeprom_write(r, cur / 8, (u16*)ptr);
        if (err)
        {
            return -1;
        }

        goto written;
    }

    if (cur / 8 == (cur + length) / 8)
    {
        err = _eeprom_read(r, cur / 8, (u16*)buffer);
        if (err)
        {
            return -1;
        }

        memcpy(buffer + cur % 8, ptr, length);

        err = _eeprom_write(r, cur / 8, (u16*)buffer);
        if (err)
        {
            return -1;
        }

        goto written;
    }

    if (cur % 8)
    {
        err = _eeprom_read(r, cur / 8, (u16*)buffer);
        if (err)
        {
            return -1;
        }

        memcpy(buffer + cur % 8, ptr, 8 - cur % 8);

        err = _eeprom_write(r, cur / 8, (u16*)buffer);
        if (err)
        {
            return -1;
        }
    }

    if ((cur + length) % 8)
    {
        err = _eeprom_read(r, (cur + length) / 8, (u16*)buffer);
        if (err)
        {
            return -1;
        }

        memcpy(buffer, ptr + length - (cur + length) % 8, (cur + length) % 8);

        err = _eeprom_write(r, (cur + length) / 8, (u16*)buffer);
        if (err)
        {
            return -1;
        }
    }

    int start = cur / 8;
    if (cur % 8)
    {
        start++;
    }
    int end = (cur + length) / 8 - 1;

    if (start > end)
    {
        goto written;
    }

    for (int i = start; i <= end; i++)
    {
        if (cur % 8)
        {
            err = _eeprom_write(r, i, (u16*)(ptr + 8 - cur % 8 + 8 * (i - start)));
        }
        else
        {
            err = _eeprom_write(r, i, (u16*)(ptr  + 8 * (i - start)));
        }
        if (err)
        {
            return -1;
        }
    }

written:
    fs->cur += length;

	return length;
}

//---------------------------------------------------------------------------------
int eeprom_close(struct _reent *r,void *fileStruct) {
//---------------------------------------------------------------------------------
    if (!eepromInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    eepromFileStruct *fs = fileStruct;
    fs->cur = fs->base;

    free(fileStruct);

	return 0;
}

//---------------------------------------------------------------------------------
ssize_t eeprom_read(struct _reent *r,void *fileStruct,char *ptr,size_t len) {
//---------------------------------------------------------------------------------
    u8 buffer[8];
    int err;

    if (!eepromInitialised)
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

    eepromFileStruct *fs = fileStruct;

    if (fs->cur == NULL)
    {
        r->_errno = EIO;
        return -1;
    }

    u32 cur = fs->cur - fs->base;

    size_t length = fs->len - cur;

    if (length > len)
    {
        length = len;
    }

    if (cur % 8 == 0 && length == 8)
    {
        err = _eeprom_read(r, cur / 8, (u16*)ptr);
        if (err)
        {
            return -1;
        }

        goto readed;
    }

    if (cur / 8 == (cur + length) / 8)
    {
        err = _eeprom_read(r, cur / 8, (u16*)buffer);
        if (err)
        {
            return -1;
        }

        memcpy(ptr, buffer, length);

        goto readed;
    }

    if (cur % 8)
    {
        err = _eeprom_read(r, cur / 8, (u16*)buffer);
        if (err)
        {
            return -1;
        }

        memcpy(ptr, buffer + cur % 8, 8 - cur % 8);
    }

    if ((cur + length) % 8)
    {
        err = _eeprom_read(r, (cur + length) / 8, (u16*)buffer);
        if (err)
        {
            return -1;
        }

        memcpy(ptr + length - (cur + length) % 8, buffer, (cur + length) % 8);
    }

    int start = cur / 8;
    if (cur % 8)
    {
        start++;
    }
    int end = (cur + length) / 8 - 1;

    if (start > end)
    {
        goto readed;
    }

    for (int i = start; i <= end; i++)
    {
        if (cur % 8)
        {
            err = _eeprom_read(r, i, (u16*)(ptr + 8 - cur % 8 + 8 * (i - start)));
        }
        else
        {
            err = _eeprom_read(r, i, (u16*)(ptr + 8 * (i - start)));
        }
        if (err)
        {
            return -1;
        }
    }

readed:
    ptr[length] = '\0';

    fs->cur += length;

	return length;
}

//---------------------------------------------------------------------------------
int eeprom_open(struct _reent *r, void *fileStruct, const char *path, int flags, int mode) {
//---------------------------------------------------------------------------------
    if (!eepromInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    eepromFileStruct *fs = fileStruct;

    fs->base = (unsigned char *)EEPROM;
    fs->len = EEPROM_SIZE;
    fs->cur = fs->base;

    return 0;
}

off_t eeprom_seek(struct _reent *r, void *fileStruct, off_t pos, int dir) {
    if (!eepromInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    eepromFileStruct *fs = fileStruct;

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

int eeprom_fstat(struct _reent *r, void *fileStruct, struct stat *st) {
    if (!eepromInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    st->st_mode = S_IFREG;

    eepromFileStruct *fs = fileStruct;
    st->st_size = fs->len;

    return 0;
}

int eeprom_stat(struct _reent *r, const char *file, struct stat *st) {
    if (!eepromInitialised)
    {
        r->_errno = ENODEV;
        return -1;
    }

    st->st_mode = S_IFREG;
    return 0;
}

const devoptab_t dotab_eeprom = {
	"eeprom",
	sizeof(eepromFileStruct),
	eeprom_open,
	eeprom_close,
	eeprom_write,
	eeprom_read,
	eeprom_seek,
    eeprom_fstat,
    eeprom_stat,
	NULL
};

void eepromInit()
{
    if (!eepromInitialised)
    {
        eepromInitialised = 1;
        AddDevice(&dotab_eeprom);
    }
}
