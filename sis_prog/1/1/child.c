#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void shell(char *username, int limit) {
    char command[256];
    int count = 0;
    while (1) {
        if (limit != -1 && count >= limit) {
            printf("Limit reached. Logging out.\n");
            break;
        }
        printf("%s> ", username);
        fgets(command, 256, stdin);
        command[strcspn(command, "\n")] = 0;

        if (strcmp(command, "Time") == 0) {
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            printf("%02d:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
        } else if (strcmp(command, "Date") == 0) {
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            printf("%02d:%02d:%04d\n", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
        } else if (strcmp(command, "Logout") == 0) {
            break;
        } else if (strcmp(command, "Sanctions") == 0) {
            char target[256];
            int limit;
            printf("Enter target username: ");
            scanf("%s", target);
            printf("Enter new limit: ");
            scanf("%d", &limit);
            // Логика изменения лимита для другого пользователя
            // Важно валидацию команд и соответствующие проверки
            printf("Sanctions applied for %s with new limit %d\n", target, limit);
        } else {
            printf("Unknown command.\n");
        }
        count++;
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: child <username> <limit>\n");
        return 1;
    }
    char *username = argv[1];
    int limit = atoi(argv[2]);

    shell(username, limit);
    return 0;
}