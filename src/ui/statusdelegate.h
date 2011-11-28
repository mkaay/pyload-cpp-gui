#ifndef STATUSDELEGATE_H
#define STATUSDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

class StatusDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit StatusDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

#endif // STATUSDELEGATE_H
