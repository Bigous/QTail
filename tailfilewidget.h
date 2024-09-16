#ifndef TAILFILEWIDGET_H
#define TAILFILEWIDGET_H

#include <QDockWidget>
#include <QTextEdit>
#include <QTableWidget>
#include <QRegularExpression>
#include "filemonitor.h"
#include "highlighter.h"

class TailFileWidget : public QDockWidget {
    Q_OBJECT
public:
    TailFileWidget(const QString& filePath, QWidget* parent = nullptr);
    void addHighlighter(Highlighter* highlighter);

private slots:
    void onNewLine(qint64 pos, const QString& line);
    void onCursorPositionChanged(); // Novo slot para mudança de posição do cursor

private:
    QTextEdit* textEdit;
    QTableWidget* fixTable;
    FileMonitor* fileMonitor;
    QList<Highlighter*> highlighters;
    QRegularExpression fixRegex;

    void processFixLine(const QString& line);
    void setupFixTable();
    QString getCurrentLine();
    void addFilterRow();
    void filterTable(int column, const QString& filterText);
};

#endif // TAILFILEWIDGET_H
