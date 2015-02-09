/** @file companyview.cpp

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 09.02.2015</p>
*/

#include <QTextStream>

#include "companyview.h"

CompanyView::CompanyView(QWidget *parent) :
    QTextBrowser(parent)
{
}



void CompanyView::setCompany(KekData::Company * c)
{
    clear();

    QString text;
    QTextStream s(&text);

    s << "<html><h1>" << c->name << "</h1>\n";

    if (!c->shares.empty())
    {
        s << "<p>" << tr("shares") << "(" << c->shares.size() << "):<ul>\n";

        for (const KekData::Share & sh : c->shares)
        {
            s << "<li>" << sh.percent << "% <a href=\"" << sh.company->fullUrl() << "\">"
              << sh.company->name << "</a></li>\n";
        }

        s << "</ul></p>\n";
    }

    if (!c->titles.empty())
    {
        s << "<p>" << tr("titles") << "(" << c->titles.size() << "):<ul>\n";

        for (const KekData::Title * t : c->titles)
        {
            s << "<li><a href=\"" << t->fullUrl() << "\">" << t->name << "</a> "
              << "(" << t->type << ")</li>\n";
        }

        s << "</ul></p>\n";
    }

    auto tits = c->kek->getIndirectTitles(c);
    if (!tits.empty())
    {
        std::sort(tits.begin(), tits.end(), [](KekData::Title * l, KekData::Title * r) { return l->name < r->name; } );

        s << "<p>" << tr("indirect titles") << "(" << tits.size() << "):<ul>\n";

        for (const KekData::Title * t : tits)
        {
            s << "<li><a href=\"" << t->fullUrl() << "\">" << t->name << "</a> "
              << "(" << t->type << ")</li>\n";
        }

        s << "</ul></p>\n";
    }

    s << "<p>" << tr("address") << ": <b>" << c->address << "</b></p>\n"
      << "<p>" << tr("remarks") << ": <b>" << c->remarks << "</b></p>\n"
      << "<p>" << tr("view at") << " <a href=\"" << c->fullUrl() << "\">kek-online.de</a></p>\n";

    setHtml(text);
}
