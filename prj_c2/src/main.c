#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "server.h"

#define PORT 3155

int main() {
    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    sqlite3* db;

    init_db(&db);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Erreur création socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("Erreur listen");
        exit(EXIT_FAILURE);
    }

    printf("Serveur en écoute sur le port %d..\n", PORT);

    while (1) {
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Erreur accept");
            continue;
        }

        printf("Nouveau client connecté.\n");

        InfoClient* data = malloc(sizeof(InfoClient));
        data->socket = client_socket;
        data->db = db;

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, gerer_client, data) != 0) {
            perror("Erreur pthread_create");
            close(client_socket);
            free(data);
        } else {
            pthread_detach(thread_id);
        }
    }

    close(server_socket);
    sqlite3_close(db);
    return 0;
}
