#ifndef SORTDEREVO_H
#define SORTDEREVO_H

#include "derevo.h"

void SortDerevoInitialize(derevo_t *derevo);
derevo_node_t** SortDerevoPush(derevo_t *const derevo, int value);
void SortDerevoFPrint(derevo_t *derevo, FILE *file);

#endif
