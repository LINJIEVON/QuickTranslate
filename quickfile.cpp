#include "quickfile.h"

#include <QDataStream>
#include <QDebug>
#include <QDir>


QuickFile::QuickFile(const QString& name):
    QFile(name),
    m_DataStreamPointer(new QDataStream(this))
{
    Q_ASSERT(name.length() > 0);

    //QString fileDir = QDir::currentPath() + '/' +fileName;

    //setFileName(fileDir);

    if(!open(QIODevice::ReadWrite))
    {
        QString errorStr;
        setErrorString(errorStr);
        qDebug()<<"File(" + name + ") open filed! "<< errorStr;
    }

    //m_DataStreamPointer = new QDataStream(this);
}

QuickFile::~QuickFile()
{
   if(nullptr != m_DataStreamPointer)
   {
       delete m_DataStreamPointer;
   }
   close();
}


QByteArray QuickFile::readAllBytes()
{
    QByteArray allByte(0);
    if(!isOpen())                //如果文件未打开，不执行操作
    {
        qDebug()<<"readRawBytes: file not open!";
    }
    else
    {
        seek(0);
        allByte = readAll();
    }

    return allByte;
}

int QuickFile::readRawBytes(char *buf, int readLen, int pos)
{
    if(!isOpen())                //如果文件未打开，不执行操作
    {
        qDebug()<<"readRawBytes: file not open!";
        return -1;
    }

    int readRet = 0;
    if(CONTINUE != pos)          //如果读取位置不是CONTINUE的话，就重新设置读取位置为pos,否则就从上一次读取完后的位置开始
    {
        seek(pos);
    }

    readRet = m_DataStreamPointer->readRawData(buf, readLen);
    seek(pos + readRet);

    return readRet;
}

int QuickFile::writeRawBytes(const char *buf, int writefLen, int pos)
{
    if(!isOpen())                //如果文件未打开，不执行操作
    {
        qDebug()<<"writeRawBytes: file not open!";
        return -1;
    }

    int writeRet = 0;

    if(CONTINUE != pos)
    {
        if(POSEND != pos)
        {
            seek(pos);
        }
        else
        {
            seek(size());
        }
    }

    writeRet = m_DataStreamPointer->writeRawData(buf, writefLen);
    //writeRet = write(buf, writefLen);

    return writeRet;
}

int QuickFile::readByteArray(QByteArray &bArray, int readLen, int pos)
{
    if(!isOpen())                //如果文件未打开，不执行操作
    {
        qDebug()<<"readRawBytes: file not open!";
        return -1;
    }

    int readRet = 0;
    if(CONTINUE != pos)          //如果读取位置不是CONTINUE的话，就重新设置读取位置为pos,否则就从上一次读取完后的位置开始
    {
        seek(pos);
    }

    QByteArray& rArray = bArray;
    rArray = read(readLen);

    seek(pos + readRet);

    return rArray.size();
}

int QuickFile::writeByteArray(const QByteArray &bArray, int writeLen, int pos)
{
    Q_UNUSED(writeLen)

    if(!isOpen())                //如果文件未打开，不执行操作
    {
        qDebug()<<"writeRawBytes: file not open!";
        return -1;
    }

    int writeRet = 0;

    if(CONTINUE != pos)
    {
        if(POSEND != pos)
        {
            seek(pos);
        }
        else
        {
            seek(size());
        }
    }

    writeRet = write(bArray);
    //writeRet = write(buf, writefLen);

    return writeRet;
}


void testQuickFile()
{
    QuickFile* pFile = new QuickFile("test_file");

    pFile->writeRawBytes(const_cast<char*>("aaaaaaaaaaaaaaaaaaaa"), 20, 0);
    pFile->writeRawBytes(const_cast<char*>("bb"), 2, 3);
    pFile->writeRawBytes(const_cast<char*>("cc"), 2, QuickFile::CONTINUE);
    pFile->writeRawBytes(const_cast<char*>("ddd"), 2, QuickFile::POSEND);

    delete pFile;

    pFile = new QuickFile("test_file");
    char* rBuf = nullptr;

    QByteArray bytes = pFile->readAllBytes();
    qDebug()<<"file content"<< bytes.size() <<" : "<< bytes.data() ;


    rBuf = new char[6];
    memset(rBuf, 0, 6);

    pFile->readRawBytes(rBuf, 5, 0);
    qInfo("file pos %d read %d bytes: %s", 0, 5, rBuf);

    pFile->readRawBytes(rBuf, 5, 5);
    qInfo("file pos %d read %d bytes: %s", 5, 5, rBuf);

    pFile->readRawBytes(rBuf, 5, QuickFile::CONTINUE);
    qInfo("file pos %d read %d bytes: %s", 5, 5, rBuf);

    pFile->readRawBytes(rBuf, 5, 0);
    qInfo("file pos %d read %d bytes: %s", 0, 5, rBuf);

    delete[] rBuf;
    delete pFile;
}




