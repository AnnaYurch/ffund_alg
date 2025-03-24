#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define NAME_OF_FILE "user.bin"
#define CONFIRMATION_CODE 12345

typedef struct User {
    char login[7];
    int PIN;
    int limit_of_command;
} User;

int update_limit_in_file(const char *target_username, int new_limit) {
    FILE *file = fopen(NAME_OF_FILE, "rb+");
    if (!file) {
        printf("Error: Unable to open user database.\n");
        return 0;
    }

    User user;
    int found = 0;

    while (fread(&user, sizeof(User), 1, file)) {
        if (strcmp(user.login, target_username) == 0) {
            user.limit_of_command = new_limit;
            fseek(file, -sizeof(User), SEEK_CUR); // Перемещаем курсор назад
            fwrite(&user, sizeof(User), 1, file);
            found = 1;
            break;
        }
    }

    fclose(file);
    return found;
}

//лимит команд не работает
void shell(char *username, int *limit) {
    char command[256];
    int count = 0;
    while (*limit > 0) {
        if (*limit != -1 && count >= *limit) {
            printf("Limit reached. Logging out.\n");
            break;
        }
        printf("%s> ", username);
        fgets(command, 256, stdin);
        command[strcspn(command, "\n")] = 0;

        // Команда "Time"
        if (strcmp(command, "Time") == 0) {
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            printf("%02d:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
        }
        // Команда "Date"
        else if (strcmp(command, "Date") == 0) {
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            printf("%02d:%02d:%04d\n", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
        }
        // Команда "Howmuch"
        else if (strncmp(command, "Howmuch", 7) == 0) {
            char date_str[11], flag[3];
            int num_items = sscanf(command, "Howmuch %10s %2s", date_str, flag);
            if (num_items != 2) {
                printf("Invalid format. Use: Howmuch <date> <flag>\n");
                continue;
            }

            struct tm t1 = {0};
            if (strptime(date_str, "%d:%m:%Y", &t1) == NULL) { 
                printf("Invalid date format. Use: dd:mm:yyyy\n");
                continue;
            }
            time_t time1 = mktime(&t1);
            time_t now = time(NULL);
            double diff = difftime(now, time1);

            if (strcmp(flag, "-s") == 0) {
                printf("Time difference: %.0f seconds\n", diff);
            } else if (strcmp(flag, "-m") == 0) {
                printf("Time difference: %.0f minutes\n", diff / 60);
            } else if (strcmp(flag, "-h") == 0) {
                printf("Time difference: %.0f hours\n", diff / 3600);
            } else if (strcmp(flag, "-y") == 0) {
                printf("Time difference: %.0f years\n", diff / (60 * 60 * 24 * 365));
            } else {
                printf("Invalid flag. Use -s, -m, -h, or -y\n");
            }
        }
        // Команда "Logout"
        else if (strcmp(command, "Logout") == 0) {
            break;
        } else if (strncmp(command, "Sanctions", 9) == 0) {
            char target_username[7];
            int new_limit, confirmation;

            if (sscanf(command, "Sanctions %6s %d", target_username, &new_limit) != 2) {
                printf("Invalid format. Use: Sanctions <username> <limit>\n");
                continue;
            }

            printf("Enter confirmation code: ");
            scanf("%d", &confirmation);
            getchar(); // Убираем символ новой строки

            if (confirmation != CONFIRMATION_CODE) {
                printf("Sanctions failed: incorrect confirmation code.\n");
                continue;
            }

            if (update_limit_in_file(target_username, new_limit)) {
                printf("Sanctions applied: %s can now execute max %d commands per session.\n", target_username, new_limit);
            } else {
                printf("Error: User %s not found.\n", target_username);
            }
        } else {
            printf("Unknown command.\n");
        }

        count++;
    }
    return; 
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: child <username> <limit>\n");
        return 1;
    }
    char *username = argv[1];
    int limit = atoi(argv[2]);

    shell(username, &limit);
    return 0;
}