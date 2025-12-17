#include "usuario_repository.h"
#include <stdio.h>

void salvarUsuario(Usuario u) {
    printf("Repository: Usuário salvo -> ID: %d | Nome: %s\n", u.id, u.nome);
}

Usuario buscarUsuario(int id) {
    Usuario u;
    u.id = id;
    snprintf(u.nome, sizeof(u.nome), "Usuário %d", id);
    return u;
}
