#ifndef QUICKTRANSLATEQuickTranslateStorage_H
#define QUICKTRANSLATEQuickTranslateStorage_H

#include <QObject>
#include <map>

class QuickFile;
class QuickTranslateDicModel;

class QuickTranslateStorage:public QObject
{
    Q_OBJECT
public:
    struct Context{
        int        contextIndex_;
        int        wordIndex_;
        int        wordLen_;
        int        explanationIndex_;
        int        explanLen_;
        int        frequency_;
    };

public:
    static QuickTranslateStorage* getInstance();
    void initLocalDictionary();
    QString find(const QString word);
    QString load(int pos, int len);
    void addStorage(const QString& word, const QString& explanation);

    QuickTranslateStorage(const QuickTranslateStorage&) = delete;
    QuickTranslateStorage& operator=(const QuickTranslateStorage&) = delete;
    ~QuickTranslateStorage();

private:
    void setContext(const QByteArray& bArray, int offset, Context& context);
    void setByteArray(const Context& context, QByteArray& bArray, int offset = 0);
    Context byteArrayToContext(const QByteArray& bArray, int offset);
    QByteArray contextToByteArray(const Context& context);
    void checkSyncStatus();
    bool isSingleWord(const QString&);

    QuickTranslateStorage(QObject* parent = nullptr);
    void setParent(QObject*){};

signals:
    void SigDictionaryReady(int);
    void SigAddNewWord(std::map<QByteArray, Context>::const_iterator);

private:
    std::map<QByteArray, Context>        dic_;
    QuickFile*        wordFile_;
    QuickFile*        explanationFile_;
    QuickFile*        contextFile_;
    //QuickFile*        logFile_;

private:
    friend QuickTranslateDicModel;
};

class TestClass:public QObject
{
    Q_OBJECT
public:
    TestClass(QObject* parent = nullptr);
    ~TestClass();

signals:
    void sigResult(const QString&);

public slots:
    void onCreatedDictionary(int);
    void onSearch(const QString&);
    void onAdd(const QString&, const QString&);

private:
    QObject*      mParent;
    QuickTranslateStorage*       mStorage;
};

void testQuickTranslateStorage();
#endif // QUICKTRANSLATEQuickTranslateStorage_H
