#ifndef HISTORYITEM_H
#define HISTORYITEM_H

#include <QString>
#include <string>

struct HistoryItem
{
public:
    enum EditType
    {
        NoEdit,
        Add,
        Remove,
        Replace
    };

public:
    HistoryItem();
    QString toString()
    {
        switch (m_type)
        {
        case NoEdit:
            return "No Edit";
            break;
        case Add:
            return QString("%1 +%2 %3").arg(m_pos).arg(m_length1).arg(m_string1);
            break;
        case Remove:
            return QString("%1 -%2 %3").arg(m_pos).arg(m_length1).arg(m_string1);
            break;
        case Replace:
            return QString("%1 -%2/+%3 %4->%5").arg(m_pos).arg(m_length1).arg(m_length2).arg(m_string1).arg(m_string2);
            break;
        default:
            return "";
            break;
        }
    }

public:
    EditType m_type;
    int m_pos;

    int m_length1;
    QString m_string1;

    int m_length2;
    QString m_string2;
};

#endif // HISTORYITEM_H
