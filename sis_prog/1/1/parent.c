#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LEN_OF_LOGIN 6 //состоит из символов латинского алфавита и цифр
#define MAX_VALUE_OF_PIN 100000 //целое число в 10сс от 0 до 100000
#define MIN_LIMIT_OF_COMMANDS 10

//максимальное количество логинов = 36^6 = 2176782336
//сл. максимальное колиство пользователей = 2176782336
#define MAX_NORMAL_NUM_OF_USERS 10000 
#define MAX_UNNORMAL_NUM_OF_USERS 2176782336 
#define NAME_OF_FILE "user.bin"


typedef enum Errors {
    INVALID_MEMORY,
    INVALID_ARG,
    INVALID_FILE,
    INVALID_DATA,
    INVALID_LIMIT,
    OK
} Errors;

typedef struct User {
    char login[MAX_LEN_OF_LOGIN + 1];
    int PIN;
    int limit_of_command;
} User;

User *all_users = NULL;
size_t all_users_count = 0;

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

    all_users = (User*)malloc(sizeof(User) * MAX_NORMAL_NUM_OF_USERS);
    if (!all_users) {
        fclose(file_bin);
        return INVALID_MEMORY;
    }
    
    //а если юзеров больше?
    all_users_count = fread(all_users, sizeof(User), MAX_NORMAL_NUM_OF_USERS, file_bin);

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

Errors save_users() {
    FILE *file = fopen(NAME_OF_FILE, "wb");
    if (!file) {
        return INVALID_FILE;
    }

    //а если кто-то не записался?
    fwrite(all_users, sizeof(User), all_users_count, file);
    fclose(file);
    return OK;
}

Errors register_user(char *login, int pin, int *res_user_index) {
    if (!login || !res_user_index) {
        return INVALID_ARG;
    }

    //расширить malloc до MAX_UNNORMAL_NUM_OF_USERS?
    if (all_users_count + 1 >= MAX_NORMAL_NUM_OF_USERS) {
        return INVALID_LIMIT;
    }

    strcpy(all_users[all_users_count].login, login);
    all_users[all_users_count].PIN = pin;
    all_users[all_users_count].limit_of_command = MIN_LIMIT_OF_COMMANDS;
    all_users_count++;
    Errors err_of_save_users = save_users();
    if (err_of_save_users != OK) {
        return err_of_save_users;
    }
    *res_user_index = all_users_count - 1;

    return OK;
}

int main() {
    //загружаем всех наших пользователей из файла (они считаются корректными)
    Errors err_after_loading = loading_of_users();
    if (err_after_loading == INVALID_FILE) {
        printf("INVALID_FILE\n");
        return 1;
    } else if (err_after_loading == INVALID_MEMORY) {
        printf("INVALID_MEMORY");
        return 1;
    } else if (err_after_loading == OK) {
        print_users();
    } else {
        printf("1");
    }

    while (1) {
        //валидация
        char login[MAX_LEN_OF_LOGIN + 1];
        int pin;
        printf("Login: ");
        scanf("%6s", login);
        printf("PIN: ");
        scanf("%d", &pin);
        getchar(); //зачем это?
        
        int user_index = -1;
        Errors err_of_find = try_to_find_user(login, pin, &user_index);
        if (err_of_find == INVALID_ARG) {
            printf("INVALID_ARG");
            return 1;
        }

        if (user_index == -1) {
            printf("User not found. Register? (y/n): ");
            //валидация
            char choice;
            scanf(" %c", &choice);
            getchar();

            if (choice == 'y') {
                Errors err_of_reg = register_user(login, pin, &user_index);
                if (err_of_reg == INVALID_ARG) {
                    printf("INVALID_ARG");
                    return 1;
                } else if (err_of_reg == INVALID_FILE) {
                    printf("INVALID_FILE");
                    return 1;
                } else if (err_of_reg == INVALID_LIMIT || user_index == -1) {
                    printf("INVALID_LIMIT\n");
                    printf("Users limit reached. Bye!\n");
                    return 1;
                }
            } else {
                continue;
            }
        }

        //printf("continue\n");

        pid_t pid = fork();
        if (pid == 0) {
            char limit_str[10];
            sprintf(limit_str, "%d", all_users[user_index].limit_of_command);
            execl("./child", "child", all_users[user_index].login, limit_str, NULL);
            perror("Failed to execute child process");
            exit(1); //0?
        } else {
             wait(NULL);
        }
    }


    free(all_users);
    return 0;
}

//база данных для хранения пользователей

//регистрация и авторизация + корректность данных (если такой же чудик в базе данных и правильность PIN и login)

//создать родительский процесс для управления детками

//в ребенке все функции программы

//использовать pipe или shared memory + обеспечить корректное завершение работы процессов