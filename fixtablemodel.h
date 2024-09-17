#ifndef FIXTABLEMODEL_H
#define FIXTABLEMODEL_H

#include <QStandardItemModel>

class FixTableModel : public QStandardItemModel {
    Q_OBJECT
public:
    explicit FixTableModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override ;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // Se precisar permitir edição dos dados, sobrescreva setData() de forma semelhante
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
};


#endif // FIXTABLEMODEL_H
