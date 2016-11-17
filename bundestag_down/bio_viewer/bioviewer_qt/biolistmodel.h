
/** @file biolistmodel.h

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 15.02.2015</p>
*/

#ifndef BIOLISTMODEL_H
#define BIOLISTMODEL_H

#include <QAbstractItemModel>

class BioData;

/** List model to display all companies */
class BioListModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit BioListModel(BioData *, QObject *parent = 0);

    // --- interface impl. ---

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &index) const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

signals:

public slots:

private:

    BioData * data_;

    QStringList headerNames_;
};

#endif // BIOLISTMODEL_H
