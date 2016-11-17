/** @file biodata.cpp

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 15.02.2015</p>
*/

#include <QDomDocument>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "biodata.h"

BioData::BioData(QObject *parent) :
    QObject(parent)
{
}


void BioData::clear()
{
    persons_.clear();
}


bool BioData::loadXml(const QString &filename)
{
    QFile f(filename);
    if (!f.open(QFile::ReadOnly | QFile::Text))
        return false;

    clear();

    QDomDocument dom;

    if (!dom.setContent(&f))
        return false;

    auto nodes = dom.elementsByTagName("person");

    for (int i=0; i<nodes.length(); ++i)
    {
        auto n = nodes.item(i);

        Person p;
        p.name = n.attributes().namedItem("name").nodeValue();
        p.url = n.attributes().namedItem("url").nodeValue();
        p.img_url = n.attributes().namedItem("img_url").nodeValue();
        p.occupation = n.attributes().namedItem("occupation").nodeValue();
        p.birth = n.attributes().namedItem("birth").nodeValue();
        p.wahlkreis = n.attributes().namedItem("wahlkreis").nodeValue();

        auto childs = n.childNodes();
        for (int j=0; j<childs.length(); ++j)
        {
            auto n = childs.item(j);

            if (n.nodeName() == "statement")
                p.statements << n.attributes().namedItem("value").nodeValue();
            if (n.nodeName() == "member")
            {
                int t = n.attributes().namedItem("type").nodeValue().toInt();
                if (p.memberships.size() <= t)
                    p.memberships.resize(t+1);
                p.memberships[t].append( n.attributes().namedItem("value").nodeValue() );
            }
        }

        persons_ << p;
    }

    return true;
}


bool BioData::isFulltextMatch(uint index, const QString &text) const
{
    if (index >= numPersons())
        return false;

    auto p = person(index);

    if (   p.name.contains(text, Qt::CaseInsensitive)
        || p.occupation.contains(text, Qt::CaseInsensitive)
        || p.wahlkreis.contains(text, Qt::CaseInsensitive)
        || p.birth.contains(text, Qt::CaseInsensitive))
        return true;

    for (auto s : p.statements)
        if (s.contains(text, Qt::CaseInsensitive))
            return true;

    for (auto l : p.memberships)
        for (auto s : l)
            if (s.contains(text, Qt::CaseInsensitive))
                return true;

    return false;
}
