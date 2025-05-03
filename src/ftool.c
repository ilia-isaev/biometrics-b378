#include "ftool.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

rbuf_s
rd_file(const char* pathname)
{
    struct stat st;
    if (stat(pathname, &st) != 0)
        return (rbuf_s){.err=errno};

    rbuf_s rbuf = {.buf.ptr=malloc((st.st_size)*sizeof(char)), .buf.sz=st.st_size};
    if (NULL == rbuf.buf.ptr)
        return (rbuf_s){.err=errno};

    FILE *stream = fopen(pathname, "rb");
    if (stream == NULL)
    {
        goto err;
    }

    size_t sz_to_read = rbuf.buf.sz;
    char  *ptr        = rbuf.buf.ptr;
    do
    {
        const size_t n = fread(ptr, sizeof(char), sz_to_read, stream);

        if (n < 0 && errno != EAGAIN)
            goto err;

        sz_to_read -= n;
        ptr        += n;
    }
    while (sz_to_read > 0);

    if (0 != fclose(stream))
        goto err;

    return rbuf;
err:
    free(rbuf.buf.ptr);
    return (rbuf_s){.err=errno};
}

static int
wr_buf(FILE *stream, buf_s buf)
{
    size_t sz_to_wr = buf.sz;
    char  *ptr      = buf.ptr;

    do
    {
        const size_t n = fwrite(ptr, sizeof(char), sz_to_wr, stream);

        if (n < 0)
            return errno;

        sz_to_wr -= n;
        ptr      += n;
    }
    while (sz_to_wr > 0);

    return 0;
}

int
wr_file(const char* pathname, buf_s buf)
{
    FILE *stream = fopen(pathname, "wb");
    if (stream == NULL)
    {
        return errno;
    }

    int err = wr_buf(stream, buf);
    if (err)
    {
        return err;
    }

    if (0 != fclose(stream))
        return errno;

    return 0;
}

int
wr_file2(const char* pathname, buf_s head, buf_s buf)
{
    FILE *stream = fopen(pathname, "wb");
    if (stream == NULL)
    {
        return errno;
    }

    if (head.sz > 0)
    {
        int err = wr_buf(stream, head);
        if (err)
        {
            return err;
        }
    }

    int err = wr_buf(stream, buf);
    if (err)
    {
        return err;
    }

    if (0 != fclose(stream))
        return errno;

    return 0;
}

