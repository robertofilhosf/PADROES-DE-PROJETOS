#ifndef STRATEGY_H
#define STRATEGY_H

typedef float (*CalculoStrategy)(float valor);

float calculoSimples(float valor);
float calculoAvancado(float valor);

#endif
