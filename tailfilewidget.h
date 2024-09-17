#ifndef TAILFILEWIDGET_H
#define TAILFILEWIDGET_H

#include <QDockWidget>
#include <QTextEdit>
#include <QRegularExpression>
#include "filemonitor.h"
#include "highlighter.h"
#include "fixtable.h"

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
    FixTable* fixTable;
    FileMonitor* fileMonitor;
    QList<Highlighter*> highlighters;
    QRegularExpression fixRegex;

    void setupFixTable();
    QString getCurrentLine();
};

#endif // TAILFILEWIDGET_H
