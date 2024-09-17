#include "fixtable.h"

#include <QHeaderView>

FixTable::FixTable(QWidget* parent) : QTableView(parent) {
    fixModel = new FixTableModel(this);
    proxyModel = new FixSortFilterProxyModel(this);

    // Sobrescrever a ordenação para evitar que a primeira linha seja ordenada
    proxyModel->setSourceModel(fixModel);
    setModel(proxyModel);

    setupTable();
}

void FixTable::setupTable() {
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // verticalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    // horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->setVisible(false);
    setRowHeight(0, 25);
    setSortingEnabled(false);

    // Adicionar linha de filtro
    addFilterRow();

    // Sets up the sorting mechanism
    connect(horizontalHeader(), &QHeaderView::sectionClicked, this, [this](int logicalIndex) {
        // Atualize o estado de ordenação da coluna clicada
        if (!sortOrderMap.contains(logicalIndex)) {
            sortOrderMap[logicalIndex] = Qt::AscendingOrder;
        } else if (sortOrderMap[logicalIndex] == Qt::AscendingOrder) {
            sortOrderMap[logicalIndex] = Qt::DescendingOrder;
        } else {
            sortOrderMap.remove(logicalIndex);
        }

        // Atualize a lista de colunas ordenadas
        QList<int> sortColumns = sortOrderMap.keys();

        // Atualize o proxyModel para considerar as novas colunas de ordenação
        proxyModel->setSortColumns(sortColumns, sortOrderMap);
        proxyModel->invalidate();
    });

}

void FixTable::addFilterRow() {
    int columnCount = fixModel->columnCount();
    fixModel->insertRow(0);  // Inserir linha de filtro no topo

    for (int col = 0; col < columnCount; ++col) {
        QLineEdit* filterEdit = new QLineEdit();
        filterEdit->setPlaceholderText("Filter...");
        connect(filterEdit, &QLineEdit::textChanged, this, [this, col](const QString& text) {
            filterTable(col, text);
        });
        this->setIndexWidget(fixModel->index(0, col), filterEdit);
    }
}

void FixTable::filterTable(int column, const QString& filterText) {
    QRegularExpression regex(filterText, QRegularExpression::CaseInsensitiveOption);
    proxyModel->setFilterKeyColumn(column);
    proxyModel->setFilterRegularExpression(regex);
}

// Sobrescrevendo o sort para fixar a linha de filtro
void FixTable::sortByColumn(int column, Qt::SortOrder order) {
    proxyModel->sort(column, order);

    // Fixar a linha de filtro no topo após a ordenação
    QModelIndex filterIndex = fixModel->index(0, 0);
    this->scrollTo(filterIndex);
}

void FixTable::setFixLine(const QString& fix) {
    fixModel->clear();

    fixModel->setColumnCount(4);
    fixModel->setHorizontalHeaderLabels(QStringList() << tr("Tag") << tr("Value") << tr("Tag Name") << tr("Value Domain"));

    addFilterRow();

    QStringList fields = fix.split("\x01");
    for (const QString& field : fields) {
        QStringList tagValue = field.split("=");
        if (tagValue.size() == 2) {
            QString tag = tagValue[0];
            QString value = tagValue[1];

            QList<QStandardItem *> items;
            items << new QStandardItem(tag)
                  << new QStandardItem(value)
                  << new QStandardItem("Tag Name")
                  << new QStandardItem("Domain Description");

            fixModel->appendRow(items);
        }
    }
}
