#include "mainwindow.h"
#include "quicktranslatenativeeventfilter.h"
#include <QApplication>
#include <QSharedMemory>

int main(int argc, char *argv[])
{
    QSharedMemory singleton("QuickTranslate");
    if(!singleton.create(1))
    {
        return 0;
    }

    QApplication a(argc, argv);
    a.installNativeEventFilter(QuickTranslateNativeEventFilter::getQuickTranslateNativeEventFilterInstance());

    MainWindow w;

    //w.show();
    return a.exec();
}
