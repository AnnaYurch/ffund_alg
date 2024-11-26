#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_LEN_NAME 80
#define MAX_LEN_GR 10
#define NUM_EXAMS 5

//большие слова не обрабатывает
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

int valid_num(const char* id) {
    for (int i = 0; id[i] != '\0'; ++i) {
        if (id[i] < '0' || id[i] > '9') {
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

        char *name;
        char n_id[10];
        char n_g0[4];
        char n_g1[4];
        char n_g2[4];
        char n_g3[4];
        char n_g4[4];
        int result = fscanf(file, "%9s %79s %79s %9s %3s %3s %3s %3s %3s", 
                            n_id, s.name, s.surname, s.group, n_g0, n_g1, 
                            n_g2, n_g3, n_g4);

        if (result == 9) {
            if (valid_num(n_id) == 0 || valid_num(n_g0) == 0 || valid_num(n_g1) == 0 ||
                valid_num(n_g2) == 0 || valid_num(n_g3) == 0 || valid_num(n_g4) == 0) {
                fprintf(stderr, "Ошибка в данных (id или в оценках). Пропуск записи.\n");
                free(s.grades);
                continue;
            }
            s.id = atoi(n_id);
            s.grades[0] = atoi(n_g0);
            s.grades[1] = atoi(n_g1);
            s.grades[2] = atoi(n_g2);
            s.grades[3] = atoi(n_g3);
            s.grades[4] = atoi(n_g4);

            if (!isValidName(s.name) || !isValidName(s.surname) || strlen(s.group) == 0 || strlen(s.group) > 9) {
                fprintf(stderr, "Ошибка в данных (имя, фамилия или группа). Пропуск записи.\n");
                free(s.grades);
                continue;
            }

            int flag = 1;
            for (int i = 0; i < *count; i++) {
                if ((*students)[i].id == s.id) {
                    flag = 0;
                    //printf("Такой id (%u) студента уже есть\n", s.id);
                    //free(s.grades);
                    break;
                }
            }

            for (int i = 0; i < 5; i++) {
                if (s.grades[i] > 100 || s.grades[i] < 0) {
                    flag = 0;
                    //printf("Неправильные данные у %u\n", s.id);
                    //free(s.grades);
                    break;
                }
            }

            if (flag == 1) {
                (*students)[(*count)++] = s;
                print_student(&s);
            } else {
                printf("Неправильные данные у %u (либо id совпадают, либо оценка неправильная)\n", s.id);
                free(s.grades);
            }
        } else {
            fprintf(stderr, "Ошибка чтения данных (получено: %d из 9). Пропуск записи.\n", result);
            free(s.grades);
            int ch;
            while ((ch = fgetc(file)) != EOF && (ch != '\n')) {
            }
        }
        if (feof(file)) {
            break;
        }
    }

    fclose(file);
    return 1;
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
            return;
        }
    }
    printf("Student not found.\n");
}

void search_by_surname(Student *students, int count, const char *surname) {
    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].surname, surname) == 0) {
            print_student(&students[i]);
            return;
        }
    }
    printf("Student not found.\n");
}

void search_by_group(Student *students, int count, const char *group) {
    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].group, group) == 0) {
            print_student(&students[i]);
            return;
        }
    }
    printf("Students not found.\n");
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
        fprintf(file, "Above Average (%f): \n", avg);
        for (int i = 0; i < count; i++) {
            if (average_grade(&students[i]) > avg) {   
                fprintf(file, "Student: %s %s, Group: %s, Average Grade: %.2f\n", (&students[i])->name, (&students[i])->surname, (&students[i])->group, average_grade((&students[i])));
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

        unsigned int id;
        char buffer[50];

        switch (choice) {
            case 1:
                printf("Enter ID (it's a number): ");
                scanf("%u", &id);
                search_by_id(students, count, id);
                break;
            case 2:
                printf("Enter Name (without unreal symbols): ");
                scanf("%s", buffer);
                search_by_name(students, count, buffer);
                break;
            case 3:
                printf("Enter Surname (without unreal symbols): ");
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
                for (int i = 0; i < count; i++) {
                    print_student(students + i);
                }
                printf("Students sorted by ID.\n");
                break;
            case 6:
                qsort(students, count, sizeof(Student), compare_by_name);
                for (int i = 0; i < count; i++) {
                    print_student(&students[i]);
                }
                printf("Students sorted by Name.\n");
                break;
            case 7:
                qsort(students, count, sizeof(Student), compare_by_surname);
                for (int i = 0; i < count; i++) {
                    print_student(students + i);
                }
                printf("Students sorted by Surname.\n");
                break;
            case 8:
                qsort(students, count, sizeof(Student), compare_by_group);
                for (int i = 0; i < count; i++) {
                    print_student(students + i);
                }
                printf("Students sorted by Group.\n");
                break;
            case 9:
                printf("Enter ID for tracing: ");
                scanf("%u", &id);
                int flag = 0;
                for (int i = 0; i < count; i++) {
                    if (students[i].id == id) {
                        trace_student(trace_file, &students[i]);
                        flag = 1;
                        printf("Студент выведен\n");
                        break;
                    }
                }
                if (flag == 0) {
                    printf("Студента нет\n");
                }
                break;
            case 10: {
                float avg_all = average_grade_all(students, count);
                trace_above_average(trace_file, students, count, avg_all);
                printf("Студенты выведены\n");
                break;
            }
            case 11:
                free_students(students, count);
                return 0;
        }
    }


    return 0;
}