#include <stdio.h>
#include "singleton/config.h"
#include "observer/observer.h"
#include "facade/sistema_facade.h"

void consoleObserver(const char* msg) {
    printf("%s\n", msg);
}

int main() {

    Config* config = getConfig();
    printf("Sistema iniciado | Debug: %d\n\n", config->modo_debug);

    adicionarObserver(consoleObserver);

    cadastrarUsuarioFacade(1, "Jose Roberto");
    calcularFacade(100.0f, 2);

    return 0;
}
