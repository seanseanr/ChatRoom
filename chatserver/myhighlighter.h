#ifndef MYHIGHLIGHTER_H
#define MYHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QTextEdit>
#include <QTcpSocket>

class MyHighlighter : public QSyntaxHighlighter
{
public:
    QString HiUser = QString();
    explicit MyHighlighter(QTextDocument* parent = 0, const QString &k = QString()): QSyntaxHighlighter(parent) {keyword = k;}
    void updateHiUser(QString hiuser){HiUser = hiuser;}
protected:
    void highlightBlock(const QString& text)
    {
        QTextCharFormat keywordFormat;
        keywordFormat.setForeground(Qt::blue);
        keywordFormat.setFontWeight(QFont::Bold);

        QRegExp HiUserPattern("^\\b" + HiUser + "\\b+.*");
        QTextCharFormat HiUserFormat;
        HiUserFormat.setForeground(Qt::red);
        HiUserFormat.setFontWeight(QFont::Bold);

        QRegExp keywordPattern("^\\b" + keyword + "\\b+.*");
        int index = text.indexOf(keywordPattern);
        while(index >= 0)
        {
            int keywordLength = keywordPattern.matchedLength();
            setFormat(index, keywordLength, keywordFormat);
            index = text.indexOf(keywordPattern, index + keywordLength);
        }

        if(HiUser.isEmpty())
            return;
        index = text.indexOf(HiUserPattern);
        while(index >= 0)
        {
            int HiUserLength = HiUserPattern.matchedLength();
            setFormat(index, HiUserLength, HiUserFormat);
            index = text.indexOf(HiUserPattern, index + HiUserLength);
        }


    }

private:
    QString keyword;
};

#endif // MYHIGHLIGHTER_H
