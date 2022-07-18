#ifndef QuickFile_H
#define QuickFile_H

#include <QFile>

class QDataStream;

class QuickFile: public QFile
{

public:
    enum{
        CONTINUE = -1,
        POSEND = -2,
    };

public:
    QuickFile(const QString& name);
    ~QuickFile();

    QuickFile(const QuickFile&) = delete;
    QuickFile& operator=(const QuickFile&) = delete;

    /**
     * @brief readAllBytes
     * @return                   整个文件数据
     */
    QByteArray readAllBytes();

    /**
     * @brief readRawBytes
     * @param buf                 装载读取到的数据的buffer
     * @param readLen             要读取数据的长度
     * @param pos                 读取位置
     * @return                    成功：实际读取到的数据长度 失败：< 0
     */
    int readRawBytes(char* buf, int readLen, int pos);

    /**
     * @brief writeRawBytes
     * @param buf                 装载待写数据的buffer
     * @param writeLen            要写数据的长度
     * @param pos                 写入位置
     * @return                    成功：实际写入的数据长度 失败：< 0
     */
    int writeRawBytes(const char* buf, int writeLen, int pos);

    /**
     * @brief readByteArray
     * @param bArray              将数据读取到bArray
     * @param readLen             要读数据的长度
     * @param pos                 读取位置
     * @return                    成功：实际读取到的数据长度 失败：< 0
     */
    int readByteArray(QByteArray& bArray, int readLen, int pos);

    /**
     * @brief writeByteArray
     * @param bArray              待写入的数据
     * @param writeLen            要写数据的长度
     * @param pos                 写入位置
     * @return                    成功：实际写入的数据长度 失败：< 0
     */
    int writeByteArray(const QByteArray& bArray, int writeLen, int pos);

private:
    QDataStream*        m_DataStreamPointer;
};

void testQuickFile();

#endif // QuickFile_H














