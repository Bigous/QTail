#ifndef LOGLISTVIEW_H
#define LOGLISTVIEW_H

#include <QListView>
#include <QScrollBar>
#include <QRegularExpression>

#include "LogFileModel.hpp"
#include "LogItemDelegate.hpp"

class LogListView : public QListView {
    Q_OBJECT

public:
    explicit LogListView(LogFileModel* model, QWidget *parent = nullptr)
        : QListView(parent), userAtEnd(true) {
        setModel(model);
        connect(model, &LogFileModel::onBeforeLinesAppended, this, &LogListView::checkScrollPositionBeforeInsert);
        connect(model, &LogFileModel::onLinesAppended, this, &LogListView::scrollToBottomIfNeeded);

        model->Start();

        m_delegate = new LogItemDelegate(this);

        m_delegate->addHighlightRule(QRegularExpression("[\u2400-\u240F]"), true, Qt::red);

        setItemDelegate(m_delegate);

        // Ajustar o modo de redimensionamento
        setUniformItemSizes(true);
        setFlow(QListView::TopToBottom);
        setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        setWordWrap(false);
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
    LogItemDelegate *m_delegate;
    bool userAtEnd;
};

#endif // LOGLISTVIEW_H
