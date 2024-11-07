#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_LEN_NAME 80
#define MAX_LEN_GR 10
#define NUM_EXAMS 5


typedef struct Student {
    unsigned int id;
    char name[MAX_LEN_NAME];
    char surname[MAX_LEN_NAME];
    char group[MAX_LEN_GR];
    unsigned char *grades;
} Student;

int isValidName(const char* name) {
    if (strlen(name) > 70) {
        return 0;
    }
    if (!isupper(name[0])) {
        return 0;
    }
    for (int i = 1; name[i] != '\0'; ++i) {
        if (!islower(name[i])) {
            return 0;
        }
    }
    return 1;
}

void free_students(Student *students, int count) {
    for (int i = 0; i < count; i++) {
        free(students[i].grades);
    }
    free(students);
}

void print_student(const Student *student) {
    printf("ID: %u, Name: %s, Surname: %s, Group: %s, Grades:", student->id, student->name, student->surname, student->group);
    for (int i = 0; i < NUM_EXAMS; i++) {
        printf(" %u", student->grades[i]);
    }
    printf("\n");
}

int read_students(const char *file_in, Student **students, int *count) {
    FILE *file = fopen(file_in, "r");
    if (!file) {
        fprintf(stderr, "We can't open such a file\n");
        return 0;
    }

    *count = 0;
    int capacity = 10;
    *students = malloc(capacity * sizeof(Student));
    if (*students == NULL) {
        fprintf(stderr, "Memory yps1\n");
        fclose(file);
        return 0;
    }

    while (1) {
        if (*count >= capacity) {
            capacity *= 2;
            Student *temp = realloc(*students, capacity * sizeof(Student));
            if (temp == NULL) {
                fprintf(stderr, "Memory yps2\n");
                free_students(*students, *count);
                fclose(file);
                return 0;
            }
            *students = temp;
        }

        Student s;
        s.grades = malloc(5 * sizeof(unsigned char));
        if (s.grades == NULL) {
            fprintf(stderr, "Memory yps3\n");
            free_students(*students, *count);
            fclose(file);
            return 0;
        }

        int result = fscanf(file, "%u %49s %49s %9s %hhu %hhu %hhu %hhu %hhu",
                            &s.id, s.name, s.surname, s.group,
                            &s.grades[0], &s.grades[1], &s.grades[2], &s.grades[3], &s.grades[4]);

        if (result == 9) {
            if (!isValidName(s.name) || !isValidName(s.surname) || strlen(s.group) == 0 || strlen(s.group) > 9) {
                fprintf(stderr, "Ошибка в данных (имя, фамилия или группа). Пропуск записи.\n");
                free(s.grades);
                continue;
            }

            int valid_grades = 1;
            for (int i = 0; i < NUM_EXAMS; i++) {
                if (s.grades[i] > 100) {
                    valid_grades = 0;
                    break;
                }
            }

            if (!valid_grades) {
                fprintf(stderr, "Ошибка в данных (оценки вне допустимого диапазона). Пропуск записи.\n");
                free(s.grades);
                continue;
            }
            (*students)[(*count)++] = s;
            print_student(&s);
        } else {
            if (feof(file)) {
                break;
            }
            fprintf(stderr, "Ошибка чтения данных (получено: %d из 9). Пропуск записи.\n", result);
            free(s.grades);
            int ch;
            while ((ch = fgetc(file)) != EOF && (ch != '\n')) {
            }
            continue;
        }
    }

    fclose(file);
    return 1;
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Not enough arg: %s <student_file> <trace_file>\n", argv[0]);
        return 1;
    }

    char stud_path[PATH_MAX];
    char trace_path[PATH_MAX];

    if (realpath(argv[1], stud_path) == NULL) {
        fprintf(stderr, "Something wrong with input\n");
        return 1;
    }

    if (realpath(argv[2], trace_path) == NULL) {
        fprintf(stderr, "Something wrong with output\n");
        return 1;
    }

    if (strcmp(stud_path, trace_path) == 0) {
        fprintf(stderr, "Name of the file can't be same\n");
        return 1;
    }

    const char *stud_file = argv[1];
    const char *trace_file = argv[2];
    Student *students = NULL;
    int count;

    if (read_students(stud_file, &students, &count) == 0) {
        return 1;
    }

    int choice;
    while (1) {
        printf("\n");
        printf("1. Search by ID\n2. Search by Name\n3. Search by Surname\n4. Search by Group\n");
        printf("5. Sort by ID\n6. Sort by Name\n7. Sort by Surname\n8. Sort by Group\n");
        printf("9. Trace student by ID\n10. Trace above-average students\n11. Exit\n");
        printf("\nChoose an option: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid command\n");
            while (getchar() != '\n');
            continue;
        } else {
            char extra;
            if (scanf("%c", &extra) != 1 || extra != '\n') {
                printf("Invalid command2\n");
                while (getchar() != '\n');
                continue;
            }
        }

        if (choice > 11 || choice < 1){
            printf("Invalid command\n");
            continue;
        }
        //считываение из файла
        //написать, что никого не нашел
        //куда сортировка выводится

        unsigned int id;
        char buffer[50];

        switch (choice) {
            
            
            case 11:
                free_students(students, count);
                return 0;
        }
    }


    return 0;
}