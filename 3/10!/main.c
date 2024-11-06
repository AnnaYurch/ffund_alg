#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M_P 1024

typedef struct TreeNode {
    char *data;
    struct TreeNode **children; //тут держим детей
    int child_count;
} TreeNode;

TreeNode* createNode(const char *data) {
    TreeNode *node = (TreeNode*)malloc(sizeof(TreeNode));
    if (node == NULL) {
        fprintf(stderr, "Memory yps1");
        return NULL;
    }

    node->data = strdup(data);
    if (node->data == NULL) {
        fprintf(stderr, "Memory yps2");
        free(node);
        return NULL;
    }

    node->children = NULL;
    node->child_count = 0;
    return node;
}

//закидываем еще детей
void addChild(TreeNode *parent, TreeNode *child) {
    TreeNode **newChildren = (TreeNode**)realloc(parent->children, sizeof(TreeNode*) * (parent->child_count + 1));
    if (newChildren == NULL) {
        fprintf(stderr, "Memory yps3");
        return;
    }
    parent->children = newChildren;
    parent->children[parent->child_count] = child;
    parent->child_count++;
}

const char* done_tree_Expression(const char *expr, TreeNode *parent) {
    char buffer[100];
    int i = 0;

    //читаем имя узла
    while (*expr != '\0' && *expr != '(' && *expr != ',' && *expr != ')') {
        buffer[i++] = *expr++;
    }
    buffer[i] = '\0';

    //у родителя появляется дитё 
    TreeNode *node = createNode(buffer);
    if (parent != NULL) {
        addChild(parent, node);
    }

    if (*expr == '(') {
        expr++;
        while (*expr != '\0' && *expr != ')') {
            expr = done_tree_Expression(expr, node);
            if (*expr == ',') {
                expr++;
            }
        }
        if (*expr == ')') {
            expr++;
        }
    }

    return expr;
}

void printTree(FILE *file, TreeNode *node, int level) {
    for (int i = 0; i < level; i++) {
        fprintf(file, "    ");
    }
    fprintf(file, "%s\n", node->data);

    for (int i = 0; i < node->child_count; i++) {
        printTree(file, node->children[i], level + 1);
    }
}

void freeTree(TreeNode *node) {
    for (int i = 0; i < node->child_count; i++) {
        freeTree(node->children[i]);
    }
    free(node->children);
    free(node->data);
    free(node);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "We need: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    char inputfilePath[M_P];
    char outputfilePath[M_P];

    if (realpath(argv[1], inputfilePath) == NULL) {
        fprintf(stderr, "Mistake with getting real path of input file");
        return 1;
    }

    if (realpath(argv[2], outputfilePath) == NULL) {
        fprintf(stderr, "Mistake with getting real path of output file\n");
        return 1;
    }

    if (strcmp(inputfilePath, outputfilePath) == 0) {
        fprintf(stderr, "Files are the same...it's promlem\n");
        return 1;
    }

    FILE *inputFile = fopen(argv[1], "r");
    if (inputFile == NULL) {
        fprintf(stderr, "Something wrong with opening input file\n");
        return 1;
    }

    FILE *outputFile = fopen(argv[2], "w");
    if (outputFile == NULL) {
        fprintf(stderr, "Something wrong with opening output file\n");
        fclose(inputFile);
        return 1;
    }

    char line[M_P];
    while (fgets(line, sizeof(line), inputFile)) {
        line[strcspn(line, "\n")] = '\0';

        TreeNode *root = createNode("");
        done_tree_Expression(line, root);

        fprintf(outputFile, "Tree for expression: %s\n", line);
        //fprintf(outputFile, "$");
        if (root->child_count > 0) {
            printTree(outputFile, root->children[0], 0);
        }
        fprintf(outputFile, "\n");

        freeTree(root);
    }

    fclose(inputFile);
    fclose(outputFile);
    return 0;
}