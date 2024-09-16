#include "filemonitor.h"

#include <QFile>
#include <QTextStream>
#include <QFileSystemWatcher>
#include <QDebug>
#include <QtConcurrent>
#include <QFuture>
#include <QPromise>

FileMonitor::FileMonitor(const QString& filePath, QObject* parent)
    : QObject(parent), filePath(filePath), lastPos(0) {

}

void FileMonitor::Start()
{
    // Carregar o conteúdo inicial do arquivo
    loadInitialContent();

    // Configurar o QFileSystemWatcher
    fileWatcher.addPath(filePath);
    connect(&fileWatcher, &QFileSystemWatcher::fileChanged, this, &FileMonitor::onFileChanged);
}

void FileMonitor::onFileChanged() {
    QFuture<void> future = QtConcurrent::run([this]() {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text | QIODevice::ExistingOnly)) {
            qDebug() << "Falha ao abrir o arquivo";
            return;
        }

        // Pular para a última posição lida
        file.seek(lastPos);

        QTextStream in(&file);
        QString newLine;
        while (!in.atEnd()) {
            newLine = in.readLine();
            lastPos = file.pos();  // Atualiza a posição de leitura

            // Emitir o sinal de nova linha lida
            emit newLineRead(lastPos, newLine);
        }

        // Re-registrar o arquivo no QFileSystemWatcher
        fileWatcher.addPath(filePath);
    });
}

void FileMonitor::loadInitialContent() {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text | QIODevice::ExistingOnly)) {
        qDebug() << "Falha ao abrir o arquivo";
        return;
    }

    qDebug() << "Reading: " << filePath;

    QTextStream in(&file);
    while (!in.atEnd()) {
        lastPos = file.pos();
        QString line = in.readLine();

        // Emitir o sinal de nova linha lida
        emit newLineRead(lastPos, line);
    }
}
