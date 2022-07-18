#include "quicktranslatewindow.h"
#include "ui_translatewindow.h"
#include "quicktranslate.h"
#include "quicktranslatenativeeventfilter.h"

#include <QDebug>
#include <QThread>

QuickTranslateWindow::QuickTranslateWindow(QWidget *parent):
    QWidget(parent),
    translateUi(new Ui::TranslateWindow),
    wholeSize(),
    partSize(),
    isMouseLeaved(false),
    isOnShow(true)
{

    //和QuickTranslate绑定
    QuickTranslate* translater = new QuickTranslate(this);

    connect(this, &QuickTranslateWindow::sigTranslate, translater, &QuickTranslate::slotTranslate);
    connect(this, &QuickTranslateWindow::sigAdd, translater, &QuickTranslate::onAddWord);
    connect(translater, &QuickTranslate::sigResult, this, &QuickTranslateWindow::onResult);

    //初始化Ui控件
    translateUi->setupUi(this);

    //设置过滤热键（ctrl + shift + f）事件
    QuickTranslateNativeEventFilter* filter =
            QuickTranslateNativeEventFilter::getQuickTranslateNativeEventFilterInstance();
    connect(filter, &QuickTranslateNativeEventFilter::sigHotKey,
            this, &QuickTranslateWindow::onHotKey);

    //设置过滤控去焦点事件
    this->installEventFilter(filter);
    connect(filter, &QuickTranslateNativeEventFilter::sigWindowDeactivate,
           this, &QuickTranslateWindow::onHide);

    //设置窗口样式，无边框，透明背景
    setWindowFlags(Qt::Drawer | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    layout()->setSizeConstraint(QLayout::SetFixedSize);

    initChangedSize();
}

QuickTranslateWindow::~QuickTranslateWindow()
{
    qDebug()<<"destroy QuickTranslateWindow";
}

void QuickTranslateWindow::setIsOnShow(bool set)
{
    isOnShow = set;
    if(!isOnShow && isVisible())
    {
        hide();
    }
}

void QuickTranslateWindow::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    isMouseLeaved = true;
}

void QuickTranslateWindow::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    isMouseLeaved = false;
}

void QuickTranslateWindow::drawPartUi()
{
    if(!isOnShow)
    {
        return;
    }

    isMouseLeaved = false;

    //隐藏一些控件
    translateUi->resultPlainTextEdit->clear();
    translateUi->resultPlainTextEdit->hide();

    translateUi->searchEdit->clear();

    resize(partSize);
    show();
    activateWindow();
}

void QuickTranslateWindow::drawWholeUi()
{
    //显示隐藏的控件
    translateUi->resultPlainTextEdit->show();

    resize(wholeSize);
}

void QuickTranslateWindow::initChangedSize()
{
    partSize = translateUi->frame_2->sizeHint();
    wholeSize = this->sizeHint();
}

void QuickTranslateWindow::onResult(const QString &result)
{

    if(!isVisible())
    {
        emit sigResult(result);
        return;
    }

    translateUi->resultPlainTextEdit->clear();
    if(result.length() > 0)
    {
        QString plainText = result;
        plainText.replace(';', '\n');
        translateUi->resultPlainTextEdit->setPlainText(plainText);
    }
    else
    {
        translateUi->resultPlainTextEdit->setPlainText("未查询到！");
    }

    drawWholeUi();
}

void QuickTranslateWindow::slotTranslate(const QString &words)
{
    emit sigTranslate(words);
}

void QuickTranslateWindow::onSearchButtonClicked()
{
    QString word = translateUi->searchEdit->text();
    if(word.length() > 0)
    {
        emit sigTranslate(word);
    }
}

void QuickTranslateWindow::onAddButtonClicked()
{
    QString word = translateUi->searchEdit->text();
    QString explan = translateUi->resultPlainTextEdit->toPlainText();
    if((word.length()) > 0 && (explan.length()) > 0)
    {
        emit sigAdd(word, explan);
    }
}

void QuickTranslateWindow::onHotKey(int keyCode)
{
    if(QuickTranslateNativeEventFilter::HOTKEY_CTRL_SHIFT_F == keyCode)
    {
        drawPartUi();
    }
}

void QuickTranslateWindow::onTranslateUi()
{
    drawPartUi();
}

void QuickTranslateWindow::onHide()
{
    if(isMouseLeaved)
    {
        hide();
    }
}
