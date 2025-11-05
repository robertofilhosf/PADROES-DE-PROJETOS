#ifndef SHA_FACADE_H
#define SHA_FACADE_H

#include <memory>
#include <mutex>
#include <thread>
#include <atomic>

class Window;

class SHAFacade {
    private:
        // Singleton
        static std::unique_ptr<SHAFacade> instance;
        static std::mutex instanceMutex;

        // Recursos internos
        std::unique_ptr<Window> m_window;
        std::thread m_threadExecucao;
        std::atomic<bool> m_executando;
        int m_intervaloTempoMs;
        bool m_geracaoImagemHabilitada;

        // Construtor privado
        SHAFacade();

    public:

        // Singleton
        static SHAFacade& getInstance();

        SHAFacade(const SHAFacade&) = delete;
        SHAFacade& operator=(const SHAFacade&) = delete;

        ~SHAFacade();

        // Funcionalidades 
        // Configura parâmetros iniciais do simulador
        void configSimuladorSHA(int intervaloTempoMs, bool gerarImagem = true, double vazaoInicial = 0.0);

        // Cria e inicia uma instância do SHA com parâmetros previamente configurados 
        void criaSHA();

        // Finaliza a instância em execução (caso haja)
        void finalizaSHA();

        // Modifica a vazão da instância em execução
        void modificaVazaoSHA(double novaVazao);

        // Habilita/desabilita geração de imagens da medição
        void habilitaGeracaoImagemSHA(bool habilitar);

        // Verificação de estado de execução
        bool estaExecutando() const;

        void alteraVazaoPorcentagem(double porcentagem);
};

#endif // SHA_FACADE_H