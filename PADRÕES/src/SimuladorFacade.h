#ifndef SIMULADORFACADE_h
#define SIMULADORFACADE_h

#include <memory>
#include "Window.h"

class SimuladorFacade {
    public:
        SimuladorFacade();
        ~SimuladorFacade();

        // Inicializa todos os componentes
        bool inicializar();

        // Inicia os simuladores e entra no loop principal
        void executar();

        // Atualiza a vazão do hidrômetro principal
        void atualizarVazao(double vazao);

        // Força a atualização da tela
        void atualizarTela();

    private:
        std::unique_ptr<Window> m_window;
};

#endif // SIMULADORFACADE_h