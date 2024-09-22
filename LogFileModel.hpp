#ifndef LOGFILEMODEL_HPP
#define LOGFILEMODEL_HPP

#include <deque>

#include <QStringView>

#include <QAbstractListModel>
#include <QStringList>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QCoroTask>
#include <QCoroIODevice>
#include <QRegularExpression>

class LogFileModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit LogFileModel(const QString& filePath, QObject *parent = nullptr)
        : QAbstractListModel(parent), m_file(filePath), m_fileName(filePath), m_controlChars("[\\x00-\\x1F]") {

    }

    virtual ~LogFileModel() {
        Stop();
    }

    void Start() {
        if(!m_task.isReady())
            Stop();
        qDebug() << "Starting reading task for file " << m_fileName;
        m_stopRequested = false;
        m_task = readFile();
    }

    void Stop() {
        qDebug() << "Stopping reading task for file " << m_fileName;
        if(m_task.isReady())
            return;
        m_stopRequested = true;
        QCoro::waitFor(m_task);
        m_lines.clear();
        m_buffers.clear();
    }

    // Implementação do método rowCount() - número de linhas no modelo
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        if (parent.isValid())
            return 0;
        return m_lines.size();
    }

    // Implementação do método data() - conteúdo a ser exibido no QListView
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_lines.size() || role != Qt::DisplayRole)
            return QVariant();

        QString displayText = QString::fromUtf8(m_lines[index.row()]);

        QRegularExpressionMatchIterator it = m_controlChars.globalMatch(displayText);

        // Iterar sobre cada correspondência de caractere de controle
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            char controlChar = match.captured(0).at(0).toLatin1();

            // Substituir o caractere de controle pelo correspondente na faixa 0x2400
            QString replacement = QString(QChar(0x2400 + controlChar));

            // Substituir o caractere no texto original
            displayText.replace(match.capturedStart(0), 1, replacement);
        }

        return displayText;
        //return QByteArray(m_lines[index.row()].data(), m_lines[index.row()].size());
    }

signals:
    // Sinal para notificar que novas linhas foram adicionadas
    void onBeforeLinesAppended();
    void onLinesAppended();

private slots:

private:
    QString m_fileName;
    std::deque<QByteArrayView> m_lines;
    std::deque<QByteArray> m_buffers;
    QFile m_file;
    QCoro::Task<void> m_task;
    bool m_lastLineWasIncomplete = false;
    bool m_stopRequested = false;
    QRegularExpression m_controlChars;

    void processBuffer(QByteArray& buffer)
    {
        emit onBeforeLinesAppended();
        int initialSize = m_lines.size();

        qDebug() << "Processing buffer of size: " << buffer.size();

        size_t startPos = 0;
        size_t i = 0;
        const auto bufferSize = buffer.size();
        for(; i < bufferSize; ++i) {
            if(buffer[i] == '\n') {
                if(i > 1 || !m_lastLineWasIncomplete)
                    m_lines.emplace_back(buffer.data() + startPos, i - startPos);
                startPos = i + 1;
            }
        }
        if(startPos < i) {
            m_lines.emplace_back(buffer.data() + startPos, i - startPos);
        }
        m_lastLineWasIncomplete = buffer.back() != '\n';
        emit onLinesAppended();

        int finalSize = m_lines.size();
        if(finalSize > initialSize) {
            qDebug() << "Inserted " << finalSize - initialSize << " lines";
            beginInsertRows(QModelIndex(), initialSize, finalSize -1);
            endInsertRows();
        }
    }


    QCoro::Task<void> readFile()
    {
        if(!m_file.open(QIODevice::ReadOnly | QIODevice::Text | QIODevice::ExistingOnly))
        {
            qDebug() << "Failed to open file: " << m_file.errorString();
            co_return;
        }

        // Carga inicial de todas as linhas já existentes no arquivo
        m_buffers.emplace_back(m_file.readAll());

        processBuffer(m_buffers.back());

        QCoro::detail::QCoroIODevice coroDevice{&m_file};

        std::chrono::milliseconds waitParameter(500);

        // Monitoramento contínuo para novas linhas
        while (!m_stopRequested) {
            QByteArray newBuffer = co_await coroDevice.readAll(waitParameter);
            if (!newBuffer.isEmpty()) {
                m_buffers.emplace_back(std::move(newBuffer));
                processBuffer(m_buffers.back());
            }
        }
    }

};

#endif // LOGFILEMODEL_HPP
