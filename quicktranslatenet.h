#ifndef QUICKTRANSLATENET_H
#define QUICKTRANSLATENET_H

#include <QObject>
#include <QNetworkReply>


//class QNetworkAccessManager;

class QuickTranslateNet : public QObject
{
    Q_OBJECT
public:
    typedef enum{
        BAIDU = 0,
        ICIBA
    }TTYPE;
public:
    explicit QuickTranslateNet(QObject *parent = nullptr);
    ~QuickTranslateNet();

    QuickTranslateNet(const QuickTranslateNet&) = delete;
    QuickTranslateNet& operator=(const QuickTranslateNet&) = delete;

    void sendTranslateRequest(const QString& words);
    QString getTranslateResult();


private:
    QUrl& Baidu_TranslatePrepareQuery(QUrl&, const QString&);
    QUrl& iCIBA_DictionaryPrepareQuery(QUrl&, const QString&);
    QString& Baidu_DictionaryResponseParse(const QByteArray&, QString&);
    QString& iCIBA_DictionaryResponseParse(const QByteArray&, QString&);


signals:
    void sigTransResult(const QString&);

public slots:
    //void slotReadyRead();
    void slotError(QNetworkReply::NetworkError);
    void slotFinished(QNetworkReply*);

private:
    QNetworkAccessManager*        netManager_;
    bool                          isSentence_;
    QuickTranslateNet::TTYPE      type_;
    //QNetworkReply*                reply_;
    //bool                          isReplied_;
};

void netTranslateTest();

#endif // QUICKTRANSLATENET_H
