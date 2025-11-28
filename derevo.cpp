#include "derevo.h"

#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct derevo_dump_travesal_function_args_t {
    FILE *file;
    travesal_function_t elementValueDumpingTravesalFunctionPointer;
};

static derevo_node_t *DerevoAllocateNode(derevo_elem_t value);
static bool DerevoFreeNodePostorderTravesalFunction(derevo_node_t **node, void *args);
static void DerevoFreeNode(derevo_t *derevo, derevo_node_t **node);
static int WriteGraphData(derevo_t *derevo, size_t id);
static bool DerevoDumpPreorderTravesalFunction(derevo_node_t **node, void *rawArgs);
static bool DerevoDumpInorderTravesalFunction(derevo_node_t **node, void *args);
static bool DerevoDumpPostorderTravesalFunction(derevo_node_t **, void *args);
static int GenerateGraphImage(derevo_t *derevo, size_t id);

void DerevoInitialize(
    derevo_t *const derevo,
    travesal_function_t const elementValueDumpingTravesalFunctionPointer,
    travesal_function_t const graphDataWritingTravesalFunctionPointer,
    travesal_function_t const nodeFreeingTravesalFunctionPointer
) {
    assert(derevo);

    derevo->elementValueDumpingTravesalFunctionPointer = elementValueDumpingTravesalFunctionPointer; 
    derevo->graphDataWritingTravesalFunctionPointer = graphDataWritingTravesalFunctionPointer;
    derevo->nodeFreeingTravesalFunctionPointer = nodeFreeingTravesalFunctionPointer;


    LoggerInitialize(&derevo->logger);
    LogEvent(derevo, "Intialize", "");
}

static derevo_node_t *DerevoAllocateNode(derevo_elem_t value) {
    derevo_node_t *result = (derevo_node_t *)calloc(1, sizeof(derevo_node_t));
    if (result == NULL)
        return NULL;

    result->value = value;
    return result;
}

derevo_node_t **DerevoInsert(derevo_t *const derevo, derevo_node_t **const destination, derevo_elem_t const value) {
    assert(derevo);
    assert(destination);

    if (*destination != NULL) {
        LogEvent(derevo, "INSERT FAILED", "destination already has node");
        return NULL;
    }

    LogEvent(derevo, "Before INSERT", "");

    *destination = DerevoAllocateNode(value);
    if (*destination == NULL)
        return NULL;

    LogEvent(derevo, "After INSERT", "");
    return destination;
}

derevo_node_t **DerevoInsertLeft(derevo_t *const derevo, derevo_node_t *const parent, derevo_elem_t const value) {
    assert(derevo);
    assert(parent);

    if (DerevoInsert(derevo, &parent->left, value) == NULL)
        return NULL;
    parent->left->parent = parent;

    return &parent->left;
}

derevo_node_t **DerevoInsertRight(derevo_t *const derevo, derevo_node_t *const parent, derevo_elem_t const value) {
    assert(derevo);
    assert(parent);

    if (DerevoInsert(derevo, &parent->right, value) == NULL)
        return NULL;
    parent->right->parent = parent;

    return &parent->right;
}

static bool DerevoFreeNodePostorderTravesalFunction(derevo_node_t **const node, void *const args) {
    assert(node);
    assert(args);

    derevo_t const *derevo = (derevo_t *)args;
    if (derevo->nodeFreeingTravesalFunctionPointer != NULL)
        derevo->nodeFreeingTravesalFunctionPointer(node, NULL);
    free(*node);
    *node = NULL;
    return true;
}

static void DerevoFreeNode(derevo_t *const derevo, derevo_node_t **const node) {
    assert(derevo);
    assert(node);

    DerevoDoTravesal(
        node,
        NULL,
        NULL,
        DerevoFreeNodePostorderTravesalFunction,
        (void *)derevo
    );
}

void DerevoPopNode(derevo_t *const derevo, derevo_node_t **const node) {
    assert(derevo);
    assert(node);

    LogEvent(derevo, "Before POP", "");
    DerevoFreeNode(derevo, node);
    LogEvent(derevo, "After POP", "");
}

derevo_node_t *DerevoCopyNode(derevo_node_t *const original) {
    assert(original);

    derevo_node_t *const copy = (derevo_node_t *)calloc(1, sizeof(*copy));
    copy->value = original->value;

    if (original->left != NULL)
        copy->left = DerevoCopyNode(original->left);
    if (original->right != NULL)
        copy->right = DerevoCopyNode(original->right);

    return copy;
}

derevo_node_t **DerevoDoTravesal(
    derevo_node_t **const node,
    travesal_function_t const preorderFunctionPointer,
    travesal_function_t const inorderFunctionPointer,
    travesal_function_t const postorderFunctionPointer,
    void *const args
) {
    if (*node == NULL)
        return node;

    if (preorderFunctionPointer != NULL && !preorderFunctionPointer(node, args))
        return node;

    if ((*node)->left != NULL) {
        derevo_node_t **leftResult = DerevoDoTravesal(
            &(*node)->left,
            preorderFunctionPointer,
            inorderFunctionPointer,
            postorderFunctionPointer,
            args
        );
        if (leftResult != NULL)
            return leftResult;
    }

    if (inorderFunctionPointer != NULL && !inorderFunctionPointer(node, args))
        return node;

    if ((*node)->right != NULL) {
        derevo_node_t **rightResult = DerevoDoTravesal(
            &(*node)->right,
            preorderFunctionPointer,
            inorderFunctionPointer,
            postorderFunctionPointer,
            args
        );
        if (rightResult != NULL)
            return rightResult;
    }

    if (postorderFunctionPointer != NULL && !postorderFunctionPointer(node, args))
        return node;

    return NULL;
}

void DerevoFinalize(derevo_t *const derevo) {
    assert(derevo);

    LogEvent(derevo, "Finalize", "");
    DerevoFreeNode(derevo, &derevo->head);
}


// Logging

static bool DerevoDumpPreorderTravesalFunction(derevo_node_t **const node, void *const rawArgs) {
    assert(node);
    assert(rawArgs);

    derevo_dump_travesal_function_args_t *const args = (derevo_dump_travesal_function_args_t *)rawArgs; 
    fprintf(args->file, "(");
    args->elementValueDumpingTravesalFunctionPointer(node, args->file);
    fprintf(args->file, " ");
    return true;
}

static bool DerevoDumpInorderTravesalFunction(derevo_node_t **const node, void *const rawArgs) {
    assert(node);
    assert(rawArgs);

    derevo_dump_travesal_function_args_t *const args = (derevo_dump_travesal_function_args_t *)rawArgs; 
    if ((*node)->left == NULL)
        fprintf(args->file, "()");
    fprintf(args->file, " ");
    if ((*node)->right == NULL)
        fprintf(args->file, "()");
    return true;
}

static bool DerevoDumpPostorderTravesalFunction(derevo_node_t **, void *const rawArgs) {
    assert(rawArgs);

    derevo_dump_travesal_function_args_t *const args = (derevo_dump_travesal_function_args_t *)rawArgs; 
    fprintf(args->file, ")");
    return true;
}

void DerevoDump(derevo_t *const derevo, FILE *const file) {
    assert(derevo);
    assert(file);

    derevo_dump_travesal_function_args_t args = {
        file,
        derevo->elementValueDumpingTravesalFunctionPointer
    };
    DerevoDoTravesal(
        &derevo->head, 
        DerevoDumpPreorderTravesalFunction,
        DerevoDumpInorderTravesalFunction,
        DerevoDumpPostorderTravesalFunction,
        &args
   );
}

static int WriteGraphData(derevo_t *const derevo, size_t const id) {
    assert(derevo);

    char buffer[1024] = "";
    snprintf(buffer, 1024, "%s/graph_%zu.gv", derevo->logger.dirPath, id);
    FILE *file = fopen(buffer, "w");
    if (file == NULL)
        return -1;

    fprintf(file, "digraph {\n");
    DerevoDoTravesal(
        &derevo->head, 
        derevo->graphDataWritingTravesalFunctionPointer, 
        NULL,
        NULL,
        (void *)file
    );
    fprintf(file, "}");

    fclose(file);
    return 0;
}

static int GenerateGraphImage(derevo_t *const derevo, size_t const id) {
    char buffer[2048] = "";
    snprintf(buffer, 2048, "dot -Tpng %s/graph_%zu.gv -o %s/photo_%zu.png", derevo->logger.dirPath, id, derevo->logger.dirPath, id);
    return system(buffer);
}

void LogEvent(derevo_t *const derevo, char const *const name, char const *const body) {
    assert(derevo);
    assert(name);
    assert(body);

    static size_t id = 0;

    FILE *file = LoggerStartEvent(derevo->logger, name);
    if (file == NULL) {
        fprintf(stderr, "ERROR: Unable to log\n");
        return;
    }
    fprintf(file, "%s\n", body);

    if (WriteGraphData(derevo, id) == -1) {
        fprintf(file, "ERROR: Unable to write graph data\n");
        return;
    }

    if (GenerateGraphImage(derevo, id) == -1) {
        fprintf(file, "EROROR: Unable to generate graph image\n");
        return;
    }
    
    DerevoDump(derevo, file);
    fprintf(file, "\n");
    fprintf(file, "<img src=\"%s/photo_%zu.png\">", derevo->logger.dirPath, id);
    LoggerEndEvent(derevo->logger, file);
    id++;
}

void DerevoLog(derevo_t *const derevo) {
    assert(derevo);
    LogEvent(derevo, "log", "");
}
