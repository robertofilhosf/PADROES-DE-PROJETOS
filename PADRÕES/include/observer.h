#ifndef OBSERVER_H
#define OBSERVER_H

typedef void (*Observer)(const char* mensagem);

void adicionarObserver(Observer obs);
void notificarObservers(const char* mensagem);

#endif
