#include <QTextEdit>
#include <QDockWidget>
#include <QFileDialog>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "tailfilewidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    static int i = 1;
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North); // Abas no topo

    // Criando a barra de menus
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));

    // Alterando "New Dock" para "Open" e conectando ao slot para abrir arquivo
    QAction* openAction = fileMenu->addAction(tr("&Open"), this, &MainWindow::openFile);
    openAction->setShortcut(QKeySequence::Open);

    QAction* quitAction = fileMenu->addAction(tr("&Quit"), this, &QMainWindow::close);
    quitAction->setShortcut(QKeySequence::Quit);
}

void MainWindow::openFile() {
    // Abrir QFileDialog para o usuário selecionar o arquivo
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All Files (*)"));

    if (!filePath.isEmpty()) {
        // Criar uma nova TailFileWidget com o arquivo selecionado
        TailFileWidget* tailFileWidget = new TailFileWidget(filePath, this);
        tailFileWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
        tailFileWidget->setWindowTitle(tr("%1").arg(filePath));

        // Se não houver widgets dockados, docka ele à esquerda. Caso contrário, tabifica junto com o primeiro widget dockado
        if (!findFirstDockWidget()) {
            addDockWidget(Qt::LeftDockWidgetArea, tailFileWidget);
        } else {
            tabifyDockWidget(findFirstDockWidget(), tailFileWidget);
        }
        tailFileWidget->raise(); // Deixar o novo dock em foco
    }
}

void MainWindow::onDockWidgetToggled(QDockWidget* dockWidget) {
    if (!dockWidget->isFloating()) {
        // Quando o widget for dockado, transformá-lo em uma aba
        QDockWidget* firstDockWidget = findFirstDockWidget();
        if (firstDockWidget && firstDockWidget != dockWidget) {
            tabifyDockWidget(firstDockWidget, dockWidget);
            dockWidget->raise(); // Deixar a aba dockada na frente
        }
    }
}

QDockWidget* MainWindow::findFirstDockWidget() {
    // Acha o primeiro QDockWidget existente para tabificação
    const auto dockWidgets = findChildren<QDockWidget*>();
    for (auto* dockWidget : dockWidgets) {
        if (!dockWidget->isFloating() && dockWidget->isVisible()) {
            return dockWidget;
        }
    }
    return nullptr;
}

MainWindow::~MainWindow() {
    delete ui;
}
