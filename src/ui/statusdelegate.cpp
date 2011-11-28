#include "statusdelegate.h"
#include "downloadsmodel.h"
#include "../file.h"

StatusDelegate::StatusDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

void StatusDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.isValid()) {
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
            painter->setPen(option.palette.highlightedText().color());
        } else {
            painter->setPen(option.palette.text().color());
        }

        QRect statusrect = option.rect;
        statusrect.setLeft(statusrect.left() + 20);
        statusrect.setRight(statusrect.right() - 40);
        statusrect.setTop(statusrect.top()+1);
        FileStatus status;
        int speed;
        if (DownloadsModel::isPackage(index)) {
            Package *package = DownloadsModel::packageFromIndex(index);
            status = static_cast<FileStatus>(package->getStatus());
            speed = package->getSpeed();
        } else {
            File *file = DownloadsModel::fileFromIndex(index);
            status = static_cast<FileStatus>(file->getStatus());
            speed = file->getSpeed();
        }
        QString s;
        if (status == Downloading) {
            s = QString("%1 @ %2/s").arg(DownloadsModel::formatStatus(status), DownloadsModel::formatSize(speed));
        } else {
            s = DownloadsModel::formatStatus(status);
        }
        painter->drawText(statusrect, Qt::AlignTop | Qt::AlignLeft, s);

        QRect iconrect = option.rect;
        iconrect.setRight(iconrect.left()+16);
        iconrect.setTop(iconrect.top()+1);
        iconrect.setBottom(iconrect.top()+16);

        QString iconstr;
        if (DownloadsModel::isPackage(index)) {
            Package *package = DownloadsModel::packageFromIndex(index);
            if (package->getStatus() == Downloading) {
                iconstr = ":/icons/icons/bullet_go.png";
            } else if (package->getStatus() == Finished) {
                iconstr = ":/icons/icons/bullet_green.png";
            } else if (package->getStatus() == Failed || package->getStatus() == Offline) {
                iconstr = ":/icons/icons/bullet_red.png";
            } else if (package->getStatus() == Skipped) {
                iconstr = ":/icons/icons/bullet_green.png";
            } else if (package->getStatus() == Processing) {
                iconstr = ":/icons/icons/bullet_yellow.png";
            }
        } else {
            File *file = DownloadsModel::fileFromIndex(index);
            if (file->getStatus() == Downloading) {
                iconstr = ":/icons/icons/bullet_go.png";
            } else if (file->getStatus() == Finished) {
                iconstr = ":/icons/icons/bullet_green.png";
            } else if (file->getStatus() == Failed || file->getStatus() == Offline) {
                iconstr = ":/icons/icons/bullet_red.png";
            } else if (file->getStatus() == Skipped) {
                iconstr = ":/icons/icons/bullet_green.png";
            } else if (file->getStatus() == Processing) {
                iconstr = ":/icons/icons/bullet_yellow.png";
            }
        }
        painter->drawPixmap(iconrect, QPixmap(iconstr));

        if (DownloadsModel::isPackage(index)) {
            Package *package = DownloadsModel::packageFromIndex(index);
            QPixmap negative(":/icons/icons/arrow_down.png");
            QPixmap positive(":/icons/icons/arrow_up.png");

            QRect priorityrect = option.rect;
            priorityrect.setTop(priorityrect.top()+1);
            priorityrect.setBottom(priorityrect.top()+16);
            priorityrect.setLeft(priorityrect.right()-16);

            if (package->getPriority() < 0) {
                painter->drawPixmap(priorityrect, negative);
                if (package->getPriority() == -2) {
                    priorityrect.setLeft(priorityrect.right()-32);
                    priorityrect.setRight(priorityrect.right()-16);
                    painter->drawPixmap(priorityrect, negative);
                }
            }

            if (package->getPriority() > 0) {
                painter->drawPixmap(priorityrect, positive);
                if (package->getPriority() == 2) {
                    priorityrect.setLeft(priorityrect.right()-32);
                    priorityrect.setRight(priorityrect.right()-16);
                    painter->drawPixmap(priorityrect, positive);
                }
            }

        }
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}
