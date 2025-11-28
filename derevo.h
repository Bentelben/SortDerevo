#ifndef DEREVO_H
#define DEREVO_H

#include "logger.h"

#include <stdio.h>

typedef int derevo_elem_t;

struct derevo_node_t {
    derevo_elem_t value;
    derevo_node_t *left;
    derevo_node_t *right;
    derevo_node_t *parent;
};

typedef bool (*travesal_function_t)(derevo_node_t **, void *args);

struct derevo_t {
    derevo_node_t *head;
    logger_t logger;
    travesal_function_t elementValueDumpingTravesalFunctionPointer; // As args must accept file pointer
    travesal_function_t graphDataWritingTravesalFunctionPointer; // As args must accept file pointer
    travesal_function_t nodeFreeingTravesalFunctionPointer; // Must accept NULL args
};


void LogEvent(
    derevo_t *derevo,
    char const *name,
    char const *body
);

void DerevoInitialize(
    derevo_t *derevo,
    travesal_function_t elementValueDumpingTravesalFunctionPointer,
    travesal_function_t graphDataWritingTravesalFunctionPointer,
    travesal_function_t nodeFreeingTravesalFunctionPointer
);

derevo_node_t **DerevoInsert(derevo_t *derevo, derevo_node_t **destination, derevo_elem_t value);
derevo_node_t **DerevoInsertLeft(derevo_t *derevo, derevo_node_t *parent, derevo_elem_t value);
derevo_node_t **DerevoInsertRight(derevo_t *derevo, derevo_node_t *parent, derevo_elem_t value);

void DerevoPopNode(
    derevo_t *derevo,
    derevo_node_t **node
);

derevo_node_t *DerevoCopyNode(
    derevo_node_t *original
);

derevo_node_t **DerevoDoTravesal(
    derevo_node_t **derevo,
    travesal_function_t preorderFunctionPointer,
    travesal_function_t inorderFunctionPointer,
    travesal_function_t postorderFunctionPointer,
    void *args
);

void DerevoFinalize(
    derevo_t *derevo
);

void DerevoDump(
    derevo_t *derevo,
    FILE *file
);

void DerevoLog(
    derevo_t *derevo
);

#endif
