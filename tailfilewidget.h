#ifndef TAILFILEWIDGET_H
#define TAILFILEWIDGET_H

#include <QDockWidget>
#include <QListView>
#include <QTableWidget>
#include <QRegularExpression>
#include <QCheckBox>
#include <QPushButton>

#include "LogFileModel.hpp"
#include "FixDictionary.hpp"
#include "FilterProxyModels.hpp"

class TailFileWidget : public QDockWidget {
    Q_OBJECT
public:
    TailFileWidget(const QString& filePath, FixDictionary *fixDictionary , QWidget* parent = nullptr);

public slots:
    void applyFilter(const QString& filterText);

private:
    QWidget* logWidget;
    QLineEdit* filterEdit;
    QCheckBox* regexCheckBox;
    QPushButton* toggleFilterButton;
    QListView* listView;
    QTableWidget* fixTable;
    LogFileModel* fileModel;
    QRegularExpression fixRegex;
    FixDictionary* m_fixDictionary;
    ContainsFilterProxyModel* containsProxyModel;
    RegExFilterProxyModel* regexProxyModel;

    void processFixLine(const QString& line);
    void setupFixTable();
    QString getCurrentLine();
    void addFilterRow();
    void filterTable(int column, const QString& filterText);
};

#endif // TAILFILEWIDGET_H
