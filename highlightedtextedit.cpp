#include "highlightedtextedit.h"

#include <QTextCursor>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QColor>

HighlightedTextEdit::HighlightedTextEdit(QWidget *parent) : QTextEdit(parent) {
    setReadOnly(true);
    setWordWrapMode(QTextOption::NoWrap);
    setLineWrapMode(QTextEdit::NoWrap);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    connect(this, &QTextEdit::cursorPositionChanged, this, &HighlightedTextEdit::highlightCurrentLine);
}

void HighlightedTextEdit::highlightCurrentLine() {
    // Remove o destaque anterior
    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextEdit::ExtraSelection selection;

    // Definir o estilo de fundo para destacar a linha
    static QColor highlightColor(QColor::fromRgb(0, 128, 255, 192));
    selection.format.setBackground(highlightColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);

    // Obter o cursor atual e selecionar a linha completa
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::LineUnderCursor);
    selection.cursor = cursor;

    // Adicionar essa seleção à lista
    extraSelections.append(selection);

    // Aplicar o destaque
    setExtraSelections(extraSelections);
}
