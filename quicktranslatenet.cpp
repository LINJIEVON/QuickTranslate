#include "quicktranslatenet.h"
#include <QNetworkAccessManager>
#include <QNetworkConfiguration>
#include <QRandomGenerator>
#include <QCryptographicHash>
#include <QUrlQuery>
#include <QThread>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

//各翻译平台请求破解
//https://blog.csdn.net/hujingshuang/article/details/80190980

//百度翻译      申请的API(稳定)
//目前api等级， 用于中英（句子）互译，单词查询解释单一不建议
#define BAIDU_ID    "20220630001260803"
#define BAIDU_KEY   "cGx52BHNP9TrwMkYPJz2"
#define BAIDU_URL   "https://fanyi-api.baidu.com/api/trans/vip/translate"
#define BAIDU_TEST_URL   "https://localhost:1884"


//金山词典      未申请API(不稳定)
//目前测试      可以中英互译，主要用来查单词
#define ICBA_URL "https://dict.iciba.com/dictionary/word/suggestion"



QuickTranslateNet::QuickTranslateNet(QObject *parent) :
    QObject(parent),
    netManager_(new QNetworkAccessManager(this))
    //reply_(nullptr),
    //isReplied_(false)
{
    QNetworkConfiguration config = netManager_->configuration();
    config.setConnectTimeout(3000);                                 //ms
    netManager_->setConfiguration(config);

    connect(netManager_, &QNetworkAccessManager::finished, this, &QuickTranslateNet::slotFinished);
}

QuickTranslateNet::~QuickTranslateNet()
{
    qDebug()<<"destroy QuickTranslateNet";
}


void QuickTranslateNet::sendTranslateRequest(const QString &words)
{
    QString tWords = words.trimmed();

    if(tWords.length() <= 0)
    {
        return;
    }

    QUrl url;
    if(!tWords.contains(' '))
    {
        type_ = ICIBA;
        iCIBA_DictionaryPrepareQuery(url, tWords);
    }
    else
    {
        type_ = BAIDU;
        Baidu_TranslatePrepareQuery(url, tWords);
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
    netManager_->get(request);
    //reply_ = netManager_->get(request);

    //connect(reply_, &QIODevice::readyRead, this, &QuickTranslateNet::slotReadyRead);
    //connect(reply_, &QNetworkReply::error, this, &QuickTranslateNet::slotError);
    //connect(reply_, SIGNAL(error(QNetworkReply::NetworkError)),
    //        this, SLOT(slotError(QNetworkReply::NetworkError)));
}

QString QuickTranslateNet::getTranslateResult()
{
    return "";
}

QUrl& QuickTranslateNet::Baidu_TranslatePrepareQuery(QUrl& url, const QString& words)
{
    QUrl& tUrl = url;
    const QString& tWords = words;

    tUrl.setUrl(BAIDU_URL);
    QUrlQuery urlQuery;

    //1.翻译query
    QByteArray transBytes = QByteArray(tWords.toLower().toUtf8());
    urlQuery.addQueryItem("q", transBytes);

    //2.源语言
    QByteArray fromBytes = QByteArray("en");
    urlQuery.addQueryItem("from", fromBytes);

    //3.目标语言
    QByteArray toBytes = QByteArray("zh");
    urlQuery.addQueryItem("to", toBytes);

    //4.id
    QByteArray appidBytes = QByteArray(BAIDU_ID);
    urlQuery.addQueryItem("appid", appidBytes);

    //5.key
    QByteArray keyBytes = QByteArray(BAIDU_KEY);

    //7.随机数
    QByteArray saltBytes = QByteArray::number(QRandomGenerator::global()->generate64());
    urlQuery.addQueryItem("salt", saltBytes);

    //md5签名源
    QByteArray srcBytes = appidBytes + transBytes + saltBytes + keyBytes;

    //签名
    QByteArray sigBytes = QByteArray(QCryptographicHash::hash(srcBytes, QCryptographicHash::Md5).toHex());
    urlQuery.addQueryItem("sign", sigBytes);

    tUrl.setQuery(urlQuery);
    qDebug()<<tUrl.toString();
    return tUrl;
}

QUrl& QuickTranslateNet::iCIBA_DictionaryPrepareQuery(QUrl& url, const QString& word)
{

    QUrl& tUrl = url;
    const QString& tWords = word;

    tUrl.setUrl(ICBA_URL);
    QUrlQuery urlQuery;

    //1.翻译单词
    QByteArray transBytes = QByteArray(tWords.toUtf8());
    urlQuery.addQueryItem("word", transBytes);

    //2.查询相关数量
    QByteArray numsBytes = QByteArray::number(1);         //默认是5，数字越大查询出来的相关内容越多, 1代表基础内容
    urlQuery.addQueryItem("nums", numsBytes);

    //暂时没做其他参数，测试发现可以忽略

    tUrl.setQuery(urlQuery);
    qDebug()<<tUrl.toString();
    return tUrl;
}

QString& QuickTranslateNet::Baidu_DictionaryResponseParse(const QByteArray &inBytes, QString &outStr)
{
    const QByteArray& readBytes = inBytes;
    QString& resultStr = outStr;

    resultStr.clear();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(readBytes, &parseError);
    qDebug()<<"baidu json parse: "<<parseError.errorString();

    if(QJsonParseError::NoError == parseError.error)
    {
        QJsonObject jsonObj = jsonDoc.object();
        if((jsonObj.contains("trans_result")) && (jsonObj["trans_result"].isArray()))
        {
            bool find = false;
            for(const auto& value : jsonObj["trans_result"].toArray())
            {
                const QJsonObject& obj= value.toObject();
                if(obj.contains("dst"))
                {
                    find = true;
                    resultStr.push_back(obj["dst"].toString());
                    break;
                }
            }

            if(!find)
            {
                qDebug()<<"json parse not contain 'dst'";
            }
        }
        else
        {
            qDebug()<<"json parse not contain 'trans_result'";
        }
    }
    else
    {
        qDebug()<<"parse error";
    }

    return resultStr;
}

QString& QuickTranslateNet::iCIBA_DictionaryResponseParse(const QByteArray &inBytes, QString &outStr)
{
    const QByteArray& readBytes = inBytes;
    QString& resultStr = outStr;
    resultStr.clear();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(readBytes, &parseError);
    qDebug()<<"iciba json parse: "<<parseError.errorString();

    if(QJsonParseError::NoError == parseError.error)
    {
        QJsonObject jsonObj = jsonDoc.object();
        if((jsonObj.contains("message")) && (jsonObj["message"].isArray()))
        {
            bool find = false;
            for(const auto& value : jsonObj["message"].toArray())
            {
                const QJsonObject& obj= value.toObject();
                if(obj.contains("paraphrase"))
                {
                    find = true;
                    resultStr.push_back(obj["paraphrase"].toString());
                    break;
                }
            }

            if(!find)
            {
                qDebug()<<"json parse not contain 'paraphrase'";
            }
        }
        else
        {
            qDebug()<<"json parse not contain 'message'";
        }
    }
    else
    {
        qDebug()<<"parse error";
    }

    return resultStr;
}

void QuickTranslateNet::slotError(QNetworkReply::NetworkError errorNum)
{
    Q_UNUSED(errorNum)
    //isReplied_ = true;
    //qDebug()<<"net error: "<<errorNum;
}

/*
void QuickTranslateNet::slotReadyRead()
{
    //isReplied_ = true;
    //qDebug()<<"could read";
}
*/

void QuickTranslateNet::slotFinished(QNetworkReply *reply)
{
    QNetworkReply::NetworkError error = reply->error();
    qDebug()<<"finised: "<<error;

    QString rStr("");

    if(error == QNetworkReply::NoError)
    {
        if(ICIBA == type_)
        {
            iCIBA_DictionaryResponseParse(reply->readAll(), rStr);
        }
        else if(BAIDU == type_)
        {
            Baidu_DictionaryResponseParse(reply->readAll(), rStr);
        }
    }

    emit sigTransResult(rStr);

    reply->deleteLater();
}



/**
 * @brief netTranslateTest
 * 测试
 */
void netTranslateTest()
{
    QuickTranslateNet* netTrans = new QuickTranslateNet();
    netTrans->sendTranslateRequest("Test");
    //qDebug()<<"network translate result: "<<netTrans->getTranslateResult();
}
