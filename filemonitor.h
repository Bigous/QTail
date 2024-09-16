#ifndef FILEMONITOR_H
#define FILEMONITOR_H

#include <QFileSystemWatcher>

class FileMonitor : public QObject {
    Q_OBJECT

public:
    FileMonitor(const QString& filePath, QObject* parent = nullptr);
    virtual ~FileMonitor() {}

    void Start();

private slots:
    void onFileChanged();

signals:
    void newLineRead(qint64 pos, const QString& line);

private:
    void loadInitialContent();

    QString filePath;
    qint64 lastPos;  // Última posição lida no arquivo
    QFileSystemWatcher fileWatcher;
};

#endif // FILEMONITOR_H
