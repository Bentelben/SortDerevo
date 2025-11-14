#include "sortderevo.h"

#include "derevo.h"

#include <stdio.h>
#include <assert.h>

static bool DumpValue(derevo_node_t **node, void *args);
static bool WriteNodeGraphData(derevo_node_t **node, void *args);
static bool SortPushLeftSelector(derevo_node_t **node, void *args);
static bool SortPushRightSelector(derevo_node_t **node, void *args);
static bool PrintValue(derevo_node_t **node, void *args);

static bool DumpValue(derevo_node_t **const node, void *const args) {
    assert(node);
    assert(args);

    FILE *const file = (FILE *)args;
    fprintf(file, "%d", (*node)->value);
    return true;
}

static bool WriteNodeGraphData(derevo_node_t **const node, void *const args) {
    assert(node);
    assert(args);

    FILE *const file = (FILE *)args;
    fprintf(file, "node_%p [shape=record, label=\"{ node %p | value = %d | { <left> left | <right> right } }\"]", *node, *node, (*node)->value);
    if ((*node)->left)
        fprintf(file, "node_%p:<left> -> node_%p\n", *node, (*node)->left);
    if ((*node)->right)
        fprintf(file, "node_%p:<right> -> node_%p\n", *node, (*node)->right);
    return true;
}


static bool SortPushLeftSelector(derevo_node_t **const node, void *const args) {
    assert(node);
    assert(args);

    int const pushValue = *(int *)args;
    return pushValue <= (*node)->value;
}

static bool SortPushRightSelector(derevo_node_t **const node, void *const args) {
    assert(node);
    assert(args);

    int const pushValue = *(int *)args;
    return pushValue > (*node)->value;
}

derevo_node_t** SortDerevoPush(derevo_t *const derevo, int value) {
    assert(derevo);

    derevo_node_t **const cursor = DerevoDoTravesal(
        &derevo->head,
        SortPushLeftSelector, &value,
        SortPushRightSelector, &value,
        NULL, NULL,
        NULL, NULL,
        NULL, NULL
    );

    return DerevoInsertNode(derevo, cursor, value);
}

void SortDerevoInitialize(derevo_t *const derevo) {
    assert(derevo);

    DerevoInitialize(derevo, DumpValue, WriteNodeGraphData, NULL);
}

static bool PrintValue(derevo_node_t **const node, void *const args) {
    assert(node);
    assert(args);

    FILE *const file = (FILE *)args;
    fprintf(file, "%d ", (*node)->value);
    return true;
}
void SortDerevoFPrint(derevo_t *const derevo, FILE *const file) {
    assert(derevo);
    assert(file);

    DerevoDoTravesal(
        &derevo->head,
        NULL, NULL,
        NULL, NULL,
        NULL, NULL,
        PrintValue, (void *)file,
        NULL, NULL
    );
    fprintf(file, "\n");
}
