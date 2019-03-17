#include "textedit.h"
#include <QTextDocument>
#include <QDebug>
#include <QDate>
#include <iostream>
#include <QMimeData>

TextEdit::TextEdit(QWidget *parent)
    : QTextEdit(parent)
    , m_lastText("")
    , m_hasSelection(false)
    , m_selectionString("")
    , m_selectionStart(-1)
    , m_lastDoc("")
    , m_lastDocCursor(&m_lastDoc)
    , m_dropStatus(NoDrop)
{
    connect(this->document(), &QTextDocument::contentsChange, this, &TextEdit::onDocumentContentsChange);
    connect(this, &TextEdit::cursorPositionChanged, this, &TextEdit::onEditCursorPositionChanged);
    connect(this, &TextEdit::selectionChanged, this, &TextEdit::onSelectionChanged);
}

void TextEdit::keyPressEvent(QKeyEvent *e)
{
    QTextEdit::keyPressEvent(e);
}

void TextEdit::dropEvent(QDropEvent *e)
{
    qDebug() << "drop begin";
    if (e->dropAction() == Qt::MoveAction)
    {
        m_dropStatus = DropMove;
    }
    else if (e->dropAction() == Qt::CopyAction)
    {
        m_dropStatus = DropCopy;
    }
    else
    {
        assert(false);
    }

    const QMimeData *mimeData = e->mimeData();
    m_dropString = mimeData->text();

    QTextEdit::dropEvent(e);
    m_dropStatus = NoDrop;

    qDebug() << "drop end";
}

void TextEdit::onDocumentContentsChange(int position, int charRemoved, int charAdded)
{
    qDebug() << "########## SHOW ##########";
    qDebug() << "current:" << document()->toPlainText();
    qDebug() << "last:" << m_lastDoc.toPlainText();

    qDebug() << position << "->" << textCursor().position();
    qDebug() << "-" << charRemoved;
    qDebug() << "+" << charAdded;

    if (m_dropStatus == DropCopy)
    {
        int dropPosition = position;
        int dropAdded = charAdded - charRemoved;
        addChars(dropPosition, dropAdded);
    }
    else if (m_dropStatus == DropMove)
    {
        int initialStart = m_selectionStart;
        int finalEnd = textCursor().position();
        moveChars(initialStart, finalEnd);
    }
    else {
        if (m_hasSelection)
        {
            if (charRemoved && charAdded)
            {
                if (position == textCursor().position())
                {
                    if (charAdded == charRemoved)
                    {
                        // mouse press->mouse right->ime->mouse to start->mouse release
                    }
                    else
                    {
                        // mouse press->ime->mouse to start->space
                        int actualRemoved = charRemoved - charAdded;
                        removeChars(position, actualRemoved);
                    }
                }
                else
                {
                    if (textCursor().selectedText().size() == 0)
                    {
                        int delta = charAdded - charRemoved;
                        int actualRemoved = m_selectionString.size();
                        int actualAdded = actualRemoved + delta;
                        int actualPos = textCursor().position() - actualAdded;
                        replaceChars(actualPos, actualRemoved, actualAdded);
                    }
                    else
                    {
                        // input "12345"->mouse end->mouse press->ime->mouse 12->mouse release
                        // do nothing
                    }
                }
            }
            else if (charRemoved)
            {
                removeChars(position, charRemoved);
            }
            else if (charAdded)
            {
                assert(false);
            }
            else // !charRemoved && !charAdded
            {
                assert(false);
            }
        }
        else
        {
            // if !m_isSelected && charRemoved && charAdded, there are 2 possibility:
            // 1. input method
            // 2. beginning pos ctrl-v
            // it's sometimes indistinguishable(past a '1' in beginning / insert a '1' in begining under ime)
            if (charRemoved && charAdded)
            {
                int actualAdded = charAdded - charRemoved;
                int actualPos = textCursor().position() - actualAdded;
                if (actualAdded > 0)
                {
                    addChars(actualPos, actualAdded);
                }
            }
            else if (charAdded)
            {
                addChars(position, charAdded);
            }
            else if (charRemoved)
            {
                removeChars(position, charRemoved);
            }
            else // !charRemoved && !charAdded
            {
                assert(false);
            }
        }
    }
    if (document()->toPlainText() != m_lastDoc.toPlainText())
    {
        qDebug() << ">>>>>>>>>> ERROR <<<<<<<<<<";
        qDebug() << "current:" << document()->toPlainText();
        qDebug() << "last:" << m_lastDoc.toPlainText();
    }
}

void TextEdit::onEditCursorPositionChanged()
{

}

void TextEdit::onSelectionChanged()
{
    m_hasSelection = textCursor().hasSelection();
    m_selectionString = textCursor().selectedText();
    m_selectionStart = textCursor().selectionStart();
    qDebug() << m_selectionStart << "selected string" << m_selectionString;
}

void TextEdit::addChars(int position, int charAdded)
{
    QString added;
    for (int i = position; i < position + charAdded; i++)
    {
        added += document()->characterAt(i);
    }
    qDebug() << QString("[%1] +%2 %3").arg(position).arg(charAdded).arg(added);
    m_lastDocCursor.setPosition(position, QTextCursor::MoveAnchor);
    m_lastDocCursor.insertText(added);
}

void TextEdit::removeChars(int position, int charRemoved)
{
    m_lastDocCursor.setPosition(position, QTextCursor::MoveAnchor);
    m_lastDocCursor.setPosition(position + charRemoved, QTextCursor::KeepAnchor);
    QString removed = m_lastDocCursor.selectedText();
    m_lastDocCursor.removeSelectedText();
    qDebug() << QString("[%1] -%2 %3").arg(position).arg(charRemoved).arg(removed);
}

void TextEdit::replaceChars(int position, int charRemoved, int charAdded)
{
    removeChars(position, charRemoved);
    addChars(position, charAdded);
}

void TextEdit::moveChars(int initialStart, int finalEnd)
{
    int selectionLength = m_selectionString.size();
    int dropLength = m_dropString.size();

    if (selectionLength != 0) // normal move
    {
        int finalStart = finalEnd - selectionLength;

        if (initialStart != finalStart) // move toward end
        {
            int removePosition = initialStart;
            int addPosition = finalStart;
            removeChars(removePosition, selectionLength);
            addChars(addPosition, selectionLength);
        }
        else // initialStart == finalStart
        {
            // do nothing
        }
    }
    else // move was interrupted by ime, the initial selection was already removed
    {
        int finalStart = finalEnd - dropLength;
        int addPosition = finalStart;
        addChars(addPosition, dropLength);
    }
}
