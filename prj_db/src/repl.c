#include "repl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 
#include <unistd.h>


// function for allocation of new Inputbuffer
InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));

    if (!input_buffer) {
        printf("Erreur d'allocation de la mémoire.\n");
        exit(EXIT_FAILURE);
    }

    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

// close the InputBuffer and free memory
void close_input_buffer(InputBuffer* input_buffer) {
    if (input_buffer) {
        free(input_buffer->buffer);
        free(input_buffer);
    }
}

//display the prompt >
void print_prompt() {
    printf("mydb> ");
}

// read user input
void read_input(InputBuffer* input_buffer) {
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
    
    if (bytes_read <= 0) {
        printf("Erreur de lecture de l'entrée.\n");
        exit(EXIT_FAILURE);
    }

    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[input_buffer->input_length] = '\0';
}

void execute_statement(InputBuffer* input_buffer) {
    if (strncmp(input_buffer->buffer, "INSERT", 6) == 0) {
        printf("Commande INSERT exécutée.\n");
    }
    else if (strcmp(input_buffer->buffer, "SELECT") == 0) {
        printf("Commande SELECT exécutée.\n");
    }
    else if (strcmp(input_buffer->buffer, "exit") == 0) {
        close_input_buffer(input_buffer);
        printf("Bye!\n");
        exit(0); 
    }
    else {
        printf("Commande non reconnue. Veuillez réessayer: '%s'\n ", input_buffer->buffer);
    }
}

void start_repl() {
    InputBuffer* input_buffer = new_input_buffer();

    while(true) {
        print_prompt();
        read_input(input_buffer);

        execute_statement(input_buffer);
    }
}

