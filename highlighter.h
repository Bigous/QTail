#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QTextCursor>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QTextEdit>
#include <QDebug>

class Highlighter : public QObject {
    Q_OBJECT

public:
    Highlighter(QTextDocument* document, QString regexPattern, QBrush highlightForeground, QBrush highlightBackground, QObject* parent = nullptr);
    virtual ~Highlighter() {}

public slots:
    void handleNewLine(qint64 position, const QString& line);

private:
    void highlightMatch(qint64 position, int startOffset, int length);

private:
    QTextDocument* document;
    QBrush highlightForeground;
    QBrush highlightBackground;
    QRegularExpression regex;
};


#endif // HIGHLIGHTER_H
