#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

Table* new_table() {
    Table* table = (Table*)malloc(sizeof(Table));
    if(!table) {
        printf("Erreur d'allocation mémoire pour la table.\n");
        exit(EXIT_FAILURE);
    }
    table->students = NULL;
    table->count = 0;
    return table; 
}

void close_table(Table* table) {
    if (table) {
        free(table->students);
        free(table);
    }
}

void insert_student(Table* table, int id, const char* name, const char* mail) {
    table->students = realloc(table->students, (table->count + 1) * sizeof(Student));
    if (!table->students) {
        printf("Erreur d'allocation de la mémoire pour la bd.\n ");
        exit(EXIT_FAILURE);
    }

    table->students[table->count].id = id;
    strncpy(table->students[table->count].name, name, sizeof(table->students[table->count].name) - 1);
    strncpy(table->students[table->count].mail, mail, sizeof(table->students[table->count].mail) - 1);

    table->students[table->count].name[sizeof(table->students[table->count].name) -1] = '\0';
    table->students[table->count].mail[sizeof(table->students[table->count].mail) -1] = '\0';

    table->count++;
    printf("Etudiant ajoutée avec succès: %d, %s, %s\n", id, name, mail);
}

void select_students(Table* table) {
    if (table->count == 0) {
        printf("Aucun étudiant trouvé\n");
        return;
    }

    printf("ID\tNom\tEmail\n");
    for (size_t i = 0; i< table->count; i++) {
        printf("%d\t%s\t%s\n", table->students[i].id, table->students[i].name, table->students[i].mail );
    }
}

