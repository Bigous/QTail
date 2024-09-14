#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "DockOverlay.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

protected:
    void onDockWidgetToggled(QDockWidget* dockWidget) {
        if (!dockWidget->isFloating()) {
            // Quando o widget for dockado, transformá-lo em uma aba
            QDockWidget* firstDockWidget = findFirstDockWidget();
            if (firstDockWidget && firstDockWidget != dockWidget) {
                tabifyDockWidget(firstDockWidget, dockWidget);
                dockWidget->raise(); // Deixar a aba dockada na frente
            }
        }
    }

    QDockWidget* findFirstDockWidget() {
        // Acha o primeiro QDockWidget existente para tabificação
        const auto dockWidgets = findChildren<QDockWidget*>();
        for (auto* dockWidget : dockWidgets) {
            if (!dockWidget->isFloating() && dockWidget->isVisible()) {
                return dockWidget;
            }
        }
        return nullptr;
    }

    void onDockTopLevelChanged(bool floating) {
        if (floating) {
            overlay->show();
        } else {
            overlay->hide();
        }
    }

    void onDockLocationChanged(Qt::DockWidgetArea area) {
        overlay->hide();
    }

    // Interceptar eventos de mouse para detectar quando o dock entra na área central
    bool eventFilter(QObject* obj, QEvent* event) override {
        if (event->type() == QEvent::MouseMove) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (dock->isFloating() && overlay->isVisible()) {
                QRect centerArea(width() / 4, height() / 4, width() / 2, height() / 2);
                if (centerArea.contains(mouseEvent->pos())) {
                    // Central Docking Preview
                    overlay->update();  // Atualiza o overlay para mostrar o highlight da área central
                }
            }
        }
        return QMainWindow::eventFilter(obj, event);
    }

    // Sobrescreve o evento de redimensionamento para ajustar o overlay
    void resizeEvent(QResizeEvent* event) override {
        QMainWindow::resizeEvent(event);
        overlay->setGeometry(0, 0, width(), height());
    }

    // Sobrescreve o evento de drop para dockar na área central
    void dropEvent(QDropEvent* event) override {
        QRect centerArea(width() / 4, height() / 4, width() / 2, height() / 2);
        if (centerArea.contains(event->position().toPoint())) {
            // Remover o widget central existente
            QWidget* currentCentralWidget = centralWidget();
            if (currentCentralWidget) {
                currentCentralWidget->hide();
            }
            setCentralWidget(dock->widget());
            dock->setFloating(false); // Fixar o dock na área central
            overlay->hide();
        }
    }


private:
    Ui::MainWindow *ui;
    QDockWidget* dock;
    DockOverlay* overlay;
};
#endif // MAINWINDOW_H
