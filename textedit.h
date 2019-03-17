#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QTextEdit>
#include <QKeyEvent>
#include <QDropEvent>

#include "util.h"

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

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void dropEvent(QDropEvent *e) override;

private slots:
    void onDocumentContentsChange(int position, int charRemoved, int charAdded);
    void onEditCursorPositionChanged();
    void onSelectionChanged();

private:
    void addChars(int position, int charAdded);
    void removeChars(int position, int charRemoved);
    void replaceChars(int position, int charRemoved, int charAdded);
    void moveChars(int initialStart, int finalEnd);

private:
    QString m_lastText;
    util::Timer m_keyReactionTimer;
    bool m_hasSelection;
    QString m_selectionString;
    int m_selectionStart;
    QString m_dropString;
    QTextDocument m_lastDoc;
    QTextCursor m_lastDocCursor;
    DropStatus m_dropStatus;
};

#endif // TEXTEDIT_H
