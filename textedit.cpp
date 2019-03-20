#include "textedit.h"
#include "historyitem.h"

#include <QTextDocument>
#include <QDebug>
#include <QDate>
#include <iostream>
#include <QMimeData>

#include <list>

TextEdit::TextEdit(QWidget *parent)
    : QTextEdit(parent)
    , m_lastDoc("")
{
    connect(this->document(), &QTextDocument::contentsChange, this, &TextEdit::onDocumentContentsChange);
}

void TextEdit::onDocumentContentsChange(int position, int oldLength, int newLength)
{
    qDebug() << position;
    qDebug() << "-" << oldLength;
    qDebug() << "+" << newLength;
    qDebug() << "**********";

    assert(oldLength != 0 || newLength != 0);

    std::list<QChar> oldList;
    std::list<QChar> newList;

    for (int i = position; i < position + oldLength; i++)
    {
        QChar c = m_lastDoc.characterAt(i);
        if (c.isNull())
        {
            break;
        }
        else
        {
            oldList.push_back(c);
        }
    }
    for (int i = position; i < position + newLength; i++)
    {
        QChar c = document()->characterAt(i);
        if (c.isNull())
        {
            break;
        }
        else
        {
            newList.push_back(c);
        }
    }

    int beginEqual = 0;
    int endEqual = 0;
    while (true)
    {
        auto oldEndIter = oldList.rbegin();
        auto newEndIter = newList.rbegin();

        if (oldEndIter != oldList.rend()
            && newEndIter != newList.rend()
            && *oldEndIter == *newEndIter)
        {
            oldList.pop_back();
            newList.pop_back();
            ++endEqual;
        }
        else
        {
            break;
        }
    }

    while (true)
    {
        auto oldBeginIter = oldList.begin();
        auto newBeginIter = newList.begin();

        if (oldBeginIter != oldList.end()
            && newBeginIter != newList.end()
            && *oldBeginIter == *newBeginIter)
        {
            oldList.pop_front();
            newList.pop_front();
            ++beginEqual;
        }
        else
        {
            break;
        }
    }

    QString oldDiff;
    QString newDiff;

    for (auto &c : oldList)
    {
        oldDiff += c;
    }
    for (auto &c : newList)
    {
        newDiff += c;
    }

    HistoryItem item;

    if (newDiff.size() == 0 && oldDiff.size() == 0)
    {
        item.m_type = HistoryItem::NoEdit;
    }
    else if (oldDiff.size() == 0)
    {
        item.m_type = HistoryItem::Add;
        item.m_pos = position + beginEqual;
        item.m_length1 = newDiff.size();
        item.m_string1 = newDiff;
    }
    else if (newDiff.size() == 0)
    {
        item.m_type = HistoryItem::Remove;
        item.m_pos = position + beginEqual;
        item.m_length1 = oldDiff.size();
        item.m_string1 = oldDiff;
    }
    else
    {
        item.m_type = HistoryItem::Replace;
        item.m_pos = position + beginEqual;
        item.m_length1 = oldDiff.size();
        item.m_string1 = oldDiff;
        item.m_length2 = newDiff.size();
        item.m_string2 = newDiff;
    }

    qDebug() << item.toString();

    redo(&item, &m_lastDoc);
}

void TextEdit::redo(const HistoryItem *item, QTextDocument *doc)
{
    QTextCursor cursor(doc);
    switch (item->m_type)
    {
    case HistoryItem::NoEdit:
        break;
    case HistoryItem::Add:
        cursor.setPosition(item->m_pos, QTextCursor::MoveAnchor);
        cursor.insertText(item->m_string1);
        break;
    case HistoryItem::Remove:
        cursor.setPosition(item->m_pos, QTextCursor::MoveAnchor);
        cursor.setPosition(item->m_pos + item->m_length1, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        break;
    case HistoryItem::Replace:
        cursor.setPosition(item->m_pos, QTextCursor::MoveAnchor);
        cursor.setPosition(item->m_pos + item->m_length1, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        cursor.setPosition(item->m_pos, QTextCursor::MoveAnchor);
        cursor.insertText(item->m_string2);
        break;
    default:
        break;
    }
}
