#include "sortderevo.h"

int main() {
    derevo_t derevo = {};
    SortDerevoInitialize(&derevo);
    SortDerevoPush(&derevo, 1);
    SortDerevoPush(&derevo, 4);
    SortDerevoPush(&derevo, 8);
    SortDerevoPush(&derevo, -8);
    derevo_node_t **node = SortDerevoPush(&derevo, 13);
    DerevoPopNode(&derevo, node);
    DerevoLog(&derevo);
    SortDerevoFPrint(&derevo, stdout);
    DerevoFinalize(&derevo);
}
