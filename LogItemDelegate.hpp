#ifndef LOGITEMDELEGATE_HPP
#define LOGITEMDELEGATE_HPP

#include <QStyledItemDelegate>
#include <QFontMetrics>
#include <QPainter>
#include <QRegularExpression>
#include <QTextLayout>
#include <QApplication>
#include <QPalette>

#include "HighlightRule.hpp"

// Delegate personalizado para ajustar a altura das linhas
class LogItemDelegate : public QStyledItemDelegate {
public:
    explicit LogItemDelegate(QList<HighlightRule> *highlightRules, QObject *parent = nullptr)
        : QStyledItemDelegate(parent), m_selectionColor(0, 96, 192), m_highlightRules(highlightRules) {
        m_hoverColor = QApplication::palette().color(QPalette::Window).lightness() < 128
                           ? m_hoverColor = QColor(100, 100, 100) // Tema escuro - cor mais clara para hover
                           : QColor(220, 220, 220); // Tema claro - cor mais escura para hover
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

        painter->save();  // Salvar o estado atual do QPainter

        // Verificar se o item está selecionado e ajustar o fundo
        if (opt.state & QStyle::State_Selected) {
            painter->fillRect(opt.rect, m_selectionColor);  // Cor azul para seleção
        } else if (opt.state & QStyle::State_MouseOver) {
            painter->fillRect(opt.rect, m_hoverColor);  // Cor para hover
        }

        if (m_highlightRules->isEmpty()) {
            // Se não houver regras de highlight, desenhar o texto normalmente
            QStyledItemDelegate::paint(painter, opt, index);
            painter->restore();
            return;
        }

        const QString &displayText = opt.text;

        QFontMetrics metrics(opt.font);

        // Criar um QTextLayout para renderizar o texto com múltiplos formatos
        QTextLayout textLayout(displayText, opt.font);
        textLayout.setTextOption(QTextOption(Qt::AlignLeft | Qt::AlignVCenter));
        QVector<QTextLayout::FormatRange> formats;

        // Iterar sobre as regras de highlight na ordem inversa e aplicar os destaques
        for (auto it = m_highlightRules->rbegin(); it != m_highlightRules->rend(); ++it) {
            const auto &rule = *it;
            QRegularExpressionMatchIterator matchIterator = rule.regex.globalMatch(displayText);
            while (matchIterator.hasNext()) {
                QRegularExpressionMatch match = matchIterator.next();
                QTextLayout::FormatRange formatRange;
                formatRange.start = match.capturedStart();
                formatRange.length = match.capturedLength();
                if (rule.useForegroundColor) {
                    formatRange.format.setForeground(rule.foregroundColor);
                }
                if (rule.useBackgroundColor) {
                    formatRange.format.setBackground(rule.backgroundColor);
                }
                formats.append(formatRange);
            }
        }

        textLayout.setFormats(formats);
        textLayout.beginLayout();
        QTextLine line = textLayout.createLine();
        if (line.isValid()) {
            line.setLineWidth(opt.rect.width());
            line.setPosition(QPointF(0, 0));
        }
        textLayout.endLayout();

        // Desenhar o layout do texto na posição correta
        painter->translate(opt.rect.topLeft());
        textLayout.draw(painter, QPointF(0, (opt.rect.height() - metrics.height()) / 2));
        painter->translate(-opt.rect.topLeft());

        painter->restore();  // Restaurar o estado do QPainter
    }

private:
    QColor m_selectionColor;
    QColor m_hoverColor;
    QList<HighlightRule> *m_highlightRules;
};

#endif // LOGITEMDELEGATE_HPP
