/** @file biolistmodel.cpp

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 15.02.2015</p>
*/

#include "biolistmodel.h"
#include "biodata.h"

BioListModel::BioListModel(BioData* data, QObject *parent) :
    QAbstractItemModel  (parent),
    data_               (data)
{
    headerNames_
            << tr("Person")
            << tr("Partei")
            << tr("Beruf")
            << tr("persönl.")
            << tr("Wahlkreis")
            << tr("Gremium")
            << tr("Wahlperiode")
            << tr("orden. Aufgaben")
            << tr("stellv. Aufgaben")
            << tr("andere Funktionen")
            << tr("vorher. Funktionen")
    ;
}

QModelIndex BioListModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || row >= (int)data_->numPersons()
        || parent.isValid())
        return QModelIndex();

    return createIndex(row, column);
}

QModelIndex BioListModel::parent(const QModelIndex &/*child*/) const
{
    return QModelIndex();
}

int BioListModel::rowCount(const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : data_->numPersons();
}


int BioListModel::columnCount(const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : headerNames_.size();
}


QVariant BioListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.column() < 0
            || index.column() >= headerNames_.size()
        || index.row() < 0 || index.row() >= (int)data_->numPersons())
        return QVariant();


    int row = index.row();

    // return text
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        const BioData::Person & p = data_->person(row);
        switch (index.column())
        {
            case 0: return p.name;
            case 1: return p.party;
            case 2: return p.occupation;
            case 3: return p.birth;
            case 4: return p.wahlkreis;
            case 5: return p.gremium;
            case 6: return p.period;
            case 7: return p.memberships.size() >= 1 ? p.memberships[0].size() : 0;
            case 8: return p.memberships.size() >= 2 ? p.memberships[1].size() : 0;
            case 9: return p.memberships.size() >= 3 ? p.memberships[2].size() : 0;
            case 10: return p.memberships.size() >= 4 ? p.memberships[3].size() : 0;
            default: return "-";
        }
    }

    // text alignment
    if (role == Qt::TextAlignmentRole)
        //return (index.column() == 0)?
            return (int)(Qt::AlignLeft | Qt::AlignVCenter);
        //         :   (int)(Qt::AlignRight | Qt::AlignVCenter);

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

Qt::ItemFlags BioListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flag = QAbstractItemModel::flags(index);

    if (index.isValid())
    {
        flag |= Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    return flag;
}

QVariant BioListModel::headerData(int section, Qt::Orientation orientation, int role) const
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
