#ifndef PROGRESSDELEGATE_H
#define PROGRESSDELEGATE_H

#include <QStyledItemDelegate>
#include <QApplication>

class ProgressDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ProgressDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // PROGRESSDELEGATE_H
