#include "derevo.h"

#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static void LogEvent(derevo_t *derevo, char const *name, char const *body);

void DerevoInitialize(derevo_t *const derevo, travesal_function_t const elementValueDumpingTravesalFunctionPointer, travesal_function_t const graphDataWritingTravesalFunctionPointer) {
    assert(derevo);

    derevo->elementValueDumpingTravesalFunctionPointer = elementValueDumpingTravesalFunctionPointer; 
    derevo->graphDataWritingTravesalFunctionPointer = graphDataWritingTravesalFunctionPointer;

    LoggerInitialize(&derevo->logger);
    LogEvent(derevo, "Intialize", "");
}

derevo_node_t** DerevoPushNode(derevo_t *const derevo, derevo_node_t **const destination, derevo_elem_t value) {
    assert(derevo);
    assert(destination);

    LogEvent(derevo, "Before PUSH left", "");

    assert(*destination == NULL);
    *destination = (derevo_node_t *)calloc(1, sizeof(**destination));
    if (*destination == NULL)
        return NULL;
    
    (*destination)->value = value;
    (*destination)->left = NULL;
    (*destination)->right = NULL;

    LogEvent(derevo, "After PUSH left", "");
    return destination;
}

static void DerevoFreeNode(derevo_node_t *const node) {
    if (node == NULL)
        return;
    derevo_node_t *const left = node->left;
    derevo_node_t *const right = node->right;
    free(node);
    DerevoFreeNode(left); // TODO maybe do with travesal
    DerevoFreeNode(right);
}

void DerevoPopNode(derevo_t *const derevo, derevo_node_t **const node) {
    LogEvent(derevo, "Before POP", "");
    DerevoFreeNode(*node);
    *node = NULL;
    LogEvent(derevo, "After POP", "");
}

static derevo_node_t **Travesal(
    derevo_node_t **const node,
    travesal_function_t const leftSelectorFunctionPointer,  void *const leftSelectorArgs,
    travesal_function_t const rightSelectorFunctionPointer, void *const rightSelectorArgs,
    travesal_function_t const preorderFunctionPointer,      void *const preorderArgs,
    travesal_function_t const inorderFunctionPointer,       void *const inorderArgs,
    travesal_function_t const postorderFunctionPointer,     void *const postorderArgs
) {
    if (*node == NULL)
        return node;

    if (preorderFunctionPointer != NULL && !preorderFunctionPointer(node, preorderArgs))
        return node;

    bool const shouldGoLeft = leftSelectorFunctionPointer == NULL ? (*node)->left != NULL : leftSelectorFunctionPointer(node, leftSelectorArgs);
    if (shouldGoLeft) {
        derevo_node_t **leftResult = Travesal(
            &(*node)->left,
            leftSelectorFunctionPointer, leftSelectorArgs,
            rightSelectorFunctionPointer, rightSelectorArgs,
            preorderFunctionPointer, preorderArgs,
            inorderFunctionPointer, inorderArgs,
            postorderFunctionPointer, postorderArgs
        );
        if (leftResult != NULL)
            return leftResult;
    }

    if (inorderFunctionPointer != NULL && !inorderFunctionPointer(node, inorderArgs))
        return node;

    bool const shouldGoRight = rightSelectorFunctionPointer == NULL ? (*node)->right != NULL : rightSelectorFunctionPointer(node, rightSelectorArgs);
    if (shouldGoRight) {
        derevo_node_t **rightResult = Travesal(
            &(*node)->right,
            leftSelectorFunctionPointer, leftSelectorArgs,
            rightSelectorFunctionPointer, rightSelectorArgs,
            preorderFunctionPointer, preorderArgs,
            inorderFunctionPointer, inorderArgs,
            postorderFunctionPointer, postorderArgs
        );
        if (rightResult != NULL)
            return rightResult;
    }

    if (postorderFunctionPointer != NULL && !postorderFunctionPointer(node, postorderArgs))
        return node;

    return NULL;
}

derevo_node_t **DerevoDoTravesal(
    derevo_t *const derevo,
    travesal_function_t const leftSelectorFunctionPointer,  void *const leftSelectorArgs,
    travesal_function_t const rightSelectorFunctionPointer, void *const rightSelectorArgs,
    travesal_function_t const preorderFunctionPointer,      void *const preorderArgs, 
    travesal_function_t const inorderFunctionPointer,       void *const inorderArgs,
    travesal_function_t const postorderFunctionPointer,     void *const postorderArgs
) {
    return Travesal(
        &derevo->head,
        leftSelectorFunctionPointer, leftSelectorArgs,
        rightSelectorFunctionPointer, rightSelectorArgs,
        preorderFunctionPointer, preorderArgs,
        inorderFunctionPointer, inorderArgs,
        postorderFunctionPointer, postorderArgs
    );
}

void DerevoFinalize(derevo_t *const derevo) {
    LogEvent(derevo, "Finalize", "");
    DerevoFreeNode(derevo->head);
}


// Logging

static void WriteGraphData(derevo_t *const derevo, FILE *const file) {
    fprintf(file, "digraph {\n");
    DerevoDoTravesal(derevo, NULL, NULL, NULL, NULL, derevo->graphDataWritingTravesalFunctionPointer, (void *)file, NULL, NULL, NULL, NULL);
    fprintf(file, "}");
}


struct derevo_dump_preorder_travesal_function_args {
    FILE *file;
    travesal_function_t elementValueDumpingTravesalFunctionPointer;
};

static bool DerevoDumpPreorderTravesalFunction(derevo_node_t **node, void *rawArgs) {
    derevo_dump_preorder_travesal_function_args *args = (derevo_dump_preorder_travesal_function_args *)rawArgs; 
    fprintf(args->file, "(");
    args->elementValueDumpingTravesalFunctionPointer(node, args->file);
    fprintf(args->file, " ");
    return true;
}

static bool DerevoDumpInorderTravesalFunction(derevo_node_t **node, void *args) {
    FILE *file = (FILE *)args;
    if ((*node)->left == NULL)
        fprintf(file, "() ");
    if ((*node)->right == NULL)
        fprintf(file, "()");
    return true;
}

static bool DerevoDumpPostorderTravesalFunction(derevo_node_t **, void *args) {
    FILE *file = (FILE *)args;
    fprintf(file, ")");
    return true;
}

static void LogEvent(derevo_t *const derevo, char const *const name, char const *const body) {
    static size_t id = 0;

    {
        char buffer[1024] = "";
        snprintf(buffer, 1024, "%s/graph_%zu.gv", derevo->logger.dirPath, id);
        FILE *file = fopen(buffer, "w");
        WriteGraphData(derevo, file);
        fclose(file);
    }

    {
        char buffer[2048] = "";
        snprintf(buffer, 2048, "dot -Tpng %s/graph_%zu.gv -o %s/photo_%zu.png", derevo->logger.dirPath, id, derevo->logger.dirPath, id);
        system(buffer);
    }

    FILE *file = LoggerStartEvent(derevo->logger, name);
    fprintf(file, "%s\n", body);

    {
        derevo_dump_preorder_travesal_function_args preorderArgs = {
            file,
            derevo->elementValueDumpingTravesalFunctionPointer
        };
        DerevoDoTravesal(
            derevo, 
            NULL, NULL, 
            NULL, NULL, 
            DerevoDumpPreorderTravesalFunction, (void *)&preorderArgs, 
            DerevoDumpInorderTravesalFunction, (void *)file, 
            DerevoDumpPostorderTravesalFunction, (void *)file
       );
    }

    fprintf(file, "\n");
    
    fprintf(file, "<img src=\"%s/photo_%zu.png\">", derevo->logger.dirPath, id);
    LoggerEndEvent(derevo->logger, file);
    id++;
}

void DerevoDump(derevo_t *const derevo) {
    LogEvent(derevo, "dump", "");
}
