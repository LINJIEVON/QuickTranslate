#include "quicktranslatedicmodel.h"
#include "quicktranslatestorage.h"
#include <QDebug>
#include <QSize>
#include <QColor>


QuickTranslateDicModel::QuickTranslateDicModel(QObject *parent):
    QAbstractTableModel(parent),
    mStorage_(QuickTranslateStorage::getInstance())
{
    connect(mStorage_, &QuickTranslateStorage::SigAddNewWord,
            this, &QuickTranslateDicModel::slotQuickTranslateModelChange);
}

QuickTranslateDicModel::~QuickTranslateDicModel()
{
    qDebug()<<"destroy QuickTranslateDicModel";
}

int QuickTranslateDicModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mStorage_->getLocalDictionnaryVector().size();
}

int QuickTranslateDicModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant QuickTranslateDicModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid())
    {
        return QVariant();
    }

    QVariant rVariant;

    auto iter  = mStorage_->getLocalDictionnaryVector()[index.row()];
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
            auto iter  = mStorage_->getLocalDictionnaryVector()[section];
            rVariant.setValue(mStorage_->load(iter->second.explanationIndex_
                                              , iter->second.explanLen_));
        }
    }

    return rVariant;
}

void QuickTranslateDicModel::slotQuickTranslateModelChange()
{
    beginResetModel();
    endResetModel();
}
