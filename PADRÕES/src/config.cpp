#include "config.h"
#include <stdlib.h>

static Config* instancia = NULL;

Config* getConfig(void) {
    if (instancia == NULL) {
        instancia = (Config*) malloc(sizeof(Config));
        instancia->modo_debug = 1;
    }
    return instancia;
}
