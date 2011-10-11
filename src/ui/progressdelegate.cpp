#include "progressdelegate.h"

#include <QDebug>

ProgressDelegate::ProgressDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

void ProgressDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == 6) {
        QStyleOptionProgressBar progressBarOption;
        progressBarOption.rect = option.rect;
        progressBarOption.minimum = 0;
        progressBarOption.maximum = 100;
        progressBarOption.textVisible = true;

        if (index.data(Qt::UserRole).toInt() == 0) { // progress
            int progress = index.data().toInt();
            progressBarOption.progress = progress;
            progressBarOption.text = QString::number(progress) + "%";
        } else { //waiting
            progressBarOption.progress = 0;
            progressBarOption.text = "waiting";
        }

        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}
