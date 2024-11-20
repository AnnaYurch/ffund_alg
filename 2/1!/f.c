char* HandlerOptC(int argc, char *argv[], int seed) {
    int count = argc;
    char **new_strs = (char **)malloc(count * sizeof(char *));
    if (new_strs == NULL) {
        printf("Memory yps3");
        return NULL;
    }
    
    for (int i = 0; i < count; i++) {
        new_strs[i] = argv[i];
    }
 
    srand(seed);

    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        char *temp = new_strs[i];
        new_strs[i] = new_strs[j];
        new_strs[j] = temp;
    }
    
    int total_l = 0;
    for (int i = 0; i < count; i++) {
        total_l += my_strlen(new_strs[i]);
    }

    char *result = (char *)malloc(total_l + 1);
    if (result == NULL) {
        printf("Memory yps4");
        free(new_strs);
        return NULL;
    }
    
    result[0] = '\0';

    for (int i = 0; i < count; i++) {
        char *new_res = my_strcat(result, new_strs[i]);
        free(result);
        result = new_res;
    }
    

    free(new_strs);
    return result;
}

