#ifndef DOWNLOADSVIEW_H
#define DOWNLOADSVIEW_H

#include <QTreeWidget>
#include <QMenu>
#include <QContextMenuEvent>

#include "downloadsmodel.h"

class DownloadsView : public QTreeView
{
    Q_OBJECT
public:
    explicit DownloadsView(QWidget *parent = 0);

protected:
    void contextMenuEvent(QContextMenuEvent *event);

private:
    QModelIndexList selection;

};

#endif // DOWNLOADSVIEW_H
