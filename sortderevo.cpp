#include "sortderevo.h"

#include "derevo.h"

#include <stdio.h>
#include <assert.h>

static bool DumpValue(derevo_node_t **const node, void *const args) {
    FILE *const file = (FILE *)args;
    fprintf(file, "%d", (*node)->value);
    return true;
}

static bool WriteNodeGraphData(derevo_node_t **const node, void *const args) {
    FILE *const file = (FILE *)args;
    fprintf(file, "node_%p [shape=record, label=\"{ node %p | value = %d | { <left> left | <right> right } }\"]", *node, *node, (*node)->value);
    if ((*node)->left)
        fprintf(file, "node_%p:<left> -> node_%p\n", *node, (*node)->left);
    if ((*node)->right)
        fprintf(file, "node_%p:<right> -> node_%p\n", *node, (*node)->right);
    return true;
}


static bool SortPushLeftSelector(derevo_node_t **const node, void *const args) {
    int pushValue = *(int *)args;
    return pushValue <= (*node)->value;
}

static bool SortPushRightSelector(derevo_node_t **const node, void *const args) {
    int pushValue = *(int *)args;
    return pushValue > (*node)->value;
}

derevo_node_t** SortDerevoPush(derevo_t *const derevo, int value) {
    assert(derevo);

    derevo_node_t **cursor = DerevoDoTravesal(
        derevo,
        SortPushLeftSelector, &value,
        SortPushRightSelector, &value,
        NULL, NULL,
        NULL, NULL,
        NULL, NULL
    );

    return DerevoPushNode(derevo, cursor, value);
}

void SortDerevoInitialize(derevo_t *const derevo) {
    DerevoInitialize(derevo, DumpValue, WriteNodeGraphData);
}

static bool PrintValue(derevo_node_t **const node, void *const args) {
    FILE *const file = (FILE *)args;
    fprintf(file, "%d ", (*node)->value);
    return true;
}
void SortDerevoFPrint(derevo_t *const derevo, FILE *const file) {
    DerevoDoTravesal(
        derevo,
        NULL, NULL,
        NULL, NULL,
        NULL, NULL,
        PrintValue, (void *)file,
        NULL, NULL
    );
    fprintf(file, "\n");
}
