#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow;  }
QT_END_NAMESPACE

class QSystemTrayIcon;
class QuickSystemTray;
class QCloseEvent;
class QToolButton;
class QuickTranslateWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void sysTrayCreate();
    void initToolView();

signals:
    void sigTranslate(const QString&);

public slots:
    void onExit();
    void onShow();
    void slotSearchButtonClicked();
    void slotResult(const QString&);


private:
    Ui::MainWindow            *ui;
    QMap<QObject*, QWidget*>  toolMap;
    QuickTranslateWindow*     transUi;
    QToolButton*              preToolButton;
    bool                      isExit;
};
#endif // MAINWINDOW_H
