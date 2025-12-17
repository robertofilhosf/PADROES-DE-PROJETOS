#include "sistema_facade.h"
#include "../repository/usuario_repository.h"
#include "../strategy/strategy.h"
#include "../observer/observer.h"
#include <string.h>
#include <stdio.h>

void cadastrarUsuarioFacade(int id, const char* nome) {
    Usuario u;
    u.id = id;
    strncpy(u.nome, nome, sizeof(u.nome));
    salvarUsuario(u);
    notificarObservers("Observer: Usuário cadastrado com sucesso");
}

void calcularFacade(float valor, int tipo) {
    CalculoStrategy strategy;

    if (tipo == 1) {
        strategy = calculoSimples;
    } else {
        strategy = calculoAvancado;
    }

    float resultado = strategy(valor);

    char msg[60];
    snprintf(msg, sizeof(msg), "Observer: Resultado do cálculo = %.2f", resultado);
    notificarObservers(msg);
}
