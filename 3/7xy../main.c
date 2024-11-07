#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//не хочет последнего удалять
#define MAX_NAME_LEN 80
#define MAX_DATE_LEN 11 // dd-mm-yyyy
#define MAX_UNDO 100

typedef struct Liver {
    char lastName[MAX_NAME_LEN];
    char firstName[MAX_NAME_LEN];
    char patronymic[MAX_NAME_LEN];
    char birthDate[MAX_DATE_LEN];
    char gender;
    double monthlyMoney;
    struct Liver* next;
} Liver;

typedef struct UndoStack {
    Liver* states[MAX_UNDO];
    int top;
} UndoStack;

void initUndoStack(UndoStack* stack) {
    stack->top = -1;
}

// Создание нового структурного жителя
Liver* createLiver(char* lastName, char* firstName, char* patronymic,
                   char* birthDate, char gender, double monthlyMoney) {
    Liver* newLiver = (Liver*)malloc(sizeof(Liver));
    strcpy(newLiver->lastName, lastName);
    strcpy(newLiver->firstName, firstName);
    strcpy(newLiver->patronymic, patronymic ? patronymic : "");
    strcpy(newLiver->birthDate, birthDate);
    newLiver->gender = gender;
    newLiver->monthlyMoney = monthlyMoney;
    newLiver->next = NULL;
    return newLiver;
}

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

int is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int isValidDate(const char* date) {
    if (strlen(date) != 10 || date[2] != '-' || date[5] != '-') {
        return 0;
    }
    int day = atoi(date);
    int month = atoi(date + 3);
    int year = atoi(date + 6);

    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 0 || year > 2024) {
        return 0;
    }

    int maxDay;
    switch (month) {
        case 4: case 6: case 9: case 11:
            maxDay = 30;
            break;
        case 2:
            maxDay = is_leap_year(year) ? 29 : 28;
            break;
        default:
            maxDay = 31;
    }


    return day <= maxDay;

    return 1;
}

int isValidGender(char gender) {
    return gender == 'M' || gender == 'W';
}

int isValidMoney(double money) {
    return money >= 0;
}

// Добавление жителя в нужное место
void addLiver(Liver** head, char* lastName, char* firstName, char* patronymic,
              char* birthDate, char gender, double monthlyMoney) {
    Liver* newLiver = createLiver(lastName, firstName, patronymic, birthDate, gender, monthlyMoney);
    if (*head == NULL || strcmp(birthDate, (*head)->birthDate) < 0) {
        newLiver->next = *head;
        *head = newLiver;
        return;
    }

    Liver* current = *head;
    while (current->next && strcmp(birthDate, current->next->birthDate) > 0) {
        current = current->next;
    }
    newLiver->next = current->next;
    current->next = newLiver;
}

// Загружаем данные из файла
void loadFromFile(Liver** head, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Не удалось открыть файл для чтения");
        return;
    }

    char lastName[MAX_NAME_LEN], firstName[MAX_NAME_LEN], patronymic[MAX_NAME_LEN];
    char birthDate[MAX_DATE_LEN], gender;
    double money;

    while (fscanf(file, "%s %s %s", lastName, firstName, patronymic) == 3) {
        if (isdigit(patronymic[0])) {
            strcpy(birthDate, patronymic);  //перекопируевам
            patronymic[0] = '\0';  //отчества не ма
        } else {
            fscanf(file, "%s", birthDate);
        }

        fscanf(file, " %c %lf", &gender, &money);

        if (!isValidName(lastName) || !isValidName(firstName) ||
            (strlen(patronymic) > 0 && !isValidName(patronymic)) ||
            !isValidDate(birthDate) || !isValidGender(gender) || !isValidMoney(money)) {
            printf("Ошибка валидации данных для: %s %s\n", lastName, firstName);
            continue;
        }

        addLiver(head, lastName, firstName, strlen(patronymic) > 0 ? patronymic : NULL,
                 birthDate, gender, money);
    }
    fclose(file);
}

// Сохранение жителей в файлик
void saveToFile(Liver* head, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Не удалось открыть файл для записи");
        return;
    }
    while (head) {
        fprintf(file, "%s %s %s %s %c %.2f\n",
                head->lastName, head->firstName, head->patronymic, head->birthDate, head->gender, head->monthlyMoney);
        head = head->next;
    }
    fclose(file);
}

// Ищем жителей
void searchLivers(Liver* head, char* lastName, char* firstName) {
    int found = 0;
    while (head) {
        if (strcmp(head->lastName, lastName) == 0 && strcmp(head->firstName, firstName) == 0) {
            printf("Фамилия: %s, Имя: %s, Отчество: %s, Дата рождения: %s, Пол: %c, Доход: %.2f\n",
                   head->lastName, head->firstName,
                   strlen(head->patronymic) > 0 ? head->patronymic : "(нет)",
                   head->birthDate, head->gender, head->monthlyMoney);
            found = 1;
        }
        head = head->next;
    }
    if (!found) {
        printf("Жители с такими фамилией и именем не найдены.\n");
    }
}

// Меняем инфу жителя
void modifyLiver(Liver* liver, char* birthDate, char gender, double monthlyMoney) {
    strcpy(liver->birthDate, birthDate);
    liver->gender = gender;
    liver->monthlyMoney = monthlyMoney;
}

void modifyLiverInfo(Liver* head, char* lastName, char* firstName, char* patronymic) {
    while (head) {
        if (strcmp(head->lastName, lastName) == 0 &&
            strcmp(head->firstName, firstName) == 0 &&
            (strlen(patronymic) == 0 || strcmp(head->patronymic, patronymic) == 0)) {
            
            printf("Найдена запись. Введите новые данные.\n");
            char birthDate[MAX_DATE_LEN];
            char gender;
            double monthlyMoney;

            printf("Введите новую дату рождения (dd-mm-yyyy): ");
            scanf("%s", birthDate);
            if (!isValidDate(birthDate)) {
                printf("Некорректная дата рождения.\n");
                break;
            }

            printf("Введите пол (M/W): ");
            scanf(" %c", &gender);
            if (!isValidGender(gender)) {
                printf("Некорректный пол.\n");
                break;
            }

            printf("Введите средний доход: ");
            scanf("%lf", &monthlyMoney);
            if (!isValidMoney(monthlyMoney)) {
                printf("Некорректный доход.\n");
                break;
            }

            // Обновляем данные
            modifyLiver(head, birthDate, gender, monthlyMoney);
            printf("Информация о жителе обновлена.\n");
            return;
        }
        head = head->next;
    }
    printf("Житель не найден.\n");
}

void deleteLiver(Liver** head, char* lastName, char* firstName, char* patronymic) {
    Liver* current = *head;
    Liver* prev = NULL;

    while (current) {
        if (strcmp(current->lastName, lastName) == 0 &&
            strcmp(current->firstName, firstName) == 0 &&
            (strlen(patronymic) == 0 || strcmp(current->patronymic, patronymic) == 0)) {
            
            if (prev) {
                prev->next = current->next;
            } else {
                *head = current->next;
            }
            free(current);
            printf("Житель успешно удалён.\n");
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("Житель не найден.\n");
}

void printList(Liver* head) {
    while (head) {
        printf("Фамилия: %s, Имя: %s, Отчество: %s, Дата рождения: %s, Пол: %c, Доход: %.2f\n",
               head->lastName, head->firstName,
               strlen(head->patronymic) > 0 ? head->patronymic : "(нет)",
               head->birthDate, head->gender, head->monthlyMoney);
        head = head->next;
    }
}

// Освобождаем память от нашего списка
void freeList(Liver* head) {
    while (head) {
        Liver* temp = head;
        head = head->next;
        free(temp);
    }
}

// копии табличку жителей
Liver* copyList(Liver* head) {
    if (!head) return NULL;
    
    Liver* newHead = createLiver(head->lastName, head->firstName, head->patronymic,
                                 head->birthDate, head->gender, head->monthlyMoney);
    Liver* current = head->next;
    Liver* newCurrent = newHead;
    
    while (current) {
        newCurrent->next = createLiver(current->lastName, current->firstName, current->patronymic,
                                       current->birthDate, current->gender, current->monthlyMoney);
        newCurrent = newCurrent->next;
        current = current->next;
    }
    return newHead;
}

//добавляем состояние в стек отмен
void pushUndoStack(UndoStack* stack, Liver* head) {
    if (stack->top < MAX_UNDO - 1) {
        stack->states[++stack->top] = copyList(head);
    }
}

//откат последних изменений
void undoChanges(Liver** head, UndoStack* stack) {
    if (stack->top < 0) {
        printf("Нет изменений для отката.\n");
        return;
    }
    
    int undoSteps = (stack->top + 1) / 2;

    if (undoSteps == 0 && stack->top >= 0) {
        undoSteps = 1;
    }

    for (int i = 0; i < undoSteps; ++i) {
        if (stack->top >= 0) {
            freeList(*head);
            *head = stack->states[stack->top--];
        }
    }
    printf("Откат последних %d изменений выполнен.\n", undoSteps);
}

void addLiverWithUndo(Liver** head, char* lastName, char* firstName, char* patronymic,
                      char* birthDate, char gender, double monthlyMoney, UndoStack* stack) {
    pushUndoStack(stack, *head);
    addLiver(head, lastName, firstName, patronymic, birthDate, gender, monthlyMoney);
}

void modifyLiverInfoWithUndo(Liver* head, char* lastName, char* firstName, char* patronymic, UndoStack* stack) {
    pushUndoStack(stack, head);
    modifyLiverInfo(head, lastName, firstName, patronymic);
}

void deleteLiverWithUndo(Liver** head, char* lastName, char* firstName, char* patronymic, UndoStack* stack) {
    pushUndoStack(stack, *head);
    deleteLiver(head, lastName, firstName, patronymic);
}

// Обновленное меню с поддержкой отмены
int main() {
    Liver* head = NULL;
    UndoStack undoStack;
    initUndoStack(&undoStack);
    
    int choice;
    char filename[MAX_NAME_LEN];

    do {
        printf("\nМеню:\n");
        printf("1. Загрузить данные из файла\n");
        printf("2. Сохранить данные в файл\n");
        printf("3. Добавить жителя\n");
        printf("4. Поиск жителя\n");
        printf("5. Изменить информацию о жителе\n");
        printf("6. Удалить жителя\n");
        printf("7. Печать списка жителей\n");
        printf("8. Отмена последних действий\n");
        printf("9. Выйти\n");
        printf("Выберите действие: ");
        if (scanf("%d", &choice) != 1) {
            printf("Некорректный ввод. Пожалуйста, введите число.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1:
                printf("Введите путь к файлу: ");
                scanf("%s", filename);
                loadFromFile(&head, filename);
                break;
            case 2:
                printf("Введите путь к файлу: ");
                scanf("%s", filename);
                saveToFile(head, filename);
                break;
            case 3: {
                char lastName[MAX_NAME_LEN], firstName[MAX_NAME_LEN], patronymic[MAX_NAME_LEN] = "";
                char birthDate[MAX_DATE_LEN], gender;
                double monthlyMoney;
                printf("Введите фамилию: ");
                scanf("%s", lastName);
                if (!isValidName(lastName)) {
                    printf("Некорректная фамилия.\n");
                    break;
                }

                printf("Введите имя: ");
                scanf("%s", firstName);
                if (!isValidName(firstName)) {
                    printf("Некорректное имя.\n");
                    break;
                }

                getchar();
                printf("Введите отчество (если нет, нажмите Enter): ");
                fgets(patronymic, MAX_NAME_LEN, stdin);
                patronymic[strcspn(patronymic, "\n")] = '\0';
                if (strlen(patronymic) > 0 && !isValidName(patronymic)) {
                    printf("Некорректное отчество.\n");
                    break;
                }

                printf("Введите дату рождения (в формате dd-mm-yyyy): ");
                scanf("%s", birthDate);
                if (!isValidDate(birthDate)) {
                    printf("Некорректная дата рождения.\n");
                    break;
                }

                printf("Введите пол (M/W): ");
                scanf(" %c", &gender);
                if (!isValidGender(gender)) {
                    printf("Некорректный пол.\n");
                    break;
                }

                printf("Введите средний доход: ");
                scanf("%lf", &monthlyMoney);
                if (!isValidMoney(monthlyMoney)) {
                    printf("Некорректный доход.\n");
                    break;
                }

                addLiverWithUndo(&head, lastName, firstName, strlen(patronymic) > 0 ? patronymic : NULL,
                                 birthDate, gender, monthlyMoney, &undoStack);
                printf("Житель добавлен.\n");
                break;
            }
            case 4: {
                char lastName[MAX_NAME_LEN], firstName[MAX_NAME_LEN];
                printf("Введите фамилию: ");
                scanf("%s", lastName);
                printf("Введите имя: ");
                scanf("%s", firstName);
                searchLivers(head, lastName, firstName);
                break;
            }
            case 5: {
                char lastName[MAX_NAME_LEN], firstName[MAX_NAME_LEN], patronymic[MAX_NAME_LEN] = "";
                printf("Введите фамилию: ");
                scanf("%s", lastName);
                printf("Введите имя: ");
                scanf("%s", firstName);
                getchar();
                printf("Введите отчество (если нет, нажмите Enter): ");
                fgets(patronymic, MAX_NAME_LEN, stdin);
                patronymic[strcspn(patronymic, "\n")] = '\0';
                modifyLiverInfoWithUndo(head, lastName, firstName, patronymic, &undoStack);
                break;
            }
            case 6: {
                char lastName[MAX_NAME_LEN], firstName[MAX_NAME_LEN], patronymic[MAX_NAME_LEN] = "";
                printf("Введите фамилию: ");
                scanf("%s", lastName);
                printf("Введите имя: ");
                scanf("%s", firstName);
                getchar();
                printf("Введите отчество (если нет, нажмите Enter): ");
                fgets(patronymic, MAX_NAME_LEN, stdin);
                patronymic[strcspn(patronymic, "\n")] = '\0';
                deleteLiverWithUndo(&head, lastName, firstName, patronymic, &undoStack);
                break;
            }
            case 7:
                printList(head);
                break;
            case 8:
                undoChanges(&head, &undoStack);
                break;
            case 9:
                freeList(head);
                printf("Выход из программы.\n");
                break;
            default:
                printf("Некорректный выбор. Попробуйте снова.\n");
        }
    } while (choice != 9);

    return 0;
}