#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);
    virtual ~Application();

    virtual bool notify(QObject *receiver, QEvent *event);
};

#endif // APPLICATION_H
