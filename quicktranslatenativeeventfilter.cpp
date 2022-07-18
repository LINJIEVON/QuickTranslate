#include "quicktranslatenativeeventfilter.h"
#include <windows.h>

#include <QByteArray>
#include <QDebug>
#include <QEvent>

bool QuickTranslateNativeEventFilter::eventFilter(QObject *watched, QEvent *event)
{

    switch(event->type())
    {
    case QEvent::WindowDeactivate:
        emit sigWindowDeactivate();
        break;

    case QEvent::FocusOut:
        emit sigfocusOut();
        break;

    default:
        //qDebug()<<"event type: "<<event->type();
        break;
    }

    return QObject::eventFilter(watched, event);
}

QuickTranslateNativeEventFilter::QuickTranslateNativeEventFilter()
{
    registerHotKey();
}

void QuickTranslateNativeEventFilter::registerHotKey()
{
    if(!RegisterHotKey(nullptr, HOTKEY_CTRL_SHIFT_F, MOD_CONTROL | MOD_SHIFT, 'F'))               //ctrl+shit+f
    {
        qDebug()<<"register hot key ctrl+shit+f failed";
    }
}

bool QuickTranslateNativeEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(result)

    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        MSG* pMsg = static_cast<MSG*>(message);

        if(WM_HOTKEY == pMsg->message)
        {
            if(HOTKEY_CTRL_SHIFT_F == pMsg->wParam)
            {
                //qDebug()<<"hot key ctrl+shit+f";
                emit sigHotKey(HOTKEY_CTRL_SHIFT_F);
            }
        }

        if(WM_MBUTTONDOWN == pMsg->message)
        {
           qDebug()<<"mouse down";
        }
    }
    return false;
}





















