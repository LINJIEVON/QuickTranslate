#ifndef QUICKTRANSLATENATIVEEVENTFILTER_H
#define QUICKTRANSLATENATIVEEVENTFILTER_H

#include <QObject>
#include <QAbstractNativeEventFilter>

class QuickTranslateNativeEventFilter :public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    enum{
        HOTKEY_CTRL_SHIFT_F = 0xF1,
    };

public:
    static QuickTranslateNativeEventFilter* getQuickTranslateNativeEventFilterInstance()
    {
        static QuickTranslateNativeEventFilter quickTransNativeEventFilter;
        return &quickTransNativeEventFilter;
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;



signals:
    void sigHotKey(int);
    void sigfocusOut();
    void sigWindowDeactivate();


private:
    QuickTranslateNativeEventFilter();
    void registerHotKey();

    QuickTranslateNativeEventFilter(QuickTranslateNativeEventFilter&) = delete;
    QuickTranslateNativeEventFilter& operator=(const QuickTranslateNativeEventFilter&) = delete;

    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
};

#endif // QUICKTRANSLATENATIVEEVENTFILTER_H
