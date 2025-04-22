#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>

#define PORT 3155
#define BUFFER_SIZE 2048

void enregistrer_dans_db(const char* username, const char* ssh_key) {
    FILE* db = fopen("db.txt", "a");
    if (db == NULL) {
        perror("Erreur ouverture DB");
        return;
    }
    fprintf(db, "Utilisateur: %s\nClé SSH: %s\n---\n", username, ssh_key);
    fclose(db);
}

struct InfoClient {
    int socket; 
}; 

void* gerer_client(void* arg) {
    struct InfoClient* info_client = (struct InfoClient*) arg;
    int client_socket = info_client->socket;
    free(info_client);

    char buffer[BUFFER_SIZE];
    char username[100];
    char ssh_key[1024];

    username[0] = '\0';
    ssh_key[0] = '\0';

    // Lire REGISTER
    int bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1);
    if (bytes_read <= 0) {
        perror("Erreur lecture du REGISTER");
        close(client_socket);
        pthread_exit(NULL);
    }

    buffer[bytes_read] = '\0';

    if (strncmp(buffer, "REGISTER ", 9) == 0) {
        strncpy(username, buffer + 9, sizeof(username) - 1);
        username[strcspn(username, "\r\n")] = '\0';
        printf("Username validé: %s\n", username);
    } else {
        printf("Commande invalide (attendu REGISTER)\n");
        close(client_socket);
        pthread_exit(NULL);
    }

    // Lire SEND_KEY
    memset(buffer, 0, sizeof(buffer)); // nettoyage du buffer
    bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1);
    if (bytes_read <= 0) {
        perror("Erreur lecture du SEND_KEY");
        close(client_socket);
        pthread_exit(NULL);
    }

    buffer[bytes_read] = '\0';

    if (strncmp(buffer, "SEND_KEY ", 9) == 0) {
        strncpy(ssh_key, buffer + 9, sizeof(ssh_key) - 1);
        ssh_key[strcspn(ssh_key, "\r\n")] = '\0';
        printf("Clé SSH reçue pour %s :\n%s\n", username, ssh_key);
    } else {
        printf("Commande invalide (attendu SEND_KEY)\n");
    }

    fflush(stdout); // Force l'affichage immédiat
    close(client_socket);
    pthread_exit(NULL);
}

int main() {
    int server_socket; 
    struct sockaddr_in server_addr, client_addr; 
    socklen_t client_len = sizeof(client_addr);

    // Création socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Erreur création socket");
        exit(EXIT_FAILURE);
    }

    // Liaison du socket à une adresse
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

    printf("Serveur en écoute sur le port %d...\n", PORT);

    while(1) {
        int client_socket = accept(server_socket, (struct sockaddr*) &client_addr, &client_len);
        if(client_socket < 0) {
            perror("Erreur accept");
            continue;
        }

        printf("Nouveau client connecté.\n");

        struct InfoClient* data = malloc(sizeof(struct InfoClient));
        data->socket = client_socket;

        pthread_t thread_id; 
        int result = pthread_create(&thread_id, NULL, gerer_client, data);
        if (result != 0) {
            perror("Erreur pthread_create");
            close(client_socket);
            free(data);
        } else {
            pthread_detach(thread_id);
        }
    }

    close(server_socket);
    return 0;
}
