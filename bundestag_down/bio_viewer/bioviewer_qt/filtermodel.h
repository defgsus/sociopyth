/** @file filtermodel.h

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 15.02.2015</p>
*/

#ifndef FILTERMODEL_H
#define FILTERMODEL_H

#include <QSortFilterProxyModel>
#include <QStringList>

class BioData;

class FilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit FilterModel(QObject *parent = 0);

signals:

public slots:

    void setBioData(const BioData * d) { data_ = d; invalidate(); }
    void setFulltextFilter(const QString& s);

protected:

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:

    const BioData * data_;
    QString fulltextFilter_;
    QStringList fulltextFilters_;
};

#endif // FILTERMODEL_H
