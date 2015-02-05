
#include <QDomDocument>
#include <QFile>
#include <QTextStream>
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

SpringSystem::Node * KekData::nodeForCompany(Company * c)
{
    auto i = nodemap_.find(c);
    if (i == nodemap_.end())
        return 0;
    return i->second;
}

QString KekData::toString() const
{
    QString ret;
    QTextStream s(&ret);

    for (auto const & i : companies_)
    {
        Company * c = i.second.get();

        s << "[" << c->name << "]";
        if (c->total_titles)
            s << " total titles " << c->total_titles << ";";
        if (c->total_shares)
            s << " total shares " << c->total_shares << " (" << c->total_shares_percent << "%);";
        s << "\n";

        for (const Share & sh : c->shares)
            s << "  S: " << sh.percent << "% " << sh.company->name << "\n";

        for (const Title & t : c->titles)
            s << "  T: \"" << t.name << "\"\n";
    }

    return ret;
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

    calcValues_();

    qDebug() << toString();

    return true;
}



void KekData::calcValues_()
{
    for (auto const & i : companies_)
    {
        Company * c = i.second.get();
        c->counted_ = false;
        c->total_shares = c->shares.size();
        c->total_titles = c->titles.size();
        c->total_shares_percent = 0;
    }

    for (auto const & i : companies_)
    {
        Company * c = i.second.get();

        count_(c);
    }
}

/* inefficient and easy */
void KekData::count_(Company * c) const
{
    int t = 0, ts = 0;
    float tsp = 0;
    for (const Share & s : c->shares)
    {
        if (!s.company->counted_)
        {
            s.company->counted_ = true;
            count_(s.company);
        }
        t += s.company->total_titles;
        ts += s.company->total_shares;
        tsp += s.percent
               + s.percent / 100. * s.company->total_shares_percent;
    }
    c->total_titles += t;
    c->total_shares += ts;
    c->total_shares_percent = tsp;
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

            float dist = 3. + 10. * (1. - s.percent/100.);

            sys->connect(n1, n2, dist);
        }
    }
}


