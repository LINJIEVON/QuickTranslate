#ifndef QUICKSYSTEMTRAY_H
#define QUICKSYSTEMTRAY_H

#include <QSystemTrayIcon>

class QWidget;
class QMenu;

class QuickSystemTray : public QSystemTrayIcon
{
    Q_OBJECT
public:
    explicit QuickSystemTray(QObject *parent = nullptr);
    ~QuickSystemTray();

private:
    void createMenu();
    void destroyMenu();

public slots:
    void onActivated(QSystemTrayIcon::ActivationReason);
    void onTranslate();
    void onSetting();
    void onExit();

signals:
    void sigTranslate();
    void sigUiShow();
    void sigExit();


private:
    QWidget*        ui_;
    QMenu*          menu_;
};

#endif // QUICKSYSTEMTRAY_H
