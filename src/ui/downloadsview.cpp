#include "downloadsview.h"
#include <QDebug>

DownloadsView::DownloadsView(QWidget *parent) : QTreeView(parent)
{

}

void DownloadsView::contextMenuEvent(QContextMenuEvent *event)
{
    DownloadsModel *m = static_cast<DownloadsModel*>(model());
    selection = selectedIndexes();

    bool package = false;
    bool file = false;
    foreach (QModelIndex index, selection) {
        if (m->isPackage(index)) {
            package = true;
            if (file) {
                break;
            }
        } else {
            file = true;
            if (package) {
                break;
            }
        }
    }

    QMenu menu(this);
    if (package) {
        menu.addAction(QIcon(":/icons/cross.png"), tr("Deactivate"));
        menu.addAction(QIcon(":/icons/tick.png"), tr("Activate"));
    }

    menu.addAction(QIcon(":/icons/arrow_refresh.png"), tr("Restart"));
    menu.addAction(QIcon(":/icons/delete.png"), tr("Delete"));
    menu.addAction(QIcon(":/icons/cancel.png"), tr("Abort"));

    if (file) {
        menu.addSeparator();
        menu.addAction(QIcon(":/icons/bullet_go.png"), tr("Force Start"));
        QAction *openlink = menu.addAction(QIcon(":/icons/world_link.png"), tr("Open in Browser"));
        if (selection.size() != m->columnCount()) {
            openlink->setDisabled(true);
        }
    }
    if (package) {
        menu.addSeparator();
        QMenu *priority = menu.addMenu("Priorty");
        priority->addAction(QIcon(":/icons/arrow_double_up.png"), tr("Highest"));
        priority->addAction(QIcon(":/icons/arrow_up.png"), tr("Higher"));
        priority->addAction(QIcon(":/icons/bullet_go.png"), tr("Normal"));
        priority->addAction(QIcon(":/icons/arrow_down.png"), tr("Lower"));
        priority->addAction(QIcon(":/icons/arrow_double_down.png"), tr("Lowest"));
        menu.addSeparator();
        menu.addAction(QIcon(":/icons/key.png"), tr("Set Password"));
        menu.addAction(QIcon(":/icons/folder_explore.png"), tr("Change Download Folder"));
        QAction *opensource = menu.addAction(QIcon(":/icons/world_link.png"), tr("Open Download Source"));
        if (selection.size() != m->columnCount()) {
            opensource->setDisabled(true);
        }
    }
    menu.exec(event->globalPos());
}
