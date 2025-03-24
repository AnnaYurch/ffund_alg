#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

#define MAX_LEN_OF_LOGIN 6 //состоит из символов латинского алфавита и цифр
#define MAX_VALUE_OF_PIN 100000 //целое число в 10сс от 0 до 100000
#define MIN_LIMIT_OF_COMMANDS 10
#define NO_LIMIT_OF_COMMANDS -1 

//максимальное количество логинов = 36^6 = 2176782336
//сл. максимальное колиство пользователей = 2176782336
size_t MAX_NORMAL_NUM_OF_USERS = 10000;
#define MAX_UNNORMAL_NUM_OF_USERS 2176782336 
#define NAME_OF_FILE "user.bin"

typedef enum Errors {
    INVALID_MEMORY,
    INVALID_ARG,
    INVALID_FILE,
    INVALID_DATA,
    INVALID_LIMIT,
    INVALID_WRITTEN,
    OK
} Errors;

typedef struct User {
    char login[MAX_LEN_OF_LOGIN + 1];
    int PIN;
    int limit_of_command;
} User;

User *all_users = NULL;
size_t all_users_count = 0;

//не нужно
int we_have_same_login(const char *login) {
    for (size_t i = 0; i < all_users_count; i++) {
        if (strcmp(all_users[i].login, login) == 0) {
            return 1;
        }
    }

    return 0;
}

int is_valid_login(const char *login) {
    for (size_t i = 0; i < strlen(login); i++) {
        if (!isalpha(login[i])) {
            return 0;
        }
    }

    return 1;
}

int is_valid_pin(const char *pin) {
    for (size_t i = 0; i < strlen(pin); i++) {
        if (!isdigit(pin[i])) {
            return 0;
        }
    }

    return 1;
}

void print_users() {
    if (!all_users || all_users_count == 0) {
        printf("NO USERS\n");
        return;
    }

    printf("All users if file (Login / PIN / Limit of command): \n");
    printf("----------------------------------------------------\n");

    for (size_t i = 0; i < all_users_count; i++) {
        printf("%-7s | %-6d | %d\n", all_users[i].login, all_users[i].PIN, all_users[i].limit_of_command);
        //вывод с выравниванием
    }
    printf("----------------------------------------------------\n");
}

Errors loading_of_users() { //используется один раз в начале работы программы
    FILE *file_bin = fopen(NAME_OF_FILE, "rb");
    if (!file_bin) {
        return INVALID_FILE;
    } 

    size_t capacity = MAX_NORMAL_NUM_OF_USERS;
    all_users = (User*)malloc(sizeof(User) * capacity);
    if (!all_users) {
        fclose(file_bin);
        return INVALID_MEMORY;
    }

    all_users_count = 0;
    while (fread(&all_users[all_users_count], sizeof(User), 1, file_bin)) {
        all_users_count++;

        if (all_users_count >= MAX_UNNORMAL_NUM_OF_USERS) {
            fclose(file_bin);
            return INVALID_LIMIT;
        }

        if (all_users_count >= capacity) {
            size_t new_capacity = capacity * 2;
            if (new_capacity > MAX_UNNORMAL_NUM_OF_USERS) {
                new_capacity = MAX_UNNORMAL_NUM_OF_USERS;
            }

            User *new_users = (User *)realloc(all_users, sizeof(User) * new_capacity);
            if (!new_users) {
                fclose(file_bin);
                return INVALID_MEMORY;
            }

            MAX_NORMAL_NUM_OF_USERS = new_capacity;
            all_users = new_users;
            capacity = new_capacity;
        }
    }

    fclose(file_bin);
    return OK;
}

Errors try_to_find_user(char *login, int pin, int *res_user_index) {
    if (!login || !res_user_index) {
        return INVALID_ARG;
    }
    for (size_t i = 0; i < all_users_count; i++) {
        if (strcmp(all_users[i].login, login) == 0 && all_users[i].PIN == pin) {
            *res_user_index = i;
            return OK;
        }
    }

    *res_user_index = -1;
    return OK;
}

Errors save_users(User new_user) {
    FILE *file = fopen(NAME_OF_FILE, "ab");
    if (!file) {
        return INVALID_FILE;
    }

    size_t how_much_written = fwrite(&new_user, sizeof(User), 1, file);
    fclose(file);

    if (how_much_written < 1) {
        return INVALID_WRITTEN;
    }

    return OK;
}

Errors register_user(char *login, int pin, int *res_user_index) {
    if (!login || !res_user_index) {
        return INVALID_ARG;
    }

    if (all_users_count + 1 >= MAX_UNNORMAL_NUM_OF_USERS) {
        return INVALID_LIMIT;
    }

    //printf("2");
    size_t capacity = MAX_NORMAL_NUM_OF_USERS; // начальная емкость
    if (all_users_count + 1 >= capacity) {
        size_t new_capacity = capacity * 2; // Увеличиваем в 2 раза
        if (new_capacity > MAX_UNNORMAL_NUM_OF_USERS) {
            new_capacity = MAX_UNNORMAL_NUM_OF_USERS;
        }

        User *new_users = (User *)realloc(all_users, sizeof(User) * new_capacity);
        if (!new_users) {
            return INVALID_MEMORY;
        }

        all_users = new_users;
        capacity = new_capacity;
    }

    strcpy(all_users[all_users_count].login, login);
    all_users[all_users_count].PIN = pin;
    all_users[all_users_count].limit_of_command = MIN_LIMIT_OF_COMMANDS;
    all_users_count++;
    *res_user_index = all_users_count - 1;

    Errors err_of_save_users = save_users(all_users[*res_user_index]);
    if (err_of_save_users != OK) {
        return err_of_save_users;
    }

    return OK;
}

int main() {
    //загружаем всех наших пользователей из файла (они считаются корректными)
    Errors err_after_loading = loading_of_users();
    if (err_after_loading == INVALID_FILE) {
        printf("INVALID_FILE\n");
        free(all_users);
        all_users = NULL;
        return 1;
    } else if (err_after_loading == INVALID_MEMORY) {
        printf("INVALID_MEMORY");
        free(all_users);
        all_users = NULL;
        return 1;
    } else if (err_after_loading == OK) {
        print_users();
    }

    while (1) {
        char login[MAX_LEN_OF_LOGIN + 1];
        int pin;
        
        printf("Login (up to 6 characters): ");
        size_t i = 0;
        char ch;
        //считываем login
        while (i < 5) {
            ch = getchar();
            if (ch == '\n') {
                break;
            }
            login[i++] = ch;
        }

        if (ch != '\n') { //если вышли не из-за конца строки
            ch = getchar();
            if (ch != '\n') {
                printf("So long login\n");
                while (getchar() != '\n') {}
                continue;
            } 
        }

        login[i] = '\0';

        //printf("%d %s", is_valid_login(login), login);
        if (!is_valid_login(login)) {
            printf("Invalid login!\n");
            continue;
        }

        // cчитываем PIN
        char str_pin[5 + 1]; //100000
        printf("\nPIN (must be between 0 and 100000): ");
        i = 0;
        while (i < 5) {
            ch = getchar();
            if (ch == '\n') {
                break;
            }
            str_pin[i++] = ch;
        }

        if (ch != '\n') { //если вышли не из-за конца строки
            ch = getchar();
            if (ch != '\n') {
                printf("So long PIN\n");
                while (getchar() != '\n') {}
                continue;
            } 
        }

        str_pin[i] = '\0';

        if (!is_valid_pin(str_pin)) {
            printf("Invalid PIN!\n");
            continue;
        }

        pin = atoi(str_pin);
        //getchar();

        int user_index = MIN_LIMIT_OF_COMMANDS;
        Errors err_of_find = try_to_find_user(login, pin, &user_index);
        if (err_of_find == INVALID_ARG) {
            printf("INVALID_ARG");
            free(all_users);
            all_users = NULL;
            return 1;
        }

        if (user_index == -1) {
            printf("User not found. Register? (y/n): ");

            char choice;
            scanf(" %c", &choice);
            if (getchar() != '\n') {
                printf("Incorrect choice\n");
                while (getchar() != '\n') {}
                continue;
            }

            if (choice == 'y') {
                if (we_have_same_login(login)) {
                    printf("We have same login\n");
                    continue;
                }
                Errors err_of_reg = register_user(login, pin, &user_index);
                
                if (err_of_reg == INVALID_ARG) {
                    printf("INVALID_ARG");
                    free(all_users);
                    all_users = NULL;
                    return 1;
                } else if (err_of_reg == INVALID_FILE) {
                    printf("INVALID_FILE");
                    free(all_users);
                    all_users = NULL;
                    return 1;
                } else if (err_of_reg == INVALID_LIMIT || user_index == -1) {
                    //printf("%d", user_index);
                    printf("INVALID_LIMIT\n");
                    printf("Users limit reached. Bye!\n");
                    free(all_users);
                    all_users = NULL;
                    return 1;
                }
            } else {
                continue;
            }
        }

        //printf("continue\n");

        pid_t pid = fork();
        //после этого возникают 2 идентичных процесса (родитель=индефикатру ребенка/ребенок=0)
        if (pid == 0) {
            char limit_str[10];
            sprintf(limit_str, "%d", all_users[user_index].limit_of_command);
            //заменяем текущий процесс на child (загружаем и запускаем child)
            execl("./child", "child", all_users[user_index].login, limit_str, NULL);
            //дальше ничего не выполняется, если execl успешен
            printf("Failed to execute child process");
            return 1;
        } else if (pid > 0) {
            wait(NULL);
        } else {
            printf("fork failed");
            return 1;
        }
    }

    free(all_users);
    return 0;
}