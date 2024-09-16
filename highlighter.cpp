#include "highlighter.h"

Highlighter::Highlighter(QTextDocument* document, QString regexPattern, QBrush highlightForeground, QBrush highlightBackground, QObject* parent)
    : QObject(parent),
    document(document),
    regex(regexPattern),
    highlightForeground(highlightForeground),
    highlightBackground(highlightBackground)
{}

void Highlighter::handleNewLine(qint64 position, const QString& line) {
    // Procurar correspondências na nova linha
    QRegularExpressionMatchIterator matchIterator = regex.globalMatch(line);
    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();
        highlightMatch(position, match.capturedStart(), match.capturedLength());
    }
}

void Highlighter::highlightMatch(qint64 position, int startOffset, int length) {
    QTextCursor cursor(document);

    // Mover o cursor para a posição correta no documento e selecionar o match
    cursor.setPosition(position + startOffset);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, length);

    // Aplicar a formatação ao texto destacado
    QTextCharFormat highlightFormat;
    highlightFormat.setForeground(highlightForeground);
    highlightFormat.setBackground(highlightBackground);
    cursor.setCharFormat(highlightFormat);

    qDebug() << "Highlighted text at position:" << position + startOffset;
}
