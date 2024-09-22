#ifndef LOGLISTVIEW_H
#define LOGLISTVIEW_H

#include <QListView>
#include <QScrollBar>
#include <QStyledItemDelegate>
#include <QFontMetrics>
#include <QPainter>
#include "LogFileModel.hpp"

// Delegate personalizado para ajustar a altura das linhas
class LogItemDelegate : public QStyledItemDelegate {
public:
    explicit LogItemDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent), m_selectionColor(0, 96, 192) {}

    // Sobrescrever o sizeHint para ajustar a altura de cada item
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QFontMetrics metrics(option.font);
        qreal dpiScale = option.widget->devicePixelRatioF();  // Fator de escala do DPI
        int height = metrics.height() * dpiScale;  // Altura ajustada pela escala
        return QSize(option.rect.width(), height);  // Usar a altura ajustada
    }

    // Sobrescrever o paint para customizar a cor de fundo da linha selecionada
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        // Verificar se o item está selecionado e ajustar o fundo
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, m_selectionColor);  // Cor azul para seleção (RGB)
        }

        // Desenhar o item como normalmente
        QStyledItemDelegate::paint(painter, opt, index);
    }

private:
    QColor m_selectionColor;
};

class LogListView : public QListView {
    Q_OBJECT

public:
    explicit LogListView(LogFileModel* model, QWidget *parent = nullptr)
        : QListView(parent), userAtEnd(true) {
        setModel(model);
        connect(model, &LogFileModel::onBeforeLinesAppended, this, &LogListView::checkScrollPositionBeforeInsert);
        connect(model, &LogFileModel::onLinesAppended, this, &LogListView::scrollToBottomIfNeeded);

        model->Start();

        setItemDelegate(new LogItemDelegate(this));  // Usar um delegate padrão

        // Ajustar o modo de redimensionamento
        setUniformItemSizes(true);
        setFlow(QListView::TopToBottom);
        setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        setWrapping(false);
    }

private slots:
    // Verificar se o usuário está no final da lista antes de adicionar novas linhas
    void checkScrollPositionBeforeInsert() {
        QScrollBar *scrollBar = verticalScrollBar();
        userAtEnd = (scrollBar->value() == scrollBar->maximum());
        setUpdatesEnabled(false);
    }

    // Rolar para o final após novas linhas serem adicionadas, se o usuário estava no final
    void scrollToBottomIfNeeded() {
        if (userAtEnd) {
            //scrollToBottom(); // Calling it here causes severe performance degradation on large files...
            QMetaObject::invokeMethod(this, "scrollToBottom", Qt::QueuedConnection);
        }
        setUpdatesEnabled(true);
    }

private:
    bool userAtEnd;
};

#endif // LOGLISTVIEW_H
