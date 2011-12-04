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
        if (DownloadsModel::isPackage(index)) {
            statusrect.setRight(statusrect.right() - 20);
        }
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
                iconstr = ":/icons/bullet_go.png";
            } else if (package->getStatus() == Finished) {
                iconstr = ":/icons/bullet_green.png";
            } else if (package->getStatus() == Failed || package->getStatus() == Offline) {
                iconstr = ":/icons/bullet_red.png";
            } else if (package->getStatus() == Skipped) {
                iconstr = ":/icons/bullet_green.png";
            } else if (package->getStatus() == Processing) {
                iconstr = ":/icons/bullet_yellow.png";
            } else if (package->getStatus() == Waiting) {
                iconstr = ":/icons/clock.png";
            }
        } else {
            File *file = DownloadsModel::fileFromIndex(index);
            if (file->getStatus() == Downloading) {
                iconstr = ":/icons/bullet_go.png";
            } else if (file->getStatus() == Finished) {
                iconstr = ":/icons/bullet_green.png";
            } else if (file->getStatus() == Failed || file->getStatus() == Offline) {
                iconstr = ":/icons/bullet_red.png";
            } else if (file->getStatus() == Skipped) {
                iconstr = ":/icons/bullet_green.png";
            } else if (file->getStatus() == Processing) {
                iconstr = ":/icons/bullet_yellow.png";
            } else if (file->getStatus() == Waiting) {
                iconstr = ":/icons/clock.png";
            }
        }
        painter->drawPixmap(iconrect, QPixmap(iconstr));

        if (DownloadsModel::isPackage(index)) {
            Package *package = DownloadsModel::packageFromIndex(index);
            QRect priorityrect = option.rect;
            priorityrect.setRight(priorityrect.right()-2);
            priorityrect.setTop(priorityrect.top()+1);
            priorityrect.setBottom(priorityrect.top()+16);
            priorityrect.setLeft(priorityrect.right()-16);

            if (package->getPriority() >= 2) {
                painter->drawPixmap(priorityrect, QPixmap(":/icons/arrow_double_up.png"));
            } else if (package->getPriority() == 1) {
                painter->drawPixmap(priorityrect, QPixmap(":/icons/arrow_up.png"));
            } else if (package->getPriority() == -1) {
                painter->drawPixmap(priorityrect, QPixmap(":/icons/arrow_down.png"));
            } else if (package->getPriority() <= -2) {
                painter->drawPixmap(priorityrect, QPixmap(":/icons/arrow_double_down.png"));
            }

        }
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}
