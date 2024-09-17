#ifndef FIXTABLE_H
#define FIXTABLE_H

#include <QTableView>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QLineEdit>
#include "fixtablemodel.h"
#include "fixsortfilterproxymodel.h"

class FixTable : public QTableView {
    Q_OBJECT

public:
    FixTable(QWidget* parent = nullptr);
    void setFixLine(const QString& fix);

public Q_SLOTS:
    void sortByColumn(int column, Qt::SortOrder order);

private:
    FixTableModel* fixModel;
    FixSortFilterProxyModel* proxyModel;
    QMap<int, Qt::SortOrder> sortOrderMap;

    void setupTable();
    void addFilterRow();
    void filterTable(int column, const QString& filterText);
};

#endif // FIXTABLE_H
