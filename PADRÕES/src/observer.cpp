#include "observer.h"
#include <stdio.h>

#define MAX_OBSERVERS 10

static Observer observers[MAX_OBSERVERS];
static int totalObservers = 0;

void adicionarObserver(Observer obs) {
    if (totalObservers < MAX_OBSERVERS) {
        observers[totalObservers++] = obs;
    }
}

void notificarObservers(const char* mensagem) {
    for (int i = 0; i < totalObservers; i++) {
        observers[i](mensagem);
    }
}
