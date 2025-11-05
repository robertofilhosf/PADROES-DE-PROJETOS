#include "SHAFacade.h"
#include "Window.h"
#include <iostream>
#include <algorithm>

std::unique_ptr<SHAFacade> SHAFacade::instance;
std::mutex SHAFacade::instanceMutex;

SHAFacade::SHAFacade()
    : m_executando(false),
      m_intervaloTempoMs(1000),
      m_geracaoImagemHabilitada(true) {}

SHAFacade::~SHAFacade() {
    finalizaSHA();
}

SHAFacade& SHAFacade::getInstance() {
    std::lock_guard<std::mutex> lock(instanceMutex);
    if (!instance) {
        instance.reset(new SHAFacade());
    }
    return *instance;
}

void SHAFacade::configSimuladorSHA(int intervaloTempoMs, bool gerarImagem, double /*vazaoInicial*/) {
    m_intervaloTempoMs = intervaloTempoMs;
    m_geracaoImagemHabilitada = gerarImagem;
}

void SHAFacade::criaSHA() {
    if (m_executando) return;

    m_executando = true;
    m_window = std::make_unique<Window>();
    m_window->setIntervaloTempo(m_intervaloTempoMs);
    m_window->setGerarImagens(m_geracaoImagemHabilitada);

    // Executa GUI em thread separada
    m_threadExecucao = std::thread([this]() {
        if (m_window->create()) {
            m_window->iniciarSimuladores();
            m_window->run();
        }
        m_executando = false; // GUI fechada → estado resetado
    });

    m_threadExecucao.detach();
}

void SHAFacade::finalizaSHA() {
    if (!m_executando || !m_window) return;
    m_window->finalizar();
    m_executando = false;
}

void SHAFacade::modificaVazaoSHA(double novaVazao) {
    if (m_window) {
        m_window->modificaVazao(novaVazao);
    }
}

void SHAFacade::habilitaGeracaoImagemSHA(bool habilitar) {
    m_geracaoImagemHabilitada = habilitar;
    if (m_window) m_window->setGerarImagens(habilitar);
}

bool SHAFacade::estaExecutando() const {
    return m_executando;
}

void SHAFacade::alteraVazaoPorcentagem(double porcentagem) {
    if (!m_window) {
        MessageBoxA(nullptr, "Nenhum simulador ativo!", "Erro", MB_ICONERROR);
        return;
    }

    porcentagem = std::clamp(porcentagem, 0.0, 100.0);

    double vazaoMaxima = 1.0;
    double novaVazao = (porcentagem/100.0) * vazaoMaxima;

    m_window->setVazaoPrincipal(novaVazao);
    m_window->forcarRedesenho();

    std::cout << "Vazao ajustada para " << porcentagem << "% (" << novaVazao << " m³/s)" << std::endl;
}