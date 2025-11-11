#ifndef DEREVO_H
#define DEREVO_H

#include "logger.h"

#include <stdio.h>

typedef int derevo_elem_t;

struct derevo_node_t {
    derevo_elem_t value;
    derevo_node_t *left;
    derevo_node_t *right;
};

typedef bool (*travesal_function_t)(derevo_node_t **, void *args);

struct derevo_t {
    derevo_node_t *head;
    logger_t logger;
    travesal_function_t elementValueDumpingTravesalFunctionPointer; // As args must accept file pointer
    travesal_function_t graphDataWritingTravesalFunctionPointer; // As args must accept file pointer
};


void DerevoInitialize(
    derevo_t *derevo,
    travesal_function_t elementValueDumpingTravesalFunctionPointer,
    travesal_function_t graphDataWritingTravesalFunctionPointer
);

derevo_node_t** DerevoPushNode(
    derevo_t *derevo,
    derevo_node_t **destination,
    derevo_elem_t value
);

void DerevoPopNode(
    derevo_t *derevo,
    derevo_node_t **node
);

derevo_node_t **DerevoDoTravesal(
    derevo_t *derevo,
    travesal_function_t leftSelectorFunctionPointer,  void *leftSelectorArgs,
    travesal_function_t rightSelectorFunctionPointer, void *rightSelectorArgs,
    travesal_function_t preorderFunctionPointer,      void *preorderArgs, 
    travesal_function_t inorderFunctionPointer,       void *inorderArgs,
    travesal_function_t postorderFunctionPointer,     void *postorderArgs
);

void DerevoFinalize(
    derevo_t *derevo
);

void DerevoDump(
    derevo_t *derevo
);

#endif
