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

    void addHighlightRule(const QRegularExpression &regex,
                          bool useBackgroundColor = true,
                          const QColor &backgroundColor = QColor(255, 255, 0),
                          bool useForegroundColor = false,
                          const QColor &foregroundColor = Qt::black
                          ) {
        m_highlightRules.push_front({regex, useForegroundColor, foregroundColor, useBackgroundColor, backgroundColor});
    }

    // Sobrescrever o sizeHint para ajustar a altura de cada item
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QFontMetrics metrics(option.font);
        qreal dpiScale = option.widget->devicePixelRatioF();  // Fator de escala do DPI
        int height = metrics.height() * dpiScale;  // Altura ajustada pela escala
        return QSize(option.rect.width(), height);  // Usar a altura ajustada
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        // Verificar se o item está selecionado e ajustar o fundo
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, m_selectionColor);  // Cor azul para seleção
        }

        if (m_highlightRules.isEmpty()) {
            // Se não houver regras de highlight, desenhar o texto normalmente
            QStyledItemDelegate::paint(painter, opt, index);
            return;
        }

        QString displayText = opt.text;

        painter->save();  // Salvar o estado atual do QPainter

        QFontMetrics metrics(opt.font);

        // Posição inicial de x
        int x = opt.rect.x() + opt.widget->style()->pixelMetric(QStyle::PM_FocusFrameHMargin, &opt, opt.widget);

        // Inicializar uma lista de intervalos destacados
        QList<QPair<int, int>> matchedRanges;  // Guardará os intervalos correspondentes e suas cores

        // Iterar sobre as regras de highlight
        for (const auto &rule : m_highlightRules) {
            QRegularExpressionMatchIterator it = rule.regex.globalMatch(displayText);
            while (it.hasNext()) {
                QRegularExpressionMatch match = it.next();
                int start = match.capturedStart();
                int length = match.capturedLength();

                // Armazenar o intervalo e as informações da cor
                matchedRanges.append(qMakePair(start, length));

                // Desenhar o texto destacado
                QString matchedText = match.captured();
                QRect matchRect(x + metrics.horizontalAdvance(displayText.left(start)), opt.rect.y(),
                                metrics.horizontalAdvance(matchedText), opt.rect.height());

                // Desenhar o fundo da correspondência
                if(rule.useBackgroundColor)
                    painter->fillRect(matchRect, rule.backgroundColor);

                // Desenhar o texto da correspondência
                if(rule.useForegroundColor)
                    painter->setPen(rule.foregroundColor);

                painter->drawText(matchRect, Qt::AlignLeft | Qt::AlignVCenter, matchedText);
            }
        }

        // Desenhar o restante do texto que não foi destacado
        int lastIndex = 0;
        for (const auto &range : matchedRanges) {
            int start = range.first;
            if (lastIndex < start) {
                QString normalText = displayText.mid(lastIndex, start - lastIndex);
                QRect textRect(x + metrics.horizontalAdvance(displayText.left(lastIndex)), opt.rect.y(),
                               metrics.horizontalAdvance(normalText), opt.rect.height());

                // Desenhar o texto normal
                painter->setPen(opt.palette.text().color());
                painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, normalText);
            }
            lastIndex = start + range.second;
        }

        // Desenhar o texto restante após a última correspondência
        if (lastIndex < displayText.length()) {
            QString remainingText = displayText.mid(lastIndex);
            QRect textRect(x + metrics.horizontalAdvance(displayText.left(lastIndex)), opt.rect.y(),
                           metrics.horizontalAdvance(remainingText), opt.rect.height());
            painter->setPen(opt.palette.text().color());
            painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, remainingText);
        }

        // Restaurar o estado do painter
        painter->restore();
    }
private:
    struct HighlightRule {
        QRegularExpression regex;   // Expressão regular para correspondência
        bool useForegroundColor;
        QColor foregroundColor;     // Cor do texto
        bool useBackgroundColor;
        QColor backgroundColor;     // Cor do fundo
    };

    QColor m_selectionColor;
    QList<HighlightRule> m_highlightRules;
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

        m_delegate = new LogItemDelegate(this);

        m_delegate->addHighlightRule(QRegularExpression("[\u2400-\u240F]"), true, QColor(255, 0, 0), false, Qt::black);

        setItemDelegate(m_delegate);

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
    LogItemDelegate *m_delegate;
    bool userAtEnd;
};

#endif // LOGLISTVIEW_H
