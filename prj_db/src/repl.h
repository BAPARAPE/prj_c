#ifndef REPL_H
#define REPL_H

#include <stdlib.h>
#include "db.h"  
typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

InputBuffer* new_input_buffer();
void close_input_buffer(InputBuffer* input_buffer);
void print_prompt();
void read_input(InputBuffer* input_buffer);
void execute_statement(InputBuffer* input_buffer, Table* table);
void start_repl();

#endif /* REPL_H */
