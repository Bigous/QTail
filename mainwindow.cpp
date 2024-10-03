#include <QTextEdit>
#include <QDockWidget>
#include <QFileDialog>
#include <QStackedWidget>
#include <qforeach.h>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "tailfilewidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_fixDictionary(this) {
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
        TailFileWidget* tailFileWidget = new TailFileWidget(filePath, &m_fixDictionary, this);
        //tailFileWidget->setWindowTitle(fileName);

        // Se não houver widgets dockados, docka ele à esquerda. Caso contrário, tabifica junto com o primeiro widget dockado
        auto firstDocked = findFirstDockWidget();
        if (firstDocked == nullptr) {
            addDockWidget(Qt::LeftDockWidgetArea, tailFileWidget);
        } else {
            addTabToDock(firstDocked, tailFileWidget);
        }
    }
}

void MainWindow::onDockWidgetToggled(QDockWidget* dockWidget) {
    if (!dockWidget->isFloating()) {
        // Quando o widget for dockado, transformá-lo em uma aba
        QDockWidget* firstDockWidget = findFirstDockWidget();
        if (firstDockWidget && firstDockWidget != dockWidget) {
            addTabToDock(firstDockWidget, dockWidget);
        }
    }
}

void MainWindow::addTabToDock(QDockWidget* first, QDockWidget* second)
{
    TailFileWidget *lfw = qobject_cast<TailFileWidget*>(first);

    // Precisamos chamar checkScrollPositionBeforeInsert e scrollToBottomIfNeeded pois a criação da tab diminui o espaço vertical do ListView
    // e isso desloca a última linha para fora da área visível, o que não é desejável para um monitor de log.
    if(lfw) lfw->checkScrollPositionBeforeInsert();

    tabifyDockWidget(first, second);
    QMetaObject::invokeMethod(second, "raise", Qt::QueuedConnection); // activate the tab

    if(lfw) lfw->scrollToBottomIfNeeded();
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
