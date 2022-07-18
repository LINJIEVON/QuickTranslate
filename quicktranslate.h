#ifndef QUICKTRANSLATE_H
#define QUICKTRANSLATE_H

#include <QObject>

class QuickTranslateStorage;
class QuickTranslateNet;

class QuickTranslate:public QObject
{  
    Q_OBJECT
public:
    QuickTranslate(QObject* parent = nullptr);
    ~QuickTranslate();

    void translate(const QString& word);

private:
    QString localTranslate(const QString& word);
    void netTranslate(const QString& word);
    void saveRecord(const QString& word, const QString& explanation);

signals:
    void sigResult(const QString&);

public slots:
    void slotTranslate(const QString&);
    void slotNetTranslated(const QString&);
    void onAddWord(const QString&, const QString&);
    void onDictionaryReady();

private:
    QuickTranslateStorage*        mStorage_;
    QuickTranslateNet*            mNetTrans_;
    QString                       srcStr_;
};


#endif // QUICKTRANSLATE_H
