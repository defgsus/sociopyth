/** @file filtermodel.cpp

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 15.02.2015</p>
*/

#include "filtermodel.h"
#include "biodata.h"

FilterModel::FilterModel(QObject *parent)
    : QSortFilterProxyModel (parent)
    , data_                 (0)
{
}

void FilterModel::setFulltextFilter(const QString &s)
{
    fulltextFilter_ = s; invalidate();
    fulltextFilters_.clear();
    auto list = s.split(";");
    for (QString s : list)
    {
        s = s.simplified();
        if (!s.isEmpty())
            fulltextFilters_.append(s);
    }

    invalidate();
}

bool FilterModel::filterAcceptsRow(
        int source_row, const QModelIndex &source_parent) const
{
    if (!QSortFilterProxyModel::filterAcceptsRow(
                source_row, source_parent))
        return false;

    if (fulltextFilter_.isEmpty() || !data_)
        return true;

    if (source_row >= (int)data_->numPersons())
        return false;

    for (auto & s : fulltextFilters_)
        if (data_->isFulltextMatch(source_row, s))
            return true;

    return false;
}
