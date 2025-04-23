#ifndef SERVER_H 
#define SERVER_H

#include <sqlite3.h>

#define BUFFER_SIZE 2048 

typedef struct InfoClient {
    int socket;
    sqlite3* db;
} InfoClient; 

void* gerer_client(void* arg);
void init_db(sqlite3** db);

#endif
