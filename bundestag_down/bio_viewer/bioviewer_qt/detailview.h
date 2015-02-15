/** @file detailview.h

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 15.02.2015</p>
*/

#ifndef DETAILVIEW_H
#define DETAILVIEW_H

#include <QTextBrowser>
#include <QStringList>

#include "biodata.h"

class Highlighter;

class DetailView : public QTextBrowser
{
    Q_OBJECT
public:
    explicit DetailView(QWidget *parent = 0);

signals:


public slots:

    void setPerson(const BioData::Person*);

    void setTextFilter(const QString& s);

private slots:

    void onAnchorClicked_(const QUrl & link);

private:

    void updateFilter_();

    Highlighter * highlighter_;
    QStringList textFilters_;
};
#endif // DETAILVIEW_H
