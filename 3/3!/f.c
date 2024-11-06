#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <ctype.h>

#define PATH_M 1024

typedef struct Employee {
    unsigned int id;
    char firstName[50];
    char lastName[50];
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

int isValidMoney(double money) {
    return money >= 0;
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

int readEmployees(const char *inputPath, Employee **employees, int *count) {
    FILE *file = fopen(inputPath, "r");
    if (!file) {
        fprintf(stderr, "Trouble with file\n");
        *employees = NULL;
        *count = 0;
        return 1;
    }

    *count = 0;
    *employees = NULL;

    Employee tempEmp;
    while (fscanf(file, "%u %49s %49s %lf", &tempEmp.id, tempEmp.firstName, tempEmp.lastName, 
                  &tempEmp.salary) == 4) {
        if (isdigit(tempEmp.salary) || isValidMoney(tempEmp.salary) == 0 || isValidMoney(tempEmp.id) == 0 || isValidName(tempEmp.firstName) == 0 || isValidName(tempEmp.lastName) == 0){
            fprintf(stderr, "One more mistake in %d\n", tempEmp.id);
            continue;
        }
        Employee *temp = realloc(*employees, (*count + 1) * sizeof(Employee));
        if (!temp) {
            fprintf(stderr, "Memory realloc failed\n");
            free(*employees);
            fclose(file);
            return 1;
        }
        *employees = temp;
        (*employees)[*count] = tempEmp;
        (*count)++;
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
        fprintf(stderr, "Failed to read emp\n");
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
        fprintf(stderr, "We need more args\n");
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