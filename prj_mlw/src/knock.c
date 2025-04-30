#define _GNU_SOURCE
#include "knock.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

static const int PORT_SEQUENCE[] = {4444, 5555, 6666};
static const int SEQ_LENGTH = sizeof(PORT_SEQUENCE) / sizeof(PORT_SEQUENCE[0]);

// Vérification si un paquet TCP est un SYN sans ACK
static bool is_syn_without_ack(const struct tcphdr *tcp) {
    return tcp->syn == 1 && tcp->ack == 0;
}

static int get_tcp_dest_port(const struct tcphdr *tcp) {
    return ntohs(tcp->dest);
}

//écoute du port knocking
void pk_init(void) {
    int raw_sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (raw_sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int current_step = 0;
    unsigned char buffer[4096];

    printf("[*] Port knocking listener started...\n");

    while (current_step < SEQ_LENGTH) {
        ssize_t bytes = recv(raw_sock, buffer, sizeof(buffer), 0);
        if (bytes <= 0)
            continue;

        struct iphdr *ip_header = (struct iphdr *)buffer;
        if (ip_header->protocol != IPPROTO_TCP)
            continue;

        struct tcphdr *tcp_header = (struct tcphdr *)(buffer + (ip_header->ihl * 4));
        if (!is_syn_without_ack(tcp_header))
            continue;

        int port = get_tcp_dest_port(tcp_header);
        printf("[*] Detected SYN on port %d\n", port);

        if (port == PORT_SEQUENCE[current_step]) {
            current_step++;
        } else {
            current_step = 0; 
        }
    }

    printf("[*] Port knocking sequence completed!\n");
    close(raw_sock);
}
