#ifndef USUARIO_REPOSITORY_H
#define USUARIO_REPOSITORY_H

typedef struct {
    int id;
    char nome[50];
} Usuario;

void salvarUsuario(Usuario u);
Usuario buscarUsuario(int id);

#endif
