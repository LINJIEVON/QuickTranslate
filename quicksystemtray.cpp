#include "quicksystemtray.h"
#include <QDebug>
#include <QWidget>
#include <QMenu>
#include <QAction>

QuickSystemTray::QuickSystemTray(QObject *parent) :
    QSystemTrayIcon(parent),
    menu_(nullptr)
{
    setIcon(QIcon(":/image/image/icon.png"));
    setToolTip(tr("便捷翻译"));

    createMenu();

    connect(this, &QSystemTrayIcon::activated, this, &QuickSystemTray::onActivated);
}

QuickSystemTray::~QuickSystemTray()
{
    destroyMenu();
    qDebug()<<"destroy QuickSystemTray";
}

void QuickSystemTray::destroyMenu()
{
    if(nullptr != menu_)
    {
        delete menu_;
    }
    qDebug()<<"destroy menu";
}

void QuickSystemTray::createMenu()
{
    menu_ = new QMenu();
    menu_->setMinimumWidth(150);
    menu_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    QAction* actTrans = new QAction(tr("翻译"), this);
    QAction* actSet = new QAction(tr("设置"), this);
    QAction* actExit = new QAction(tr("退出"), this);

    menu_->addAction(actTrans);
    menu_->addSeparator();
    menu_->addAction(actSet);
    menu_->addSeparator();
    menu_->addAction(actExit);

    connect(actTrans, &QAction::triggered, this, &QuickSystemTray::onTranslate);
    connect(actSet, &QAction::triggered, this, &QuickSystemTray::onSetting);
    connect(actExit, &QAction::triggered, this, &QuickSystemTray::onExit);
    setContextMenu(menu_);
}

void QuickSystemTray::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::DoubleClick)
    {
        emit sigUiShow();
    }
    else if(reason == QSystemTrayIcon::Context)
    {
        //qDebug()<<"context menu";
    }
}

void QuickSystemTray::onTranslate()
{
    emit sigTranslate();
}

void QuickSystemTray::onSetting()
{
    emit sigUiShow();
}

void QuickSystemTray::onExit()
{
    setVisible(false);
    emit sigExit();
}
