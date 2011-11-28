#ifndef SPEEDWIDGET_H
#define SPEEDWIDGET_H

#include <QWidget>
#include <QtAlgorithms>
#include <QPainter>
#include <QSize>
#include <QPainterPath>

class SpeedWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SpeedWidget(QWidget *parent = 0);
    void paintEvent(QPaintEvent *event);
    QSize sizeHint() const;

signals:

public slots:
    void addSpeed(int speed);

private:
    QList<int> speeds;

};

#endif // SPEEDWIDGET_H
