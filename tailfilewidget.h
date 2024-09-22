#ifndef TAILFILEWIDGET_H
#define TAILFILEWIDGET_H

#include <QDockWidget>
#include <QListView>
#include <QTableWidget>
#include <QRegularExpression>
#include "LogFileModel.hpp"
#include "highlighter.h"
#include "FixDictionary.hpp"

class TailFileWidget : public QDockWidget {
    Q_OBJECT
public:
    TailFileWidget(const QString& filePath, FixDictionary *fixDictionary , QWidget* parent = nullptr);
    void addHighlighter(Highlighter* highlighter);

private:
    QListView* listView;
    QTableWidget* fixTable;
    LogFileModel* fileModel;
    QList<Highlighter*> highlighters;
    QRegularExpression fixRegex;
    FixDictionary* m_fixDictionary;

    void processFixLine(const QString& line);
    void setupFixTable();
    QString getCurrentLine();
    void addFilterRow();
    void filterTable(int column, const QString& filterText);
};

#endif // TAILFILEWIDGET_H
