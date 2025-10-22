#include "SimuladorFacade.h"

SimuladorFacade::SimuladorFacade() {
    m_window = std::make_unique<Window>();
}

SimuladorFacade::~SimuladorFacade() = default;

bool SimuladorFacade::inicializar() {
    return m_window->create();
}

void SimuladorFacade::executar() {
    m_window->iniciarSimuladores();
    m_window->run();
}

void SimuladorFacade::atualizarVazao(double vazao) {
    m_window->setVazaoPrincipal(vazao);
}

void SimuladorFacade::atualizarTela() {
    m_window->forcarRedesenho();
}