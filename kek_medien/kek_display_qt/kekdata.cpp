
#include <QDomDocument>
#include <QFile>
#include <QDebug>

#include "kekdata.h"

KekData::KekData()
{

}

KekData::~KekData()
{

}


void KekData::clear()
{
    companies_.clear();
}


KekData::Company * KekData::getCompany(const QString& name)
{
    auto i = companies_.find(name);
    if (i != companies_.end())
        return i->second.get();

    auto c = new Company;
    c->name = name;

    auto p = std::shared_ptr<Company>(c);
    companies_.insert(std::make_pair(name, p));

    return c;
}


bool KekData::loadXml(const QString &fn)
{
    QFile f(fn);
    if (!f.open(QFile::ReadOnly | QFile::Text))
        return false;

    QDomDocument dom;

    if (!dom.setContent(&f))
        return false;

    auto nodes = dom.elementsByTagName("company");

    for (int i=0; i<nodes.length(); ++i)
    {
        auto n = nodes.item(i);

        QString compname = n.attributes().namedItem("name").nodeValue();

        auto c = getCompany(compname);
        c->url = n.attributes().namedItem("url").nodeValue();
        qDebug() << c->name;

        // titles
        auto childs = n.childNodes();
        for (int j=0; j<childs.length(); ++j)
        {
            auto n = childs.item(j);

            if (n.nodeName() == "title")
            {
                Title t;
                t.name = n.attributes().namedItem("name").nodeValue();

                c->titles.push_back( t );
                qDebug() << "--- t ---" << t.name;
            }

            if (n.nodeName() == "share")
            {
                Share s;
                s.company = getCompany( n.attributes().namedItem("name").nodeValue() );
                s.percent = n.attributes().namedItem("p").nodeValue().toFloat();

                c->shares.push_back( s );
                qDebug() << "--- s ---" << s.percent << s.company;
            }
        }

    }

    return true;
}

SpringSystem::Node * KekData::nodeForCompany(Company * c)
{
    auto i = nodemap_.find(c);
    if (i == nodemap_.end())
        return 0;
    return i->second;
}

void KekData::getSpringSystem(SpringSystem * sys)
{
    sys->clear();

    // create company nodes
    for (auto const & i : companies_)
    {
        Company * c = i.second.get();

        auto node = sys->createNode();
        node->pos.setX(100. * (.5+float(rand())/RAND_MAX));
        node->pos.setY(100. * (.5+float(rand())/RAND_MAX));
        nodemap_.insert(std::make_pair(c, node));
    }

    for (auto const & i : companies_)
    {
        Company * c1 = i.second.get();
        auto n1 = nodeForCompany(c1);

        // connect shares
        for (const Share & s : c1->shares)
        {
            auto n2 = nodeForCompany(s.company);

            float dist = 10. * (1. - s.percent/120.);

            sys->connect(n1, n2, dist);
        }
    }
}
