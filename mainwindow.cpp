#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QTextEdit>
#include <QDockWidget>

#include <format>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    static int i = 1;
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North); // Abas no topo

    QDockWidget* dock = new QDockWidget(tr("Dockable Widget - %1").arg(i++), this);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setWidget(new QTextEdit(dock));
    // Exibe flutuando
    dock->setFloating(false);
    setCentralWidget(dock);

    // Criando a barra de menus
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&New Dock"), [this] {
        QDockWidget* newDock = new QDockWidget(tr("Dockable Widget - %1").arg(i++), this);
        newDock->setAllowedAreas(Qt::AllDockWidgetAreas);
        newDock->setWidget(new QTextEdit(newDock));
        // Exibe flutuando
        newDock->setFloating(true);
        newDock->show();
    });
    QAction* quitAction = fileMenu->addAction(tr("&Quit"), this, &QMainWindow::close);
    quitAction->setShortcut(QKeySequence::Quit);

    // Criando o overlay para docking
    overlay = new DockOverlay(this);
    overlay->setGeometry(0, 0, width(), height());
    overlay->hide();

    // Conectar os sinais para detectar arrastamento do dock widget
    connect(dock, &QDockWidget::topLevelChanged, this, &MainWindow::onDockTopLevelChanged);
    connect(dock, &QDockWidget::dockLocationChanged, this, &MainWindow::onDockLocationChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}
