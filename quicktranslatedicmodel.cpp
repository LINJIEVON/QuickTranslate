#include "quicktranslatedicmodel.h"
#include <QDebug>
#include <QSize>
#include <QColor>



QuickTranslateDicModel::QuickTranslateDicModel(QObject *parent):
    QAbstractTableModel(parent),
    mStorage_(QuickTranslateStorage::getInstance()),
    data_(nullptr),
    orderColumn_(0),
    order_(Qt::AscendingOrder)
{
    connect(mStorage_, &QuickTranslateStorage::SigAddNewWord, this, &QuickTranslateDicModel::slotUpdateModelData);
    data_  = reinterpret_cast<void*>(new std::vector<DataType>);
    std::vector<DataType>& updateData = *reinterpret_cast< std::vector<DataType>* >(data_);

    const auto& dicData =  mStorage_->dic_;
    updateData.reserve(dicData.size());

    for(auto iter = dicData.begin(); iter != dicData.end(); iter++)
    {
        updateData.push_back(iter);
    }

    sortData(0, Qt::AscendingOrder);
}

QuickTranslateDicModel::~QuickTranslateDicModel()
{
    delete reinterpret_cast< QVector<DataType>* >(data_);
    qDebug()<<"destroy QuickTranslateDicModel";
}

int QuickTranslateDicModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return reinterpret_cast< std::vector<DataType>* >(data_)->size();
}

int QuickTranslateDicModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant QuickTranslateDicModel::data(const QModelIndex &index, int role) const
{

    const auto& modelData = *reinterpret_cast< std::vector<DataType>* >(data_);
    if (!index.isValid() || ( index.row() >= (int)modelData.size()))
    {
        return QVariant();
    }

    QVariant rVariant;
    const auto& iter  = modelData[index.row()];

    if(Qt::DisplayRole == role)
    {
        switch(index.column())
        {
        case 0:
            rVariant.setValue(iter->first);
            break;
        case 1:
            rVariant.setValue(mStorage_->load(iter->second.explanationIndex_
                                              , iter->second.explanLen_));
            break;
        case 2:
            rVariant.setValue(iter->second.frequency_);
            break;
        default:
            qDebug()<<index.column();
        }
    }
    else if(Qt::ToolTipRole ==  role)
    {
        QString plainText = mStorage_->load(iter->second.explanationIndex_
                                            , iter->second.explanLen_);
        plainText.replace(';', '\n');
        rVariant.setValue(plainText);
    }
    else if(Qt::StatusTipRole == role)
    {
        rVariant.setValue(mStorage_->load(iter->second.explanationIndex_
                                          , iter->second.explanLen_));
    }

    return rVariant;
}

QVariant QuickTranslateDicModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant rVariant;

    if(Qt::DisplayRole == role)
    {
        if(Qt::Horizontal == orientation)
        {
            switch(section)
            {
            case 0:
                rVariant.setValue(QString("单词"));
                break;
            case 1:
                rVariant.setValue(QString("解释"));
                break;
            case 2:
                rVariant.setValue(QString("频率"));
                break;
            default:
                qDebug()<<section;
            }
        }
        else
        {
            rVariant.setValue(section);
        }
    }
    else if(Qt::SizeHintRole == role)
    {
        if(Qt::Horizontal == orientation)
        {
            rVariant.setValue(QSize(100,25));
        }
        else
        {
            rVariant.setValue(QSize(30,25));
        }
    }
    else if(Qt::Vertical == orientation)
    {
        if(Qt::StatusTipRole ==  role)
        {
            const auto& modelData = *reinterpret_cast< std::vector<DataType>* >(data_);
            if (section >= (int)modelData.size())
            {
                return QVariant();
            }
            auto iter  = modelData[section];
            rVariant.setValue(mStorage_->load(iter->second.explanationIndex_
                                              , iter->second.explanLen_));
        }
    }

    return rVariant;
}

void QuickTranslateDicModel::sort(int column, Qt::SortOrder order)
{
    beginResetModel();
    sortData(column, order);
    endResetModel();
    //qDebug()<<"sort column:"<<column;
}


void QuickTranslateDicModel::sortData(int column, int order)
{
    if((column >= 0) && (order >= 0))
    {
        orderColumn_ = column;
        order_ = order;
    }

    if(0 == orderColumn_)
    {
        if(order_ == Qt::AscendingOrder)
        {
            auto sortFunc = [](const DataType& l, const DataType& r){      //单词升序
                return l->first < r->first;
            };
            std::vector<DataType>& updateData = *reinterpret_cast< std::vector<DataType>* >(data_);
            std::sort(updateData.begin(), updateData.end(), sortFunc);
        }
        else if(order_ == Qt::DescendingOrder)
        {
            auto sortFunc = [](const DataType& l, const DataType& r){      //单词降序
                        return l->first > r->first;
                    };
            std::vector<DataType>& updateData = *reinterpret_cast< std::vector<DataType>* >(data_);
            std::sort(updateData.begin(), updateData.end(), sortFunc);
        }
    }
    else if(2 == orderColumn_)
    {
        if(order_ == Qt::AscendingOrder)
        {
            auto sortFunc = [](const DataType& l, const DataType& r){      //频率升序
                return l->second.frequency_ < r->second.frequency_;
            };
            std::vector<DataType>& updateData = *reinterpret_cast< std::vector<DataType>* >(data_);
            std::sort(updateData.begin(), updateData.end(), sortFunc);
        }
        else if(order_ == Qt::DescendingOrder)
        {
            auto sortFunc = [](const DataType& l, const DataType& r){      //频率降序
                        return l->second.frequency_ > r->second.frequency_;
                    };
            std::vector<DataType>& updateData = *reinterpret_cast< std::vector<DataType>* >(data_);
            std::sort(updateData.begin(), updateData.end(), sortFunc);
        }
    }
}

void QuickTranslateDicModel::slotUpdateModelData(DataType iter)
{
    beginResetModel();
    std::vector<DataType>& updateData = *reinterpret_cast< std::vector<DataType>* >(data_);
    if(iter != mStorage_->dic_.end())
    {
        updateData.push_back(iter);
    }
    sortData(-1, -1);
    endResetModel();
}
