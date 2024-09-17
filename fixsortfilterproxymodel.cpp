#include "fixsortfilterproxymodel.h"

FixSortFilterProxyModel::FixSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent), filterTexts(4) {}

void FixSortFilterProxyModel::setFilterTexts(const QStringList &texts) {
    filterTexts = texts;
    invalidateFilter();
}

void FixSortFilterProxyModel::setSortColumns(const QList<int> &columns, const QMap<int, Qt::SortOrder> &orders) {
    sortColumns = columns;
    sortOrders = orders;
    invalidate();
}

bool FixSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    // Ignore a primeira linha (linha de filtro)
    if (sourceRow == 0) {
        return false; // Não exibe a linha de filtro nos dados
    }
    // Ajuste sourceRow para considerar a linha de filtro
    int adjustedRow = sourceRow - 1;
    for (int i = 0; i < filterTexts.size(); ++i) {
        if (!filterTexts[i].isEmpty()) {
            QModelIndex index = sourceModel()->index(adjustedRow, i, sourceParent);
            QString data = sourceModel()->data(index).toString();
            if (!data.contains(filterTexts[i], Qt::CaseInsensitive)) {
                return false;
            }
        }
    }
    return true;
}

bool FixSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    // Ignore a primeira linha
    if (left.row() == 0 || right.row() == 0) {
        return false;
    }
    // Ajuste os índices para considerar a linha de filtro
    int leftRow = left.row() - 1;
    int rightRow = right.row() - 1;
    for (int column : sortColumns) {
        QModelIndex leftIndex = sourceModel()->index(leftRow, column);
        QModelIndex rightIndex = sourceModel()->index(rightRow, column);

        QVariant leftData = sourceModel()->data(leftIndex);
        QVariant rightData = sourceModel()->data(rightIndex);

        if (leftData != rightData) {
            if (sortOrders[column] == Qt::AscendingOrder) {
                return leftData.toString() < rightData.toString();
            } else {
                return leftData.toString() > rightData.toString();
            }
        }
    }
    return false;
}
