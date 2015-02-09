/** @file companyview.h

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 09.02.2015</p>
*/

#ifndef COMPANYVIEW_H
#define COMPANYVIEW_H

#include <QTextBrowser>
#include "kekdata.h"

class CompanyView : public QTextBrowser
{
    Q_OBJECT
public:
    explicit CompanyView(QWidget *parent = 0);

signals:

public slots:

    void setCompany(KekData::Company*);

};

#endif // COMPANYVIEW_H
