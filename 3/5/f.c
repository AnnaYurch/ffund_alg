#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>


#define MAX_LEN_NAME 50
#define MAX_LEN_GR 20
#define NUM_EXAMS 5


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

void free_students(Student *students, int count) {
    for (int i = 0; i < count; i++) {
        free(students[i].grades);
    }
    free(students);
}

int compare_by_id(const void *a, const void *b) {
    return ((Student *)a)->id - ((Student *)b)->id;
}

int compare_by_name(const void *a, const void *b) {
    return strcmp(((Student *)a)->name, ((Student *)b)->name);
}

int compare_by_surname(const void *a, const void *b) {
    return strcmp(((Student *)a)->surname, ((Student *)b)->surname);
}

int compare_by_group(const void *a, const void *b) {
    return strcmp(((Student *)a)->group, ((Student *)b)->group);
}

void print_student(const Student *student) {
    printf("ID: %u, Name: %s, Surname: %s, Group: %s, Grades:", student->id, student->name, student->surname, student->group);
    for (int i = 0; i < NUM_EXAMS; i++) {
        printf(" %u", student->grades[i]);
    }
    printf("\n");
}

void search_by_id(Student *students, int count, unsigned int id) {
    for (int i = 0; i < count; i++) {
        if (students[i].id == id) {
            print_student(&students[i]);
            return;
        }
    }
    printf("Student not found.\n");
}

void search_by_name(Student *students, int count, const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].name, name) == 0) {
            print_student(&students[i]);
        }
    }
}

void search_by_surname(Student *students, int count, const char *surname) {
    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].surname, surname) == 0) {
            print_student(&students[i]);
        }
    }
}

void search_by_group(Student *students, int count, const char *group) {
    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].group, group) == 0) {
            print_student(&students[i]);
        }
    }
}

float average_grade(const Student *student) {
    int sum = 0;
    for (int i = 0; i < NUM_EXAMS; i++) {
        sum += student->grades[i];
    }
    return (float)sum / NUM_EXAMS;
}

float average_grade_all(Student *students, int count) {
    float total_sum = 0;
    for (int i = 0; i < count; i++) {
        total_sum += average_grade(&students[i]);
    }
    return total_sum / count;
}

void trace_student(const char *trace_filename, const Student *student) {
    FILE *file = fopen(trace_filename, "a");
    if (file) {
        fprintf(file, "Student: %s %s, Group: %s, Average Grade: %.2f\n", student->name, student->surname, student->group, average_grade(student));
        fclose(file);
    } else {
        perror("Unable to open trace file");
    }
}

void trace_above_average(const char *trace_filename, Student *students, int count, float avg) {
    FILE *file = fopen(trace_filename, "a");
    if (file) {
        for (int i = 0; i < count; i++) {
            if (average_grade(&students[i]) > avg) {
                fprintf(file, "Above Average: %s %s\n", students[i].name, students[i].surname);
            }
        }
        fclose(file);
    } else {
        perror("Unable to open trace file");
    }
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
        printf("\nChoose an option:\n");
        scanf("%d", &choice);
        //проверка на правильность имен
        //проверка на choice
        //написать, что никого не нашел
        //куда сортировка выводитсяч

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