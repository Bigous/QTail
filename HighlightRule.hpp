#ifndef HIGHLIGHTRULE_HPP
#define HIGHLIGHTRULE_HPP

#include <QColor>
#include <QRegularExpression>

struct HighlightRule {
    QRegularExpression regex;
    bool useForegroundColor;
    QColor foregroundColor;
    bool useBackgroundColor;
    QColor backgroundColor;
};


#endif // HIGHLIGHTRULE_HPP
