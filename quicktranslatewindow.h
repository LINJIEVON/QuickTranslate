#ifndef QUICKTRANSLATEWINDOW_H
#define QUICKTRANSLATEWINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class TranslateWindow; }
QT_END_NAMESPACE

//class QuickTranslate;

class QuickTranslateWindow : public QWidget
{
    Q_OBJECT
public:
    explicit QuickTranslateWindow(QWidget *parent = nullptr);
    ~QuickTranslateWindow();
    void setIsOnShow(bool set);

protected:
    void leaveEvent(QEvent *event) override;
    void enterEvent(QEvent *event) override;
    //bool event(QEvent *event) override;

private:
    void drawPartUi();
    void drawWholeUi();
    void initChangedSize();

signals:
    void sigTranslate(const QString&);
    void sigResult(const QString&);
    void sigAdd(const QString&, const QString&);

public slots:
    void onResult(const QString&);
    void slotTranslate(const QString&);
    void onSearchButtonClicked();
    void onAddButtonClicked();
    void onHotKey(int);
    void onTranslateUi();
    void onHide();

private:
    Ui::TranslateWindow*      translateUi;
    QSize                     wholeSize;
    QSize                     partSize;
    bool                      isMouseLeaved;
    bool                      isOnShow;
};

#endif // QUICKTRANSLATEWINDOW_H
