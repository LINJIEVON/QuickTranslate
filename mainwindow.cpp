#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "quickfile.h"
#include "quicktranslate.h"
#include "quicksystemtray.h"
#include "quicktranslatewindow.h"
//#include "quicktranslatenet.h"
#include "quicktranslatedicmodel.h"

#include <QDebug>
#include <QTextCodec>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , toolMap()
    , transUi(new QuickTranslateWindow(this))
    , preToolButton(nullptr)
    , isExit(false)
{
    sysTrayCreate();

    setWindowIcon(QIcon(":/image/image/icon.png"));
    ui->setupUi(this);

    initToolView();

    //netTranslateTest();

    //testQuickFile();
}

MainWindow::~MainWindow()
{
    delete ui;
    qDebug()<<"destroy MainWindow";
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(isExit)
    {
        event->accept();                               //默认行为（关闭）
    }
    else
    {
        transUi->setIsOnShow(true);
        event->ignore();                                //忽略关闭事件
        hide();                                         //将窗口隐藏
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    //qDebug()<<event->type();
    if(QEvent::MouseButtonPress == event->type())
    {
         QToolButton* currToolButton = (QToolButton*)watched;
         if(preToolButton != currToolButton)
         {
             auto iter = toolMap.find(preToolButton);
             if(preToolButton->isChecked())
             {
                //qDebug()<<preToolButton<<" pre checked";
                preToolButton->setChecked(false);
                if(toolMap.end() != iter)
                {
                    iter.value()->hide();
                }
             }

             //qDebug()<<currToolButton<<"new checked";
             iter = toolMap.find(currToolButton);
             if(toolMap.end() != iter)
             {
                 iter.value()->show();
             }

             return true;
         }
         else
         {
             return true;
         }
    }
    else if(QEvent::MouseButtonRelease == event->type())
    {
        QToolButton* currToolButton = (QToolButton*)watched;
        //qDebug()<<currToolButton<<" release";
        if(preToolButton != currToolButton)
        {
            currToolButton->setChecked(true);
            preToolButton = currToolButton;
            return true;
        }
    }
    else if(QEvent::MouseButtonDblClick == event->type())
    {
        return true;
    }

    return false;
}

void MainWindow::sysTrayCreate()
{
    //翻译UI
    //QuickTranslateWindow* transUi = new QuickTranslateWindow(this);
    //transUi->hide();
    //transUi->show();

    //系统托盘图标
    QuickSystemTray* tray = new QuickSystemTray(this);
    connect(tray, &QuickSystemTray::sigUiShow, this, &MainWindow::onShow);
    connect(tray, &QuickSystemTray::sigExit, this, &MainWindow::onExit);
    connect(tray, &QuickSystemTray::sigTranslate, transUi, &QuickTranslateWindow::onTranslateUi);

    tray->show();
}

void MainWindow::initToolView()
{
    ui->toolButtonDictionary->installEventFilter(this);
    ui->toolButtonSetting->installEventFilter(this);
    ui->toolButtonTranslate->installEventFilter(this);

    QuickTranslateDicModel* dicModel = new QuickTranslateDicModel(this);
    ui->tableViewDictionary->setModel(dicModel);

    ui->tableWidgetSetting->hide();

    ui->frameTranslate->hide();

    preToolButton = ui->toolButtonDictionary;

    toolMap.insert(ui->toolButtonDictionary,  ui->tableViewDictionary);
    toolMap.insert(ui->toolButtonSetting,  ui->tableWidgetSetting);
    toolMap.insert(ui->toolButtonTranslate,  ui->frameTranslate);

    connect(this, &MainWindow::sigTranslate, transUi, &QuickTranslateWindow::slotTranslate);
    connect(transUi, &QuickTranslateWindow::sigResult, this, &MainWindow::slotResult);
}

void MainWindow::onExit()
{
    isExit = true;
    close();
}

void MainWindow::onShow()
{
    transUi->setIsOnShow(false);
    show();
    setFocus();
}

void MainWindow::slotSearchButtonClicked()
{
    QString word = ui->searchEdit->text();
    if(word.length() > 0)
    {
        emit sigTranslate(word);
    }
}

void MainWindow::slotResult(const QString &result)
{
    ui->resultPlainTextEdit->clear();
    if(result.length() > 0)
    {
        QString plainText = result;
        plainText.replace(';', '\n');
        ui->resultPlainTextEdit->setPlainText(plainText);
    }
    else
    {
        ui->resultPlainTextEdit->setPlainText("未查询到！");
    }
}



