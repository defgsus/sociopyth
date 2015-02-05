/** @file kekmodel.cpp

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 05.02.2015</p>
*/

#include "kekmodel.h"
#include "kekdata.h"

KekModel::KekModel(KekData * kek, QObject *parent) :
    QAbstractItemModel  (parent),
    kek_                (kek)
{
    headerNames_
            << "actor"
            << "titles"
            << "shares"
            << "total titles"
            << "total shares"
            << "%"
    ;
}

QModelIndex KekModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || row >= (int)kek_->companies().size()
        || parent.isValid())
        return QModelIndex();

    return createIndex(row, column);
}

QModelIndex KekModel::parent(const QModelIndex &/*child*/) const
{
    return QModelIndex();
}

int KekModel::rowCount(const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : kek_->companies().size();
}


int KekModel::columnCount(const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : headerNames_.size();
}


QVariant KekModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.column() < 0
            || index.column() >= headerNames_.size()
        || index.row() < 0 || index.row() >= (int)kek_->companies().size())
        return QVariant();


    int row = index.row();

    // return text
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
            case 0: return kek_->companies()[row]->name;
            case 1: return int(kek_->companies()[row]->titles.size());
            case 2: return int(kek_->companies()[row]->shares.size());
            case 3: return kek_->companies()[row]->total_titles;
            case 4: return kek_->companies()[row]->total_shares;
            case 5: return kek_->companies()[row]->total_shares_percent;
            default: return "-";
        }
    }

    // text alignment
    if (role == Qt::TextAlignmentRole)
        return (index.column() == 0)?
                    (int)(Qt::AlignLeft | Qt::AlignVCenter)
                :   (int)(Qt::AlignRight | Qt::AlignVCenter);

    /*
    if (role == Qt::BackgroundRole)
    {
        if (index.column() == 2)
            return QBrush(Qt::gray);
        if (index.column() == 1)
            return QBrush(Qt::green);
    }*/

    return QVariant();
}

Qt::ItemFlags KekModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flag = QAbstractItemModel::flags(index);

    if (index.isValid())
    {
        flag |= Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    return flag;
}

QVariant KekModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal && section < headerNames_.size())
        {
            return headerNames_[section];
        }
    }

    return QVariant();
}
