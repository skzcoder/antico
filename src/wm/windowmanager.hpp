#ifndef _WINDOWMANAGER_HPP
#define _WINDOWMANAGER_HPP

#include <QObject>
#include <QMap>

class Client;

union _XEvent; // avoid X11 include

class WindowManager: public QObject
{
public:
    WindowManager();

    void init();

    bool x11EventFilter(_XEvent *e);

    static WindowManager *self();

private:
    Client *createClient(Qt::HANDLE winId);
    inline Client *findClient(Qt::HANDLE winId) { return _clients.value(winId); }

private:
    QMap<Qt::HANDLE, Client *> _clients;
};

#endif
