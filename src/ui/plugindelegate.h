#ifndef PLUGINDELEGATE_H
#define PLUGINDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

class PluginDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PluginDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:

};

#endif // PLUGINDELEGATE_H
