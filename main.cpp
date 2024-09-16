#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "QTail_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    // Seta o tamanho da janela para 800x600 e centraliza
    w.resize(800, 600);
    // Pega a posição do mouse
    QPoint globalCursorPos = QCursor::pos();
    // Move a janela para a posição do mouse
    qDebug() << "Mouse is in: " << globalCursorPos;
    qDebug() << "There are " << QApplication::screens().size() << " screens";
    for(auto screen : QApplication::screens()) {
        qDebug() << "Screen: " << screen->geometry();
        if(screen->geometry().contains(globalCursorPos))
        {
            w.move(screen->geometry().center() - w.rect().center());
            break;
        }
    }
    w.showNormal();
    return a.exec();
}
