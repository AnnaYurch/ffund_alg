#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <ctype.h>

#define PATH_M 1024
#define MAX_LEN_NAME 50

typedef struct Employee {
    unsigned int id;
    char firstName[MAX_LEN_NAME];
    char lastName[MAX_LEN_NAME];
    double salary;
} Employee;

typedef enum kOpts{
    OPT_A,
    OPT_D
} kOpts;

int Get_Opts(char* b_opt, kOpts *option) {
    if (strlen(b_opt) != 2) {
        return 1;
    }

    if (b_opt[0] == '/' || b_opt[0] == '-') {
        switch (b_opt[1]) {
        case 'a':
            *option = OPT_A;
            break;
        case 'd':
            *option = OPT_D;
            break;
        default:
            return 1;
        }
    } else {
        return 1;
    }

    return 0;
}

int isValidName(const char* name) {
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


int isValidMoney(const char *id) {
    int has_dem_point = 0;

    for (int i = 0; id[i] != '\0'; i++) {
        if (id[i] == '.') {
            if(has_dem_point) {
                return 0;
            }
            has_dem_point = 1;
        }else if (!isdigit(id[i])) {
            return 0;
        }
    }
    return 1;
}

int compar_emp(const void *a, const void *b) { 
    Employee *empA = (Employee *)a;
    Employee *empB = (Employee *)b;

    if (empA->salary != empB->salary) {
        return (empA->salary > empB->salary) ? 1 : -1;
    }
    int lastNameCompare = strcmp(empA->lastName, empB->lastName);
    if (lastNameCompare != 0) {
        return lastNameCompare;
    }
    int firstNameCompare = strcmp(empA->firstName, empB->firstName);
    if (firstNameCompare != 0) {
        return firstNameCompare;
    }
    return (empA->id > empB->id) ? 1 : -1;
}

int readEmployees(const char *file_in, Employee **employees, int *count) {
    FILE *file = fopen(file_in, "r");
    if (!file) {
        fprintf(stderr, "Could not open file %s\n", file_in);
        return 1;
    }

    *count = 0;
    int capacity = 10;
    *employees = malloc(capacity * sizeof(Employee));
    if (*employees == NULL) {
        fprintf(stderr, "Memory yps1\n");
        fclose(file);
        return 1;
    }

    while (1) {
        if (*count >= capacity) {
            capacity *= 2;
            Employee *temp = realloc(*employees, capacity * sizeof(Employee));
            if (temp == NULL) {
                fprintf(stderr, "Memory yps2\n");
                free(*employees);
                fclose(file);
                return 0;
            }
            *employees = temp;
        }

        Employee emp;
        char buffer[MAX_LEN_NAME]; //поменять на 10
        int index = 0;
        int ch;

        //считываем ID
        int flag_id = 1; 
        while ((ch = fgetc(file)) != EOF && ch != ' ' && ch != '\n') {
            if (isdigit(ch)) {
                if (index < sizeof(buffer) - 1) {
                    buffer[index++] = ch;
                } else {
                    fprintf(stderr, "ID is too long. ");
                    while ((ch = fgetc(file)) != '\n' && ch != EOF) {};
                    break;
                }
            } else {
                fprintf(stderr, "Invalid character in ID. ");
                flag_id = 0;
                while ((ch = fgetc(file)) != '\n' && ch != EOF);
                break;
            }
        }

        if (ch == '\n') {
            fprintf(stderr, "Uncorrect student\n");
            continue;
        }

        buffer[index] = '\0';
        emp.id = atoi(buffer);

        int flag = 1;
        for (int i = 0; i < *count; ++i) {
            if ((*employees)[i].id == emp.id) {
                flag = 0;
                fprintf(stderr, "Такой id (%u) студента уже есть\n", emp.id);
                break;
            }
        }
        
        if (flag == 0) {
            while ((ch = fgetc(file)) != '\n' && ch != EOF) {
                continue;
            }
            continue;
        }

        while (ch == ' ') {
            ch = fgetc(file);
        }
        

        //считываем имя
        index = 0;
        int flag_name = 1;
        while (ch != EOF && ch != ' ' && ch != '\n') {
            if (index < sizeof(emp.firstName) - 1) {
                emp.firstName[index++] = ch;
            } else {
                fprintf(stderr, "First name is too long. ");
                flag_name = 0;
                while ((ch = fgetc(file)) != '\n' && ch != EOF);
                break;
            }
            ch = fgetc(file);
        }

        if (ch == '\n') {
            fprintf(stderr, "Uncorrect student2\n");
            continue;
        }

        emp.firstName[index] = '\0';
        if (!isValidName(emp.firstName) && flag_name != 0) {
            fprintf(stderr, "Uncorrect name. ");
            while ((ch = fgetc(file)) != '\n' && ch != EOF) {}
        }

        if (ch == '\n') {
            fprintf(stderr, "Uncorrect student22\n");
            continue;
        }

        while (ch == ' ') {
            ch = fgetc(file);
        }

        //считываем фамилию
        int flag_surname = 1;
        index = 0;
        while (ch != EOF && ch != ' ' && ch != '\n') {
            if (index < sizeof(emp.lastName) - 1) {
                emp.lastName[index++] = ch;
            } else {
                fprintf(stderr, "Last name is too long. ");
                flag_surname = 0;
                while ((ch = fgetc(file)) != '\n' && ch != EOF);
                break;
            }
            ch = fgetc(file);
        }

        if (ch == '\n') {
            fprintf(stderr, "Uncorrect student3\n");
            continue;
        }

        emp.lastName[index] = '\0';

        if (!isValidName(emp.lastName) && flag_surname != 0) {
            fprintf(stderr, "Uncorrect surname. ");
            while ((ch = fgetc(file)) != '\n' && ch != EOF) {}
        }

        if (ch == '\n') {
            fprintf(stderr, "Uncorrect student33\n");
            continue;
        }

        while (ch == ' ') {
            ch = fgetc(file);
        }

        //считываем зарплату
        index = 0;
        int flag_salary = 1;
        while (ch != EOF && ch != ' ' && ch != '\n') {
            if (isdigit(ch) || ch == '.') {
                if (index < sizeof(buffer) - 1) {
                    buffer[index++] = ch;
                } else {
                    fprintf(stderr, "Salary is too long. ");
                    flag_salary = 0;
                    while ((ch = fgetc(file)) != '\n' && ch != EOF) {};
                    break;
                }
            } else {
                fprintf(stderr, "Invalid character in salary. ");
                flag_salary = 0;
                while ((ch = fgetc(file)) != '\n' && ch != EOF);
                break;
            }
            
            ch = fgetc(file);
        }

        buffer[index] = '\0';

        //валидность
        if (isValidMoney(buffer) == 0 || flag_salary == 0) {
            fprintf(stderr, "Incorrect salary\n");
            while ((ch = fgetc(file)) != '\n' && ch != EOF) {}
            continue;
        }
        emp.salary = atof(buffer);

        if (index == 0) {
            fprintf(stderr, "Invalid salary format\n");
            continue;
        }

        printf("%u\n", emp.id);
        (*employees)[(*count)++] = emp;

        if (ch == EOF) {
            break;
        }
    }

    fclose(file);
    return 0;
}

void writeEmployees(const char *outputPath, Employee *employees, int count) {
    FILE *file = fopen(outputPath, "w");
    if (!file) {
        fprintf(stderr, "Trouble with file\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(file, "%u %s %s %.2lf\n", employees[i].id, employees[i].firstName,
                employees[i].lastName, employees[i].salary);
    }

    fclose(file);
}

void HandlerOptA(const char *inputPath, const char *outputPath) {
    Employee *employees;
    int count;

    if (readEmployees(inputPath, &employees, &count) != 0) {
        fprintf(stderr, "Failed with reading emp\n");
        return;
    }
    qsort(employees, count, sizeof(Employee), compar_emp);
    writeEmployees(outputPath, employees, count);
    free(employees);
}

void HandlerOptD(const char *inputPath, const char *outputPath) {
    Employee *employees;
    int count;

    readEmployees(inputPath, &employees, &count);
    qsort(employees, count, sizeof(Employee), compar_emp);
    for (int i = 0; i < count / 2; i++) {
        Employee temp = employees[i];
        employees[i] = employees[count - i - 1];
        employees[count - i - 1] = temp;
    }
    writeEmployees(outputPath, employees, count);
    free(employees);
}

int main(int argc, char** argv) {
    kOpts option;
    char input[PATH_M];
    char output[PATH_M];

    if (argc != 4) {
        fprintf(stderr, "We need more args ./a.out -flag <file> <file>\n");
        return 1;
    }

    if (realpath(argv[2], input) == NULL) {
        fprintf(stderr, "Something wrong with input file\n");
        return 1;
    }

    if (realpath(argv[3], output) == NULL) {
        strncpy(output, argv[3], PATH_M);
    }
    //printf("%s, %s\n", input, output);
    if (strcmp(input, output) == 0) {
        fprintf(stderr, "Files are the same\n");
        return 1;
    }

    if (Get_Opts(argv[1], &option) == 1) {
        fprintf(stderr, "Something wrong with opts\n");
        return 1;
    }

    if (option == OPT_A) {
        HandlerOptA(input, output);
    } else {
        HandlerOptD(input, output);
    }

    return 0;
}