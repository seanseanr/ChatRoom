#ifndef MYHIGHLIGHTER_H
#define MYHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QTextEdit>
class MyHighlighter : public QSyntaxHighlighter
{
public:
    explicit MyHighlighter(QTextDocument* parent = 0): QSyntaxHighlighter(parent){}
protected:
    void highlightBlock(const QString& text)
    {
        QString keyword = "BigOne";
        QTextCharFormat keywordFormat;
        keywordFormat.setForeground(Qt::blue);
        keywordFormat.setFontWeight(QFont::Bold);

        QRegExp keywordPattern("\\b(" + keyword + ")\\b");
        int index = text.indexOf(keywordPattern);
        while(index >= 0)
        {
            int keywordLength = keywordPattern.matchedLength();
            setFormat(index, keywordLength, keywordFormat);
            index = text.indexOf(keywordPattern, index + keywordLength);
        }


    }
};

#endif // MYHIGHLIGHTER_H
