#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

protected:
    void onDockWidgetToggled(QDockWidget* dockWidget);

    QDockWidget* findFirstDockWidget();

private slots:
    void openFile(); // Novo slot para abrir o arquivo

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
