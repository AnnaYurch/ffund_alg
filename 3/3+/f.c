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

typedef enum kOpts {
    OPT_A,
    OPT_D
} kOpts;

typedef enum Errors {
    INVALID_MEMORY,
    INVALID_ARG,
    INVALID_FILE,
    INVALID_DATA,
    OK
} Errors;

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

int isValidMoney(const char *money) {
    int has_dem_point = 0;

    for (int i = 0; money[i] != '\0'; i++) {
        if (money[i] == '.') {
            if(has_dem_point) {
                return 0;
            }
            has_dem_point = 1;
        }else if (!isdigit(money[i])) {
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

Errors readID(FILE *file, unsigned int *id) {
    char buffer[10];
    int index = 0;
    int ch;

    while ((ch = fgetc(file)) != EOF && ch != ' ' && ch != '\n') {
        if (isdigit(ch)) {
            if (index < sizeof(buffer) - 1) {
                buffer[index++] = ch;
            } else {
                while ((ch = fgetc(file)) != '\n' && ch != EOF);
                return INVALID_DATA;
            }
        } else {
            while ((ch = fgetc(file)) != '\n' && ch != EOF);
            return INVALID_DATA;
        }
    }


    buffer[index] = '\0';
    *id = atoi(buffer);
    return OK;
}

Errors readName(FILE *file, char *name, const char *fieldName) {
    int index = 0;
    int ch;

    while ((ch = fgetc(file)) == ' ');
    if (ch == EOF || ch == '\n') {
        return INVALID_DATA;
    }

    while (ch != EOF && ch != ' ' && ch != '\n') {
        if (index < MAX_LEN_NAME - 1) {
            name[index++] = ch;
        } else {
            while ((ch = fgetc(file)) != '\n' && ch != EOF);
            return INVALID_DATA;
        }
        ch = fgetc(file);
    }

    name[index] = '\0';
    if (!isValidName(name)) {
        return INVALID_DATA;
    }

    return OK;
}

Errors readSalary(FILE *file, double *salary) {
    char buffer[10];
    int index = 0;
    int ch;

    while ((ch = fgetc(file)) == ' ');
    if (ch == EOF || ch == '\n') {
        return INVALID_DATA;
    }

    while (ch != EOF && ch != ' ' && ch != '\n') {
        if (isdigit(ch) || ch == '.') {
            if (index < sizeof(buffer) - 1) {
                buffer[index++] = ch;
            } else {
                while ((ch = fgetc(file)) != '\n' && ch != EOF);
                return INVALID_DATA;
            }
        } else {
            while ((ch = fgetc(file)) != '\n' && ch != EOF);
            return INVALID_DATA;
        }
        ch = fgetc(file);
    }

    buffer[index] = '\0';
    if (!isValidMoney(buffer)) {
        return INVALID_DATA;
    }

    *salary = atof(buffer);
    return OK;
}

Errors readEmployees(const char *file_in, Employee **employees, int *count) {
    FILE *file = fopen(file_in, "r");
    if (!file) {
        return INVALID_FILE;
    }

    *count = 0;
    int capacity = 10;
    *employees = (Employee *)malloc(capacity * sizeof(Employee));
    if (*employees == NULL) {
        fclose(file);
        return INVALID_MEMORY;
    }

    while (1) {
        if (*count >= capacity) {
            capacity *= 2;
            Employee *temp = (Employee *)realloc(*employees, capacity * sizeof(Employee));
            if (temp == NULL) {
                free(*employees);
                fclose(file);
                return INVALID_MEMORY;
            }
            *employees = temp;
        }

        Employee emp;

        if (readID(file, &emp.id) != OK) {
            continue; 
        }

        if (readName(file, emp.firstName, "first name") != OK) {
            continue; 
        }

        if (readName(file, emp.lastName, "last name") != OK) {
            continue; 
        }

        if (readSalary(file, &emp.salary) != OK) {
            continue; 
        }

        int idExists = 0; //проверка что ID уже есть
        for (int i = 0; i < *count; ++i) {
            if ((*employees)[i].id == emp.id) {
                idExists = 1;
                break;
            }
        }
        if (idExists) {
            continue; 
        }

        (*employees)[(*count)++] = emp;

        if (feof(file)) {
            break;
        }
    }

    fclose(file);
    return OK;
}

Errors writeEmployees(const char *outputPath, Employee *employees, int count) {
    FILE *file = fopen(outputPath, "w");
    if (!file) {
        return INVALID_FILE;
    }

    for (int i = 0; i < count; i++) {
        fprintf(file, "%u %s %s %.2lf\n", employees[i].id, employees[i].firstName,
                employees[i].lastName, employees[i].salary);
    }

    fclose(file);
    return OK;
}

Errors HandlerOptA(const char *inputPath, const char *outputPath) {
    Employee *employees;
    int count;
    Errors err;

    err = readEmployees(inputPath, &employees, &count);
    if (err != OK) {
        return OK;
    }
    qsort(employees, count, sizeof(Employee), compar_emp);
    err = writeEmployees(outputPath, employees, count);
    if (err != OK) {
        return OK;
    }
    free(employees);
    return OK;
}

Errors HandlerOptD(const char *inputPath, const char *outputPath) {
    Employee *employees;
    int count;
    Errors err;

    err = readEmployees(inputPath, &employees, &count);
    if (err != OK) {
        return OK;
    }
    qsort(employees, count, sizeof(Employee), compar_emp);
    for (int i = 0; i < count / 2; i++) {
        Employee temp = employees[i];
        employees[i] = employees[count - i - 1];
        employees[count - i - 1] = temp;
    }
    err = writeEmployees(outputPath, employees, count);
    if (err != OK) {
        return OK;
    }
    free(employees);
    return OK;
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

    Errors err;
    if (option == OPT_A) {
        err = HandlerOptA(input, output);
    } else {
        err = HandlerOptD(input, output);
    }

    if (err == INVALID_ARG) {
        printf("INVALID_ARG\n");
    } else if (err == INVALID_FILE) {
        printf("INVALID_FILE\n");
    } else if (err == INVALID_MEMORY) {
        printf("INVALID_MEMORY\n");
    } else if (err == INVALID_DATA) {
        printf("INVALID_DATA\n");
    }



    return 0;
}

///////////////////////

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

typedef enum kOpts {
    OPT_A,
    OPT_D
} kOpts;

typedef enum Errors {
    INVALID_MEMORY,
    INVALID_ARG,
    INVALID_FILE,
    OK
} Errors;

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

int isValidMoney(const char *money) {
    int has_dem_point = 0;

    for (int i = 0; money[i] != '\0'; i++) {
        if (money[i] == '.') {
            if(has_dem_point) {
                return 0;
            }
            has_dem_point = 1;
        }else if (!isdigit(money[i])) {
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

Errors readEmployees(const char *file_in, Employee **employees, int *count) {
    FILE *file = fopen(file_in, "r");
    if (!file) {
        //fprintf(stderr, "Could not open file %s\n", file_in);
        return INVALID_FILE;
    }

    *count = 0;
    int capacity = 10;
    *employees = (Employee *)malloc(capacity * sizeof(Employee));
    if (*employees == NULL) {
        //fprintf(stderr, "Memory yps1\n");
        fclose(file);
        return INVALID_MEMORY;
    }

    while (1) {
        if (*count >= capacity) {
            capacity *= 2;
            Employee *temp = (Employee *)realloc(*employees, capacity * sizeof(Employee));
            if (temp == NULL) {
                //fprintf(stderr, "Memory yps2\n");
                free(*employees);
                fclose(file);
                return INVALID_MEMORY;
            }
            *employees = temp;
        }

        Employee emp;
        char buffer[10];
        int index = 0;
        int ch;

        //считываем ID
        int flag_id = 1; 
        while ((ch = fgetc(file)) != EOF && ch != ' ' && ch != '\n') {
            if (isdigit(ch)) {
                if (index < sizeof(buffer) - 1) {
                    buffer[index++] = ch;
                } else {
                    //fprintf(stderr, "ID is too long. ");
                    while ((ch = fgetc(file)) != '\n' && ch != EOF) {};
                    break;
                }
            } else {
                //fprintf(stderr, "Invalid character in ID. ");
                flag_id = 0;
                while ((ch = fgetc(file)) != '\n' && ch != EOF);
                break;
            }
        }

        if (ch == '\n') {
            //fprintf(stderr, "Uncorrect student\n");
            continue;
        }

        buffer[index] = '\0';
        emp.id = atoi(buffer);

        int flag = 1;
        for (int i = 0; i < *count; ++i) {
            if ((*employees)[i].id == emp.id) {
                flag = 0;
                //fprintf(stderr, "Такой id (%u) студента уже есть\n", emp.id);
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
                //fprintf(stderr, "First name is too long. ");
                flag_name = 0;
                while ((ch = fgetc(file)) != '\n' && ch != EOF);
                break;
            }
            ch = fgetc(file);
        }

        if (ch == '\n') {
            //fprintf(stderr, "Uncorrect student2\n");
            continue;
        }

        emp.firstName[index] = '\0';
        if (!isValidName(emp.firstName) && flag_name != 0) {
            //fprintf(stderr, "Uncorrect name. ");
            while ((ch = fgetc(file)) != '\n' && ch != EOF) {}
        }

        if (ch == '\n') {
            //fprintf(stderr, "Uncorrect student22\n");
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
                //fprintf(stderr, "Last name is too long. ");
                flag_surname = 0;
                while ((ch = fgetc(file)) != '\n' && ch != EOF);
                break;
            }
            ch = fgetc(file);
        }

        if (ch == '\n') {
            //fprintf(stderr, "Uncorrect student3\n");
            continue;
        }

        emp.lastName[index] = '\0';

        if (!isValidName(emp.lastName) && flag_surname != 0) {
            //fprintf(stderr, "Uncorrect surname. ");
            while ((ch = fgetc(file)) != '\n' && ch != EOF) {}
        }

        if (ch == '\n') {
            //fprintf(stderr, "Uncorrect student33\n");
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
                    //fprintf(stderr, "Salary is too long. ");
                    flag_salary = 0;
                    while ((ch = fgetc(file)) != '\n' && ch != EOF) {};
                    break;
                }
            } else {
                //(stderr, "Invalid character in salary. ");
                flag_salary = 0;
                while ((ch = fgetc(file)) != '\n' && ch != EOF);
                break;
            }
            
            ch = fgetc(file);
        }

        buffer[index] = '\0';

        //валидность
        if (isValidMoney(buffer) == 0 || flag_salary == 0) {
            //fprintf(stderr, "Incorrect salary\n");
            while ((ch = fgetc(file)) != '\n' && ch != EOF) {}
            continue;
        }
        emp.salary = atof(buffer);

        if (index == 0) {
            //fprintf(stderr, "Invalid salary format\n");
            continue;
        }

        //printf("%u\n", emp.id);
        (*employees)[(*count)++] = emp;

        if (ch == EOF) {
            break;
        }
    }

    fclose(file);
    return OK;
}

Errors writeEmployees(const char *outputPath, Employee *employees, int count) {
    FILE *file = fopen(outputPath, "w");
    if (!file) {
        return INVALID_FILE;
    }

    for (int i = 0; i < count; i++) {
        fprintf(file, "%u %s %s %.2lf\n", employees[i].id, employees[i].firstName,
                employees[i].lastName, employees[i].salary);
    }

    fclose(file);
    return OK;
}

Errors HandlerOptA(const char *inputPath, const char *outputPath) {
    Employee *employees;
    int count;
    Errors err;

    err = readEmployees(inputPath, &employees, &count);
    if (err != OK) {
        return OK;
    }
    qsort(employees, count, sizeof(Employee), compar_emp);
    err = writeEmployees(outputPath, employees, count);
    if (err != OK) {
        return OK;
    }
    free(employees);
    return OK;
}

Errors HandlerOptD(const char *inputPath, const char *outputPath) {
    Employee *employees;
    int count;
    Errors err;

    err = readEmployees(inputPath, &employees, &count);
    if (err != OK) {
        return OK;
    }
    qsort(employees, count, sizeof(Employee), compar_emp);
    for (int i = 0; i < count / 2; i++) {
        Employee temp = employees[i];
        employees[i] = employees[count - i - 1];
        employees[count - i - 1] = temp;
    }
    err = writeEmployees(outputPath, employees, count);
    if (err != OK) {
        return OK;
    }
    free(employees);
    return OK;
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

    Errors err;
    if (option == OPT_A) {
        err = HandlerOptA(input, output);
    } else {
        err = HandlerOptD(input, output);
    }

    if (err == INVALID_ARG) {
        printf("INVALID_ARG\n");
    } else if (err == INVALID_FILE) {
        printf("INVALID_FILE\n");
    } else if (err == INVALID_MEMORY) {
        printf("INVALID_MEMORY\n");
    }

    return 0;
}