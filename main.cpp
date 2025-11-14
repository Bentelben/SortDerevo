#include "sortderevo.h"

int main() {
    derevo_t derevo = {};
    SortDerevoInitialize(&derevo);
    SortDerevoInsert(&derevo, 1);
    SortDerevoInsert(&derevo, 4);
    SortDerevoInsert(&derevo, 8);
    SortDerevoInsert(&derevo, -8);
    derevo_node_t **node = SortDerevoInsert(&derevo, 13);
    DerevoPopNode(&derevo, node);
    DerevoLog(&derevo);
    SortDerevoFPrint(&derevo, stdout);
    DerevoFinalize(&derevo);
}
