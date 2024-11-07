#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>


#define MAX_LEN_NAME 50
#define MAX_LEN_GR 20


typedef struct Student {
    unsigned int id;
    char name[MAX_LEN_NAME];
    char surname[MAX_LEN_NAME];
    char group[MAX_LEN_GR];
    unsigned char *grades;
} Student;

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
                free(*students);
                fclose(file);
                return 0;
            }
            *students = temp;
        }

        Student s;
        s.grades = malloc(5 * sizeof(unsigned char));
        if (s.grades == NULL) {
            fprintf(stderr, "Memory yps3\n");
            free(*students);
            fclose(file);
            return 0;
        }

        int result = fscanf(file, "%u %49s %49s %9s %hhu %hhu %hhu %hhu %hhu",
                            &s.id, s.name, s.surname, s.group,
                            &s.grades[0], &s.grades[1], &s.grades[2], &s.grades[3], &s.grades[4]);

        if (result == 9) {
            (*students)[(*count)++] = s;
        } else {
            free(s.grades);
            break;
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
        printf("\nChoose an option:\n");
        printf("1. Search by ID\n2. Search by Name\n3. Search by Surname\n4. Search by Group\n");
        printf("5. Sort by ID\n6. Sort by Name\n7. Sort by Surname\n8. Sort by Group\n");
        printf("9. Trace student by ID\n10. Trace above-average students\n11. Exit\n");
        scanf("%d", &choice);
        //проверка на правильность имен
        //проверка на choice

        unsigned int id;
        char buffer[50];

        switch (choice) {
            case 1:
                printf("Enter ID: ");
                scanf("%u", &id);
                search_by_id(students, count, id);
                break;
            case 2:
                printf("Enter Name: ");
                scanf("%s", buffer);
                search_by_name(students, count, buffer);
                break;
            case 3:
                printf("Enter Surname: ");
                scanf("%s", buffer);
                search_by_surname(students, count, buffer);
                break;
            case 4:
                printf("Enter Group: ");
                scanf("%s", buffer);
                search_by_group(students, count, buffer);
                break;
            case 5:
                qsort(students, count, sizeof(Student), compare_by_id);
                printf("Students sorted by ID.\n");
                break;
            case 6:
                qsort(students, count, sizeof(Student), compare_by_name);
                printf("Students sorted by Name.\n");
                break;
            case 7:
                qsort(students, count, sizeof(Student), compare_by_surname);
                printf("Students sorted by Surname.\n");
                break;
            case 8:
                qsort(students, count, sizeof(Student), compare_by_group);
                printf("Students sorted by Group.\n");
                break;
            case 9:
                printf("Enter ID for tracing: ");
                scanf("%u", &id);
                for (int i = 0; i < count; i++) {
                    if (students[i].id == id) {
                        trace_student(trace_file, &students[i]);
                        break;
                    }
                }
                break;
            case 10: {
                float avg_all = average_grade_all(students, count);
                trace_above_average(trace_file, students, count, avg_all);
                break;
            }
            case 11:
                free_students(students, count);
                return 0;
        }
    }


    return 0;
}