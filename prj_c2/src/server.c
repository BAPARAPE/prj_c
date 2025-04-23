#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <pthread.h>
#include <sqlite3.h>

void init_db(sqlite3** db)  {
    if (sqlite3_open("data.db", db) != SQLITE_OK) {
        fprintf(stderr, "Erreur ouverture DB: %s\n", sqlite3_errmsg(*db));  // corrigé: fprint -> fprintf
        exit(EXIT_FAILURE);
    }

    const char* sql_create_table = 
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL, "
        "ssh_key TEXT NOT NULL);";  // petit espace inutile retiré

    char* errmsg; 
    if (sqlite3_exec(*db, sql_create_table, 0, 0, &errmsg) != SQLITE_OK) {
        fprintf(stderr, "Erreur création table: %s\n", errmsg);  // corrigé: ‰s -> %s
        sqlite3_free(errmsg);
        exit(EXIT_FAILURE);
    }    
}

void* gerer_client(void* arg) {
    InfoClient* infoclient = (InfoClient*)arg;
    int client_socket = infoclient->socket;
    sqlite3* db = infoclient->db;
    free(infoclient);

    char buffer[BUFFER_SIZE];
    char username[100] = "";
    char ssh_key[1024] = "";

    // Lecture de LOGIN
    int bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1);
    if (bytes_read <= 0) {
        perror("Erreur lecture LOGIN");
        close(client_socket);
        pthread_exit(NULL);
    }
    buffer[bytes_read] = '\0'; 

    char commande[50], param[1024];
    if (sscanf(buffer, "%s %[^\n]", commande, param) != 2 || strcmp(commande, "LOGIN") != 0) {  // corrigé: '%s' -> "%s"
        printf("Commande LOGIN invalide.\n");
        close(client_socket);
        pthread_exit(NULL);
    }
    strncpy(username, param, sizeof(username) - 1);
    printf("Username reçu: %s\n", username); 

    memset(buffer, 0, sizeof(buffer));
    bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1);
    if (bytes_read <= 0) {
        perror("Erreur lecture KEY");
        close(client_socket);
        pthread_exit(NULL);
    }
    buffer[bytes_read] = '\0';

    if (sscanf(buffer, "%s %[^\n]", commande, param) != 2 || strcmp(commande, "KEY") != 0) {  // corrigé: '%s' -> "%s"
        printf("Commande KEY invalide.\n");
        close(client_socket);
        pthread_exit(NULL);
    }

    strncpy(ssh_key, param, sizeof(ssh_key) - 1);
    printf("Clé SSH reçue pour %s : %s\n", username, ssh_key);

    // Insertion dans la base SQLite
    char* errmsg;
    char sql_insert[1500];
    snprintf(sql_insert, sizeof(sql_insert),
             "INSERT INTO users (username, ssh_key) VALUES ('%s', '%s');",
             username, ssh_key);

    if (sqlite3_exec(db, sql_insert, 0, 0, &errmsg) != SQLITE_OK) {
        fprintf(stderr, "Erreur insertion: %s\n", errmsg);
        sqlite3_free(errmsg);
    } else {
        printf("Utilisateur %s enregistré en DB.\n", username);
    }

    fflush(stdout);
    close(client_socket);
    pthread_exit(NULL);
}
