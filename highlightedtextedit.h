#ifndef HIGHLIGHTEDTEXTEDIT_H
#define HIGHLIGHTEDTEXTEDIT_H

#include <QTextEdit>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QColor>

class HighlightedTextEdit : public QTextEdit {
    Q_OBJECT

public:
    HighlightedTextEdit(QWidget *parent = nullptr);
    virtual ~HighlightedTextEdit() {}

private slots:
    void highlightCurrentLine();
};


#endif // HIGHLIGHTEDTEXTEDIT_H
