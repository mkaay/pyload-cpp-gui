#include "speedwidget.h"
#include "downloadsmodel.h"
#include <QDebug>

SpeedWidget::SpeedWidget(QWidget *parent) : QWidget(parent)
{
    for (int i = 0; i < 20; i++) {
        speeds << 0;
    }
}

void SpeedWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //painter.fillRect(QRect(QPoint(0, 0), size()), QColor("white"));

    int x = 0;
    int max = 0;

    foreach (int speed, speeds) {
        if (max < speed) {
            max = speed;
        }
    }

    if (max > 0) {
        painter.setPen(QColor("black"));

        painter.drawLine(0, 8, 195, 8);

        painter.drawText(200, 13, QString("max."));
        painter.drawText(235, 13, QString("%1/s").arg(DownloadsModel::formatSize(max, 0)));

        painter.drawText(200, height()-5, QString("curr."));
        painter.drawText(235, height()-5, QString("%1/s").arg(DownloadsModel::formatSize(speeds.last(), 0)));
    }

    painter.setPen(QColor("grey"));

    QPainterPath path;
    path.moveTo(0, height());

    qreal scale = static_cast<qreal>(max) / (height() - 9);
    foreach(int speed, speeds) {
        path.lineTo(x, height() - (speed/scale));
        x += 10;
    }
    path.lineTo(x-10, height());
    path.lineTo(0, height());

    QLinearGradient grad(0, 0, 0, height());
    grad.setColorAt(0, QColor(0x03, 0x7a, 0xff, 150));
    grad.setColorAt(1, QColor(0x03, 0x10, 0xff, 150));

    painter.fillPath(path, QBrush(grad));
    painter.drawPath(path);

    //painter.setPen(QColor("black"));
    //painter.drawRect(0, 0, width(), height());

    painter.end();
}

void SpeedWidget::addSpeed(int speed)
{
    speeds << speed;
    if (speeds.size() > 20) {
        speeds.removeFirst();
    }
    update();
}

QSize SpeedWidget::sizeHint() const
{
    return QSize(300, 42);
}
