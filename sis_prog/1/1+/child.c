#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define NAME_OF_FILE "user.bin"
#define REAL_CODE 12345

typedef enum Errors {
    INVALID_MEMORY,
    INVALID_ARG,
    INVALID_FILE,
    INVALID_DATA,
    INVALID_LIMIT,
    INVALID_WRITTEN,
    INVALID_READ,
    OK
} Errors;

typedef struct User {
    char login[7];
    int PIN;
    int limit_of_command;
} User;

char *my_strptime(const char *buf, const char *format, struct tm *tm) {
    if (strcmp(format, "%d:%m:%Y") == 0) {
        if (sscanf(buf, "%d:%d:%d", &tm->tm_mday, &tm->tm_mon, &tm->tm_year) != 3) {
            return NULL;
        }
        tm->tm_mon -= 1;          
        tm->tm_year -= 1900;     
        tm->tm_hour = 0;          
        tm->tm_min = 0;
        tm->tm_sec = 0;
        return (char *)(buf + strlen(buf)); 
    }
    return NULL;
}

Errors update_limit_in_file(const char *target_username, int new_limit) {
    FILE *file = fopen(NAME_OF_FILE, "rb+");
    if (!file) {
        return INVALID_FILE;
    }

    User user;
    int found = 0;

    while (fread(&user, sizeof(User), 1, file)) {
        if (strcmp(user.login, target_username) == 0) {
            user.limit_of_command = new_limit;
            fseek(file, -sizeof(User), SEEK_CUR);
            fwrite(&user, sizeof(User), 1, file);
            found = 1;
            break;
        }
    }

    fclose(file);
    if (found == 1) {
        return OK;
    }
    return INVALID_READ;
}

void shell(char *username, int *limit) {
    char command[30];
    int count = 0;
    while (*limit > 0) {
        if (count >= *limit) {
            printf("Limit reached. Logging out.\n");
            break;
        }
        printf("%s> ", username);
        fgets(command, 30, stdin);
        command[strcspn(command, "\n")] = 0;

        if (strcmp(command, "Time") == 0) {
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            printf("%02d:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
        } else if (strcmp(command, "Date") == 0) {
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            printf("%02d:%02d:%04d\n", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
        } else if (strncmp(command, "Howmuch", 7) == 0) {
            char date_str[11], flag[3];
            int num_items = sscanf(command, "Howmuch %10s %2s", date_str, flag);
            if (num_items != 2) {
                printf("Invalid format. Use: Howmuch <date> <flag>1\n");
                continue;
            }
            
            struct tm t1;
            t1.tm_sec   = 0;
            t1.tm_min   = 0;
            t1.tm_hour  = 0;
            t1.tm_mday  = 0;
            t1.tm_mon   = 0;
            t1.tm_year  = 0;
            t1.tm_wday  = 0;
            t1.tm_yday  = 0;
            t1.tm_isdst = 0;

            if (my_strptime(date_str, "%d:%m:%Y", &t1) == NULL) { 
                printf("Invalid date format. Howmuch <date> <flag>2\n");
                continue;
            }
            time_t time1 = mktime(&t1);
            time_t now = time(NULL);
            double diff = (double)(now - time1);

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
        } else if (strcmp(command, "Logout") == 0) {
            break;
        } else if (strncmp(command, "Sanctions", 9) == 0) {
            char target_username[7];
            int new_limit, in_code;

            if (sscanf(command, "Sanctions %6s %d", target_username, &new_limit) != 2) {
                printf("Invalid format. Use: Sanctions <username> <limit>\n");
                continue;
            }

            printf("Enter confirmation code: ");

            size_t i = 0;
            char ch;
            char str_in_code[5 + 1];
            //считываем суперпароль
            while (i < 5) {
                ch = getchar();
                if (ch == '\n') {
                    //printf("Sanctions failed: So litle in_code.\n");
                    break;
                }
                str_in_code[i++] = ch;
            }

            if (ch != '\n') { //если вышли не из-за конца строки
                ch = getchar();
                if (ch != '\n') {
                    printf("So long in_code\n");
                    while (getchar() != '\n') {}
                    continue;
                } 
            }

            str_in_code[i] = '\0';

            if (atoi(str_in_code) != REAL_CODE) {
                printf("Sanctions failed: incorrect in_code2.\n");
                continue;
            }

            if (update_limit_in_file(target_username, new_limit) == OK) {
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