#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QTextEdit>
#include <QKeyEvent>
#include <QDropEvent>

#include "util.h"

struct HistoryItem;

class TextEdit : public QTextEdit
{
public:
    enum DropStatus
    {
        NoDrop,
        DropMove,
        DropCopy
    };
public:
    TextEdit(QWidget *parent = nullptr);
    QTextDocument *lastDocument() { return &m_lastDoc; }

private slots:
    void onDocumentContentsChange(int position, int charRemoved, int charAdded);

private:
    void redo(const HistoryItem *item, QTextDocument *doc);

private:
    QTextDocument m_lastDoc;
};

#endif // TEXTEDIT_H
