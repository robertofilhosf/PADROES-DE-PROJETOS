// hidrometro.cpp
#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QSpinBox>
#include <cmath>
#include <iomanip>
#include <sstream>

// --------- Modelo (classes) ----------
class Fluxo {
public:
    Fluxo(double vazao = 0.0, double pressao = 0.0, double volume = 0.0)
        : vazao_(vazao), pressao_(pressao), volume_total_(volume) {}
    double getVazao() const { return vazao_; }
    void setVazao(double v) { vazao_ = v; }
    double getPressao() const { return pressao_; }
    void setPressao(double p) { pressao_ = p; }
    double getVolumeTotal() const { return volume_total_; }
    void setVolumeTotal(double v) { volume_total_ = v; }
    void incrementarVolume(double tempoHoras) { volume_total_ += vazao_ * tempoHoras; }
private:
    double vazao_;
    double pressao_;
    double volume_total_;
};

class Medicao {
public:
    Medicao() : consumo_m3(0), centenas_litros(0), dezenas_litros(0), litros(0), decimos_litros(0) {}
    void atualizarMedicao(double volume_total) {
        consumo_m3 = static_cast<int>(std::floor(volume_total));
        double parte_decimal = volume_total - consumo_m3;
        double litros_total = parte_decimal * 1000.0; // 1 m3 = 1000 litros

        centenas_litros = (static_cast<int>(litros_total) / 100) % 10;
        dezenas_litros  = (static_cast<int>(litros_total) / 10) % 10;
        litros          = static_cast<int>(litros_total) % 10;
        decimos_litros  = (static_cast<int>(litros_total * 10)) % 10;
    }

    int getConsumoM3() const { return consumo_m3; }
    int getCentenasLitros() const { return centenas_litros; }
    int getDezenasLitros() const { return dezenas_litros; }
    int getLitros() const { return litros; }
    int getDecimosLitros() const { return decimos_litros; }

    static double calcularConsumo(double leitura_anterior, double leitura_atual) {
        return leitura_atual - leitura_anterior;
    }

private:
    int consumo_m3;
    int centenas_litros;
    int dezenas_litros;
    int litros;
    int decimos_litros;
};

class Hidrometro {
public:
    Hidrometro() : entrada(), saida(), medicao(), leitura_anterior(0.0) {}
    void medir() {
        medicao.atualizarMedicao(entrada.getVolumeTotal());
    }
    void setVazaoEntrada(double v) { entrada.setVazao(v); }
    void atualizarVolume(double tempo_horas) { entrada.incrementarVolume(tempo_horas); }
    double getVolumeTotal() const { return entrada.getVolumeTotal(); }
    double getConsumo() const { return Medicao::calcularConsumo(leitura_anterior, entrada.getVolumeTotal()); }
    void setLeituraAnterior(double l) { leitura_anterior = l; }
    const Medicao& getMedicao() const { return medicao; }
    const Fluxo& getEntrada() const { return entrada; }
private:
    Fluxo entrada;
    Fluxo saida;
    Medicao medicao;
    double leitura_anterior;
};

// --------- Widget de mostradores customizados ----------
class DialWidget : public QWidget {
    Q_OBJECT
public:
    DialWidget(QWidget* parent=nullptr) : QWidget(parent), value(0) {
        setMinimumSize(140,140);
    }
    void setValue(int v) { value = v % 10; update(); }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        int w = width(), h = height();
        int cx = w/2, cy = h/2;
        int r = qMin(w,h)/2 - 10;

        // círculo externo
        p.setPen(Qt::black);
        p.drawEllipse(cx-r, cy-r, 2*r, 2*r);

        // números 0-9
        for (int i=0;i<10;i++){
            double ang = (i * 36.0 - 90.0) * M_PI/180.0;
            int x = static_cast<int>(cx + (r-20) * cos(ang));
            int y = static_cast<int>(cy + (r-20) * sin(ang));
            p.drawText(x-8, y+6, QString::number(i));
        }

        // ponteiro
        double angv = (value * 36.0 - 90.0) * M_PI/180.0;
        int x2 = static_cast<int>(cx + (r-35) * cos(angv));
        int y2 = static_cast<int>(cy + (r-35) * sin(angv));
        QPen pen(Qt::red, 4);
        p.setPen(pen);
        p.drawLine(cx,cy,x2,y2);
    }
private:
    int value;
};

// --------- Janela principal (Controller + View) ----------
class HidrometroWindow : public QMainWindow {
    Q_OBJECT
public:
    HidrometroWindow(QWidget* parent=nullptr) : QMainWindow(parent), hidrometro() {
        QWidget* central = new QWidget;
        setCentralWidget(central);
        setWindowTitle(QStringLiteral("💧 Simulador de Hidrômetro"));
        resize(1150, 900);

        QVBoxLayout* mainLayout = new QVBoxLayout;
        central->setLayout(mainLayout);

        QLabel* title = new QLabel(QStringLiteral("Simulação de Hidrômetro Analógico"));
        QFont f = title->font();
        f.setPointSize(16);
        f.setBold(true);
        title->setFont(f);
        title->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(title);

        // Controles
        QGroupBox* controlBox = new QGroupBox(QStringLiteral("⚙️ Controles"));
        QHBoxLayout* cLayout = new QHBoxLayout;
        controlBox->setLayout(cLayout);

        // Vazão
        QVBoxLayout* vazaoLayout = new QVBoxLayout;
        QLabel* vazaoLabel = new QLabel(QStringLiteral("Vazão (m³/h):"));
        vazaoLayout->addWidget(vazaoLabel);
        vazaoSlider = new QSlider(Qt::Horizontal);
        vazaoSlider->setRange(0, 1000); // 0.00 .. 10.00
        vazaoSlider->setValue(150); // 1.50
        vazaoLayout->addWidget(vazaoSlider);
        vazaoValueLabel = new QLabel("1.50 m³/h");
        vazaoLayout->addWidget(vazaoValueLabel);
        cLayout->addLayout(vazaoLayout);

        // Leitura anterior
        QVBoxLayout* leituraLayout = new QVBoxLayout;
        QLabel* leituraLabel = new QLabel(QStringLiteral("Leitura anterior (m³):"));
        leituraLayout->addWidget(leituraLabel);
        leituraSpin = new QDoubleSpinBox;
        leituraSpin->setRange(0, 100000);
        leituraSpin->setDecimals(3);
        leituraSpin->setValue(0.0);
        leituraLayout->addWidget(leituraSpin);
        cLayout->addLayout(leituraLayout);

        // Botões
        QVBoxLayout* botoesLayout = new QVBoxLayout;
        startButton = new QPushButton(QStringLiteral("▶ Iniciar"));
        stopButton  = new QPushButton(QStringLiteral("⏹ Parar"));
        resetButton = new QPushButton(QStringLiteral("🔄 Reiniciar"));
        stopButton->setEnabled(false);
        botoesLayout->addWidget(startButton);
        botoesLayout->addWidget(stopButton);
        botoesLayout->addWidget(resetButton);
        cLayout->addLayout(botoesLayout);

        mainLayout->addWidget(controlBox);

        // Mostrador principal (widget de desenho)
        displayWidget = new QWidget;
        displayWidget->setMinimumHeight(260);
        mainLayout->addWidget(displayWidget);

        // Ponteiros analógicos
        QGroupBox* ponteirosBox = new QGroupBox(QStringLiteral("⏱ Ponteiros Analógicos (Litros e Décimos)"));
        QHBoxLayout* pLayout = new QHBoxLayout;
        ponteirosBox->setLayout(pLayout);

        litrosDial = new DialWidget;
        decimosDial = new DialWidget;
        pLayout->addWidget(litrosDial);
        pLayout->addWidget(decimosDial);

        mainLayout->addWidget(ponteirosBox);

        // Informações de consumo
        QGroupBox* infoBox = new QGroupBox(QStringLiteral("📊 Informações de Consumo"));
        QGridLayout* g = new QGridLayout;
        infoBox->setLayout(g);

        g->addWidget(new QLabel("Consumo total (m³):"), 0, 0);
        totalLabel = new QLabel("0.000");
        g->addWidget(totalLabel, 0, 1);

        g->addWidget(new QLabel("Consumo desde última leitura (m³):"), 1, 0);
        consumoLabel = new QLabel("0.000");
        g->addWidget(consumoLabel, 1, 1);

        g->addWidget(new QLabel("Vazão atual (m³/h):"), 2, 0);
        vazaoAtualLabel = new QLabel("0.000");
        g->addWidget(vazaoAtualLabel, 2, 1);

        mainLayout->addWidget(infoBox);

        // Timer (simulação)
        timer = new QTimer(this);
        timer->setInterval(80); // aproximadamente 12.5 FPS

        // Conexões
        connect(vazaoSlider, &QSlider::valueChanged, this, &HidrometroWindow::onVazaoChanged);
        connect(leituraSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &HidrometroWindow::onLeituraAnteriorChanged);
        connect(startButton, &QPushButton::clicked, this, &HidrometroWindow::onStart);
        connect(stopButton, &QPushButton::clicked, this, &HidrometroWindow::onStop);
        connect(resetButton, &QPushButton::clicked, this, &HidrometroWindow::onReset);
        connect(timer, &QTimer::timeout, this, &HidrometroWindow::onTick);

        // inicializa valores
        onVazaoChanged(vazaoSlider->value());
    }

protected:
    void paintEvent(QPaintEvent* ev) override {
        QMainWindow::paintEvent(ev);
        // desenha o mostrador principal dentro do displayWidget
        QPainter p(this);
        // vamos desenhar manualmente sobre displayWidget area
        QWidget* dw = displayWidget;
        QRect r = dw->geometry();
        p.translate(dw->mapTo(this, QPoint(0,0)));
        p.setRenderHint(QPainter::Antialiasing);

        // fundo
        p.fillRect(0,0, r.width(), r.height(), QColor("#ffffff"));
        p.setPen(Qt::black);
        p.drawRect(10,10, r.width()-20, r.height()-20);

        // Título
        QFont ft = p.font(); ft.setBold(true); ft.setPointSize(11);
        p.setFont(ft);
        p.drawText(r.width()/2 - 60, 20, "m³ = 1000 litros");

        // Medição
        const Medicao& m = hidrometro.getMedicao();
        std::stringstream ss; ss << std::setw(4) << std::setfill('0') << m.getConsumoM3();
        std::string s = ss.str();

        int start_x = 20;
        for (int i=0;i<4;i++){
            int x = start_x + i*90;
            p.drawRect(x, 40, 70, 80);
            QFont f; f.setPointSize(24); f.setBold(true);
            p.setFont(f);
            p.drawText(x, 40, 70, 80, Qt::AlignCenter, QString::fromStdString(std::string(1, s[i])));
        }

        // Centenas, Dezenas, etc
        int xbase = start_x + 4*90 + 10;
        // Centenas
        p.setFont(QFont("Arial", 20, QFont::Bold));
        p.drawRect(xbase, 40, 60, 80);
        p.drawText(xbase, 40, 60, 80, Qt::AlignCenter, QString::number(m.getCentenasLitros()));
        p.setFont(QFont("Arial", 8, QFont::Bold));
        p.drawText(xbase, 125, 60, 30, Qt::AlignCenter, "Centenas\nlitros");

        // Dezenas
        xbase += 80;
        p.setFont(QFont("Arial", 20, QFont::Bold));
        p.drawRect(xbase, 40, 60, 80);
        p.drawText(xbase, 40, 60, 80, Qt::AlignCenter, QString::number(m.getDezenasLitros()));
        p.setFont(QFont("Arial", 8, QFont::Bold));
        p.drawText(xbase, 125, 60, 30, Qt::AlignCenter, "Dezenas\nlitros");

        // Atualiza ponteiros (widgets próprios)
        // (os DialWidget cuidam do desenho deles mesmos)
    }

private slots:
    void onVazaoChanged(int v) {
        // slider 0..1000 -> 0.00 .. 10.00
        double vazao = v / 100.0;
        hidrometro.setVazaoEntrada(vazao);
        vazaoValueLabel->setText(QString::number(vazao, 'f', 2) + " m³/h");
        vazaoAtualLabel->setText(QString::number(vazao, 'f', 3));
    }

    void onLeituraAnteriorChanged(double val) {
        hidrometro.setLeituraAnterior(val);
    }

    void onStart() {
        startTs = std::chrono::steady_clock::now();
        lastTick = startTs;
        timer->start();
        startButton->setEnabled(false);
        stopButton->setEnabled(true);
    }

    void onStop() {
        timer->stop();
        startButton->setEnabled(true);
        stopButton->setEnabled(false);
    }

    void onReset() {
        onStop();
        hidrometro = Hidrometro();
        vazaoSlider->setValue(150);
        leituraSpin->setValue(0.0);
        updateDisplays();
        repaint();
    }

    void onTick() {
        // calcula dt em horas baseado em steady_clock
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> dt = now - lastTick;
        lastTick = now;
        double segundos = dt.count();
        double horas = segundos / 3600.0;
        hidrometro.atualizarVolume(horas);
        hidrometro.medir();
        updateDisplays();
        // repaint mostrador principal
        update(); // chama paintEvent de toda a janela (inclui displayWidget)
    }

private:
    void updateDisplays() {
        totalLabel->setText(QString::number(hidrometro.getVolumeTotal(), 'f', 3));
        consumoLabel->setText(QString::number(hidrometro.getConsumo(), 'f', 3));
        vazaoAtualLabel->setText(QString::number(hidrometro.getEntrada().getVazao(), 'f', 3));

        const Medicao& m = hidrometro.getMedicao();
        litrosDial->setValue(m.getLitros());
        decimosDial->setValue(m.getDecimosLitros());
        // força redesenho dos dials
        litrosDial->update();
        decimosDial->update();
    }

    // Modelo
    Hidrometro hidrometro;

    // Widgets
    QSlider* vazaoSlider;
    QLabel* vazaoValueLabel;
    QDoubleSpinBox* leituraSpin;
    QPushButton* startButton;
    QPushButton* stopButton;
    QPushButton* resetButton;
    QWidget* displayWidget;
    DialWidget* litrosDial;
    DialWidget* decimosDial;

    QLabel* totalLabel;
    QLabel* consumoLabel;
    QLabel* vazaoAtualLabel;

    QTimer* timer;
    std::chrono::steady_clock::time_point startTs;
    std::chrono::steady_clock::time_point lastTick;
};

// --------- main ----------
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    HidrometroWindow w;
    w.show();
    return a.exec();
}

#include "hidrometro.moc"
