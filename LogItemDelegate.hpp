#ifndef LOGITEMDELEGATE_HPP
#define LOGITEMDELEGATE_HPP

#include <QStyledItemDelegate>
#include <QFontMetrics>
#include <QPainter>
#include <QRegularExpression>

#include "HighlightRule.hpp"

// Delegate personalizado para ajustar a altura das linhas
class LogItemDelegate : public QStyledItemDelegate {
public:
    explicit LogItemDelegate(QList<HighlightRule> *highlightRules, QObject *parent = nullptr)
        : QStyledItemDelegate(parent), m_selectionColor(0, 96, 192), m_highlightRules(highlightRules) {
    }

    // Sobrescrever o sizeHint para ajustar a altura de cada item
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QFontMetrics metrics(option.font);

        // Obtém o texto do item para calcular sua largura
        int size = index.data(Qt::SizeHintRole).toInt();

        // Calcula a largura necessária com base no conteúdo do texto
        int textWidth = metrics.horizontalAdvance('a') * size;

        // Calcula a altura com base no DPI
        int height = metrics.height();

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

        if (m_highlightRules->isEmpty()) {
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
        for(auto size = m_highlightRules->size() -1; size >= 0; --size) {
            const auto &rule = m_highlightRules->at(size);
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

    QColor m_selectionColor;
    QList<HighlightRule> *m_highlightRules;
};


#endif // LOGITEMDELEGATE_HPP
