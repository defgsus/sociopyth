/** @file kekmodel.h

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 05.02.2015</p>
*/

#ifndef KEKMODEL_H
#define KEKMODEL_H

#include <QAbstractItemModel>

class KekData;

class KekModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit KekModel(KekData *, QObject *parent = 0);

//    QObject * itemForIndex(const QModelIndex& index) const;

    // --- interface impl. ---

    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QModelIndex parent(const QModelIndex &index) const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

signals:

public slots:

private:

    KekData * kek_;

    QStringList headerNames_;
};
#endif // KEKMODEL_H
