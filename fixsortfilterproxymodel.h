#ifndef FIXSORTFILTERPROXYMODEL_H
#define FIXSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class FixSortFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit FixSortFilterProxyModel(QObject *parent = nullptr);

    void setFilterTexts(const QStringList &texts);

    void setSortColumns(const QList<int> &columns, const QMap<int, Qt::SortOrder> &orders);

protected:
    QStringList filterTexts;
    QList<int> sortColumns;
    QMap<int, Qt::SortOrder> sortOrders;

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};


#endif // FIXSORTFILTERPROXYMODEL_H
