#ifndef FILTERPROXYMODELS_HPP
#define FILTERPROXYMODELS_HPP

#include <QSortFilterProxyModel>
#include <QRegularExpression>

class ContainsFilterProxyModel : public QSortFilterProxyModel {
public:
    ContainsFilterProxyModel(QObject* parent = nullptr)
        : QSortFilterProxyModel(parent) {
    }

    void setFilter(const QString& filter) {
        m_filter = filter;
        search = m_filter.toUtf8();
        searchView = QByteArrayView(search);
        invalidateFilter(); // Reaplica o filtro
    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override {
        QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
        QByteArrayView line = sourceModel()->data(index, Qt::UserRole).value<QByteArrayView>();
        return line.contains(searchView); // Filtro com contains
    }

private:
    QString m_filter;
    QByteArray search;
    QByteArrayView searchView;
};

class RegExFilterProxyModel : public QSortFilterProxyModel {
public:
    RegExFilterProxyModel(QObject* parent = nullptr)
        : QSortFilterProxyModel(parent) {
    }

    void setFilter(const QString& pattern) {
        m_regex.setPattern(pattern);
        invalidateFilter(); // Reaplica o filtro
    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override {
        QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
        QString lineText = sourceModel()->data(index).toString();
        return m_regex.match(lineText).hasMatch(); // Filtro com regex
    }

private:
    QRegularExpression m_regex;
};


#endif // FILTERPROXYMODELS_HPP
