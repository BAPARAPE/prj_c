#ifndef DB_H
#define DB_H

#include <stdlib.h>

typedef struct {
    int id;
    char name[100];
    char mail[100];
} Student;

typedef struct {
    Student* students;
    size_t count;
} Table;

Table* new_table();
void free_table(Table* table);
void insert_student(Table* table, int id, const char* name, const char* mail);
void select_students(Table* table);

#endif /* DB_H */
