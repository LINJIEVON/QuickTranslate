#include "quicktranslate.h"
#include "quicktranslatestorage.h"
#include "quicktranslatenet.h"

#include <QDebug>
#include <QList>
#include <QtConcurrent/QtConcurrentRun>


QuickTranslate::QuickTranslate(QObject *parent):
    QObject(parent),
    mStorage_(QuickTranslateStorage::getInstance()),
    mNetTrans_(new QuickTranslateNet(parent)),
    srcStr_()
{
    //网络翻译完成通知
    connect(mNetTrans_, &QuickTranslateNet::sigTransResult, this, &QuickTranslate::slotNetTranslated);

    //本地字典生成完成通知
    connect(mStorage_, &QuickTranslateStorage::SigDictionaryReady, this, &QuickTranslate::onDictionaryReady);
    QtConcurrent::run(mStorage_, &QuickTranslateStorage::initLocalDictionary);            //初始化本地字典
}

QuickTranslate::~QuickTranslate()
{
    qDebug()<<"destroy QuickTranslate";
}

void QuickTranslate::translate(const QString &words)
{
    srcStr_ = words.trimmed();                   //去除前后空格
    if(srcStr_.length() <= 0)
    {
        return;
    }

    QString translation = localTranslate(srcStr_);
    if(translation.length() <= 0)
    {
        netTranslate(srcStr_);
    }

    if(translation.length() > 0)
    {
        emit sigResult(translation);
    }
}

QString QuickTranslate::localTranslate(const QString &word)
{
    return mStorage_->find(word);
}

void QuickTranslate::netTranslate(const QString &words)
{
    mNetTrans_->sendTranslateRequest(words);
}

void QuickTranslate::saveRecord(const QString &word, const QString &explanation)
{
    mStorage_->addStorage(word, explanation);
}

void QuickTranslate::slotTranslate(const QString &words)
{
    translate(words);
}

void QuickTranslate::slotNetTranslated(const QString &result)
{
    emit sigResult(result);

    if((!srcStr_.contains(' ')) && (result.length() > 0))                   //只记录单词
    {
       mStorage_->addStorage(srcStr_, result);
    }
}

void QuickTranslate::onAddWord(const QString &word, const QString &explanation)
{
    saveRecord(word, explanation);

    emit sigResult("添加成功");
}

void QuickTranslate::onDictionaryReady()
{
    qDebug()<<"local dictionary ready";
}


