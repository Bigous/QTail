#ifndef DOCKOVERLAY_HPP
#define DOCKOVERLAY_HPP

#include <QMainWindow>
#include <QDockWidget>
#include <QTextEdit>
#include <QMenuBar>
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QLabel>
#include <QVBoxLayout>

class DockOverlay : public QWidget {
public:
    DockOverlay(QWidget* parent = nullptr) : QWidget(parent) {
        setAttribute(Qt::WA_TransparentForMouseEvents); // Permite eventos passarem
        setAttribute(Qt::WA_NoSystemBackground); // Sem fundo de sistema
        setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip); // Janela sem borda
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);
        QColor semiTransparentColor(0, 128, 255, 100);

        // Áreas laterais para docking
        QRect leftArea(0, 0, 100, height());
        QRect rightArea(width() - 100, 0, 100, height());
        QRect topArea(0, 0, width(), 100);
        QRect bottomArea(0, height() - 100, width(), 100);

        // Área central para docking
        QRect centerArea(width() / 4, height() / 4, width() / 2, height() / 2);

        // Desenha as áreas de docking
        painter.fillRect(leftArea, semiTransparentColor);
        painter.fillRect(rightArea, semiTransparentColor);
        painter.fillRect(topArea, semiTransparentColor);
        painter.fillRect(bottomArea, semiTransparentColor);
        painter.fillRect(centerArea, QColor(255, 0, 0, 100));  // Cor diferente para a área central
    }
};

#endif // DOCKOVERLAY_HPP
