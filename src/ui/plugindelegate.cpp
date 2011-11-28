#include "plugindelegate.h"
#include "downloadsmodel.h"
#include "../file.h"

PluginDelegate::PluginDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void PluginDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.isValid()) {
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
        }

        QRect pluginrect = option.rect;
        pluginrect.setTop(pluginrect.top()+1);
        pluginrect.setBottom(pluginrect.top()+16);
        pluginrect.setRight(pluginrect.left()+16);

        if (DownloadsModel::isPackage(index)) {
            Package *package = DownloadsModel::packageFromIndex(index);
            foreach (QString plugin, package->getPlugins()) {
                painter->drawPixmap(pluginrect, QPixmap(QString(":/hoster/%1.png").arg(plugin)));
                pluginrect.setLeft(pluginrect.left()+20);
                pluginrect.setRight(pluginrect.left()+16);
            }
        } else {
            File *file = DownloadsModel::fileFromIndex(index);
            painter->drawPixmap(pluginrect, QPixmap(QString(":/hoster/%1.png").arg(file->getPlugin())));
        }
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}
