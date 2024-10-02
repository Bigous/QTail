#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QScreen>
#include <QSplashScreen>

Q_DECLARE_METATYPE(QByteArrayView)

int main(int argc, char *argv[])
{
#ifndef NDEBUG
    // Se estiver no modo de depuração, incluir nome do arquivo e número da linha
    qSetMessagePattern("%{time hh:mm:ss.zzz}|%{file}:%{line}|%{message}");
#else
    // Caso contrário, apenas exibir o tempo e a mensagem
    qSetMessagePattern("%{time hh:mm:ss.zzz}|%{message}");
#endif
    qInfo() << "Starting QTail...";

    qRegisterMetaType<QByteArrayView>("QByteArrayView");

    QApplication a(argc, argv);

    //QString iconPath = QDir(QCoreApplication::applicationDirPath() + "/assets/img/").filePath("app_icon.ico");
    QIcon icon("./assets/imgs/QTail.ico");

    a.setWindowIcon(icon);

    QPixmap pixmap("./assets/imgs/splash-realista.webp");
    QSplashScreen splash(pixmap);
    // Pega a posição do mouse
    QPoint globalCursorPos = QCursor::pos();
    // Move a janela para a posição do mouse
    qDebug() << "Mouse is in: " << globalCursorPos;
    qDebug() << "There are " << QApplication::screens().size() << " screens";
    for(auto screen : QApplication::screens()) {
        qDebug() << "Screen: " << screen->geometry();
        if(screen->geometry().contains(globalCursorPos))
        {
            splash.move(screen->geometry().center() - splash.rect().center());
            break;
        }
    }
    splash.show();


    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();

    qInfo() << "Setting up translator for locale " << uiLanguages;

    for (const QString &locale : uiLanguages) {
        const QString baseName = "QTail_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    qInfo() << "Creating main window";

    MainWindow w;

    w.setWindowIcon(icon);
    w.loadDictionary("./assets/fix/FIX44.xml");

    // Seta o tamanho da janela para 800x600 e centraliza
    w.resize(1024, 768);
    for(auto screen : QApplication::screens()) {
        qDebug() << "Screen: " << screen->geometry();
        if(screen->geometry().contains(globalCursorPos))
        {
            w.move(screen->geometry().center() - w.rect().center());
            break;
        }
    }

    w.showNormal();
    splash.finish(&w);

    qInfo() << "Processing Event Loop.";

    auto ret = a.exec();

    qInfo() << "Exiting QTail with value " << ret;

    return ret;
}
