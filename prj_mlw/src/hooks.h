#ifndef HOOKS_H
#define HOOKS_H

#include <openssl/ssl.h>

int SSL_read(SSL *ssl, void *buf, int num);
int SSL_write(SSL *ssl, const void *buf, int num);

#endif 