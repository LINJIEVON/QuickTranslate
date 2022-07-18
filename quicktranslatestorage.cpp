#include "quicktranslatestorage.h"
#include "quickfile.h"
#include <windows.h>
#include <QDir>
#include <QDebug>
#include <QVector>
#include <QtConcurrent/QtConcurrentRun>


#define WORDFILE        "word"
#define EXPLANATIONFILE "explanation"
#define CONTEXTFILE     "context"
#define LOGFILE         "log"

#define PATH         "C:/Users/LINJIE/Documents/QuickTranslate/"

QuickTranslateStorage::QuickTranslateStorage(QObject *parent):
    QObject(parent),
    dic_(),
    dicArray_()
{
#ifdef QT_NO_DEBUG
    QDir dir(PATH);
    if(!dir.exists())
    {
        Q_ASSERT(dir.mkdir(PATH));
    }

    SetFileAttributesA(PATH, FILE_ATTRIBUTE_HIDDEN);

    QDir::setCurrent(PATH);
    QString dicFileName(WORDFILE);
    QString explanFileNmae(EXPLANATIONFILE);
    QString cntFileName(CONTEXTFILE);
#else
    //文件在当前程序运行的目录下
    QString dicFileName(WORDFILE);
    QString explanFileNmae(EXPLANATIONFILE);
    QString cntFileName(CONTEXTFILE);
#endif

    wordFile_ = new QuickFile(dicFileName);
    explanationFile_ = new QuickFile(explanFileNmae);
    contextFile_ = new QuickFile(cntFileName);
}

QuickTranslateStorage::~QuickTranslateStorage()
{
    wordFile_->close();
    explanationFile_->close();
    contextFile_->close();

    delete wordFile_;
    delete explanationFile_;
    delete contextFile_;
    qDebug()<<"destroy QuickTranslateStorage";
}

QuickTranslateStorage *QuickTranslateStorage::getInstance()
{
    static QuickTranslateStorage storage;
    return &storage;
}

void QuickTranslateStorage::initLocalDictionary()
{
    checkSyncStatus();                                                  //（如果存在）修正之前word,explanation,context文件不同步问题

    QList<QByteArray> wordlist(wordFile_->readAllBytes().split(' '));
    QByteArray contextBytes(contextFile_->readAllBytes());

    //QByteArray explaBytes = explanationFile_->readAllBytes();

    if(wordlist.length() <= 1)
    {
        emit SigDictionaryReady(-1);
        return;
    }

    int step = sizeof(Context);
    int count = contextBytes.length() / step;
    qDebug()<<"step: "<<step<<","
           <<"count: "<<count;

    for(int i = 0; i < count; i++)
    {
        Context tcnx;
        setContext(contextBytes, i*step, tcnx);

        //dic_.emplace(wordlist[i], tcnx);
        auto pair(dic_.emplace(wordlist[i], tcnx));
        if(pair.second)
        {
            dicArray_.emplace_back(pair.first);
        }
    }
    emit SigDictionaryReady(0);
}

QString QuickTranslateStorage::find(const QString word)
{
    const auto iter = dic_.find(word.toLocal8Bit());
    if(iter == dic_.end())
    {
        return QString("");
    }

    Context& tcnx = dic_[word.toLocal8Bit()];
    ++tcnx.frequency_;

    QByteArray rBytes;
    explanationFile_->readByteArray(rBytes, tcnx.explanLen_, tcnx.explanationIndex_);

    QByteArray wBytes;
    setByteArray(tcnx, wBytes);
    contextFile_->writeByteArray(wBytes, wBytes.length(), tcnx.contextIndex_);

    return QString(rBytes);
}

QString QuickTranslateStorage::load(int pos, int len)
{
   QByteArray rBytes;
   explanationFile_->readByteArray(rBytes, len, pos);
   return rBytes;
}


void QuickTranslateStorage::addStorage(const QString &word, const QString &explanation)
{
    Context tcnx;

    QString newWord = word + ' ';                      //添加一个空格，方便解析
    QByteArray wordBytes = newWord.toLocal8Bit();
    std::string explanStr = explanation.toStdString();

    tcnx.wordLen_ = wordBytes.length();
    tcnx.wordIndex_ = wordFile_->size();
    tcnx.explanLen_ = explanStr.length();
    tcnx.explanationIndex_ = explanationFile_->size();
    tcnx.contextIndex_ = contextFile_->size();
    tcnx.frequency_ = 1;



    wordFile_->writeRawBytes(wordBytes.data(), tcnx.wordLen_, QuickFile::POSEND);
    explanationFile_->writeRawBytes(explanStr.c_str(), tcnx.explanLen_, QuickFile::POSEND);

    QByteArray wBytes = contextToByteArray(tcnx);
    contextFile_->writeByteArray(wBytes, wBytes.length(), QuickFile::POSEND);

    //dic_.emplace(word.toLocal8Bit(), tcnx);
    auto pair(dic_.emplace(word.toLocal8Bit(), tcnx));
    if(pair.second)
    {
        dicArray_.emplace_back(pair.first);
    }

    emit SigAddNewWord();
}

void QuickTranslateStorage::setContext(const QByteArray &bArray, int offset, QuickTranslateStorage::Context &context)
{
    Context& tcnx = context;
    const QByteArray& tb = bArray;

    memcpy(reinterpret_cast<char*>(&tcnx), tb.data() + offset, sizeof (Context));
}

void QuickTranslateStorage::setByteArray(const QuickTranslateStorage::Context &context, QByteArray &bArray, int offset)
{
    Q_UNUSED(offset);

    const Context& tcnx = context;
    QByteArray& tb = bArray;

    QByteArray tb2 = QByteArray(reinterpret_cast<const char*>(&tcnx), sizeof (Context));
    tb.push_back(tb2);
}

QuickTranslateStorage::Context QuickTranslateStorage::byteArrayToContext(const QByteArray &bArray, int offset)
{
    Context tcnx;
    const QByteArray& tb = bArray;

    memcpy(reinterpret_cast<char*>(&tcnx), tb.data() + offset, sizeof (Context));
    return tcnx;
}


QByteArray QuickTranslateStorage::contextToByteArray(const QuickTranslateStorage::Context &context)
{
    const Context& tcnx = context;
    return QByteArray(reinterpret_cast<const char*>(&tcnx), sizeof (Context));
}


void QuickTranslateStorage::checkSyncStatus()
{
    int cnxSize = contextFile_->size();
    int step = sizeof(Context);

    if(0 != (cnxSize % step))                                 //context文件长度不是Context结构的整数倍，那么context文件需要修复
    {
        cnxSize -= cnxSize % step;
        contextFile_->resize(cnxSize);
        qDebug()<<"repair context file";
    }

    QByteArray rBytes;
    contextFile_->readByteArray(rBytes, step, cnxSize - step);

    Context tcnx = byteArrayToContext(rBytes, 0);
    int realSize = tcnx.wordIndex_ + tcnx.wordLen_;
    if(wordFile_->size() != realSize)                          //如果word文件长度和context文件记录不一致，则重置为context记录的长度
    {
        wordFile_->resize(realSize);
        qDebug()<<"repair word file";
    }

    realSize = tcnx.explanationIndex_ + tcnx.explanLen_;
    if(explanationFile_->size() != realSize)                  //如果explanation文件长度和context文件记录不一致，则重置为context记录的长度
    {
        explanationFile_->resize(realSize);
        qDebug()<<"repair explanation file";
    }
}



TestClass::TestClass(QObject *parent):
    mStorage(QuickTranslateStorage::getInstance())
{
    mParent = parent;
    setParent(parent);

    connect(mStorage, SIGNAL(SigDictionaryReady(int)), this, SLOT(onCreatedDictionary(int)));

    QtConcurrent::run(mStorage, &QuickTranslateStorage::initLocalDictionary);
    //mQuickTranslateStorage.initLocalDictionary();
}

TestClass::~TestClass()
{
    qDebug()<<"TestClass destroy";
}

void TestClass::onCreatedDictionary(int ret)
{
    qDebug()<<"Dictionary init ret = "<< ret;

    if(ret < 0)
    {
        mStorage->addStorage("record", "n.记录;记载;唱片;(尤指体育运动中最高或最低的)纪录;（有关过去的）事实;前科v.记录;记载;录制;录(音);演奏音乐供录制;灌(唱片);发表正式（或法律方面的）声明;标明");
        mStorage->addStorage("company", "n.公司;商号;商行;剧团;演出团;陪伴;做伴;宾客;在一起的一群人;连队");
        mStorage->addStorage("theatre", "n.剧院;剧场;戏剧;戏院;露天剧场;戏剧工作;剧作;演出;上演;战场");
    }
/*
    QString tranStr = mQuickTranslateStorage.find("record");
    qDebug()<<tranStr;

    tranStr = mQuickTranslateStorage.find("record");
    qDebug()<<tranStr;

    tranStr = mQuickTranslateStorage.find("company");
    qDebug()<<tranStr;

    tranStr = mQuickTranslateStorage.find("theatre");
    qDebug()<<tranStr;
    */

}

void TestClass::onSearch(const QString &word)
{
    emit sigResult(mStorage->find(word));
}

void TestClass::onAdd(const QString &word, const QString& explanation)
{
    mStorage->addStorage(word, explanation);
    emit sigResult("添加成功");
}
