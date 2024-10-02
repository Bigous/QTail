#ifndef LOGITEMDELEGATE_HPP
#define LOGITEMDELEGATE_HPP

#include <QStyledItemDelegate>
#include <QFontMetrics>
#include <QPainter>
#include <QRegularExpression>

// Delegate personalizado para ajustar a altura das linhas
class LogItemDelegate : public QStyledItemDelegate {
public:
    explicit LogItemDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent), m_selectionColor(0, 96, 192) {}

    void addHighlightRule(const QRegularExpression &regex,
                          bool useBackgroundColor = true,
                          const QColor &backgroundColor = Qt::yellow,
                          bool useForegroundColor = false,
                          const QColor &foregroundColor = Qt::black
                          ) {
        m_highlightRules.push_front({regex, useForegroundColor, foregroundColor, useBackgroundColor, backgroundColor});
    }

    // Sobrescrever o sizeHint para ajustar a altura de cada item
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QFontMetrics metrics(option.font);
        //qreal dpiScale = option.widget->devicePixelRatioF();  // Fator de escala do DPI

        // Obtém o texto do item para calcular sua largura
        QString itemText = index.data(Qt::DisplayRole).toString();

        // Calcula a largura necessária com base no conteúdo do texto
        int textWidth = metrics.horizontalAdvance(itemText);// * dpiScale;

        // Calcula a altura com base no DPI
        int height = metrics.height();// * dpiScale;

        // Retorna a largura baseada no conteúdo e a altura ajustada
        return QSize(textWidth, height);
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

        QRect lineRect(x, opt.rect.y(),
                       metrics.horizontalAdvance(displayText), opt.rect.height());

        painter->drawText(lineRect, Qt::AlignLeft | Qt::AlignVCenter, displayText);

        // Iterar sobre as regras de highlight
        for (const auto &rule : m_highlightRules) {
            QRegularExpressionMatchIterator it = rule.regex.globalMatch(displayText);
            while (it.hasNext()) {
                QRegularExpressionMatch match = it.next();
                int start = match.capturedStart();

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


#endif // LOGITEMDELEGATE_HPP
