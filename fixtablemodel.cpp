#include "fixtablemodel.h"

FixTableModel::FixTableModel(QObject *parent)
    : QStandardItemModel(parent) {
}


QVariant FixTableModel::data(const QModelIndex &index, int role) const {
    if (index.row() == 0) {
        return QVariant(); // Primeira linha contém filtros, não dados
    }
    // Ajuste o índice para considerar a linha de filtro
    QModelIndex newIndex = this->index(index.row() - 1, index.column());
    return QStandardItemModel::data(newIndex, role);
}

int FixTableModel::rowCount(const QModelIndex &parent) const {
    // Adicione uma linha extra para os filtros
    return QStandardItemModel::rowCount(parent) + 1;
}

// Se precisar permitir edição dos dados, sobrescreva setData() de forma semelhante
bool FixTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.row() == 0) {
        return false; // Não permite editar a linha de filtro
    }
    QModelIndex newIndex = this->index(index.row() - 1, index.column());
    return QStandardItemModel::setData(newIndex, value, role);
}
