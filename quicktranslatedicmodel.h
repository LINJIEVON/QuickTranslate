#ifndef QUICKTRANSLATEDICMODEL_H
#define QUICKTRANSLATEDICMODEL_H

#include "quicktranslatestorage.h"
#include <QAbstractTableModel>

using DataType = std::map<QByteArray, QuickTranslateStorage::Context>::const_iterator;

class QuickTranslateDicModel : public QAbstractTableModel
{
    Q_OBJECT   
public:
    QuickTranslateDicModel(QObject* parent = nullptr);
    ~QuickTranslateDicModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

private:
    void sortData(int, int);

public slots:
    void slotUpdateModelData(DataType);

private:
    QuickTranslateStorage*            mStorage_;
    void*                             data_;
    int                               orderColumn_;
    int                               order_;
};

#endif // QUICKTRANSLATEDICMODEL_H
