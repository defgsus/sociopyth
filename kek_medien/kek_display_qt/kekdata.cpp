
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

QString KekData::fullUrl(const QString &url)
{
    static const QString base_url = "http://kek-online.de/";
    return url.startsWith(base_url)
            ? base_url : (base_url + url);
}

void KekData::clear()
{
    compmap_.clear();
    titlemap_.clear();
    companies_.clear();
    titles_.clear();
    cnodemap_.clear();
    nodecmap_.clear();
    tnodemap_.clear();
}

KekData::Company * KekData::getCompany(const QString& name)
{
    auto i = compmap_.find(name);
    if (i != compmap_.end())
        return i->second.get();

    auto c = new Company;

    c->name = name;
    c->kek = this;

    auto p = std::shared_ptr<Company>(c);
    compmap_.insert(std::make_pair(name, p));
    companies_.push_back(c);

    return c;
}

KekData::Title * KekData::getTitle(const QString& name)
{
    auto i = titlemap_.find(name);
    if (i != titlemap_.end())
        return i->second.get();

    auto t = new Title;
    t->name = name;

    auto p = std::shared_ptr<Title>(t);
    titlemap_.insert(std::make_pair(name, p));
    titles_.push_back(t);

    return t;
}

SpringSystem::Node * KekData::nodeForCompany(Company * c)
{
    auto i = cnodemap_.find(c);
    if (i == cnodemap_.end())
        return 0;
    return i->second;
}

KekData::Company * KekData::companyForNode(SpringSystem::Node * n)
{
    auto i = nodecmap_.find(n);
    if (i == nodecmap_.end())
        return 0;
    return i->second;
}

int KekData::index(Company * c) const
{
    for (uint i=0; i<companies_.size(); ++i)
        if (companies_[i] == c)
            return i;
    return -1;
}

SpringSystem::Node * KekData::nodeForTitle(Title * c)
{
    auto i = tnodemap_.find(c);
    if (i == tnodemap_.end())
        return 0;
    return i->second;
}

QString KekData::toString() const
{
    QString ret;
    QTextStream s(&ret);

    for (auto const & i : compmap_)
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

        for (const Title * t : c->titles)
            s << "  T: \"" << t->name << "\"\n";
    }

    return ret;
}


bool KekData::loadXml(const QString &fn)
{
    QFile f(fn);
    if (!f.open(QFile::ReadOnly | QFile::Text))
        return false;

    clear();

    QDomDocument dom;

    if (!dom.setContent(&f))
        return false;

    auto nodes = dom.elementsByTagName("company");

    for (int i=0; i<nodes.length(); ++i)
    {
        auto n = nodes.item(i);

        QString compname = n.attributes().namedItem("name").nodeValue();
        //qDebug() << compname;

        auto c = getCompany(compname);
        c->url = n.attributes().namedItem("url").nodeValue();
        c->address = n.attributes().namedItem("address").nodeValue();
        c->remarks = n.attributes().namedItem("remarks").nodeValue();
        c->x = n.attributes().namedItem("x").nodeValue().toFloat();
        c->y = n.attributes().namedItem("y").nodeValue().toFloat();
        c->fixed = n.attributes().namedItem("fix").nodeValue().toInt();
        c->address.replace("\n", " | ");
        c->address = c->address.simplified();

        // titles
        auto childs = n.childNodes();
        for (int j=0; j<childs.length(); ++j)
        {
            auto n = childs.item(j);

            if (n.nodeName() == "title")
            {
                QString tname = n.attributes().namedItem("name").nodeValue();
                //qDebug() << "--- t ---" << tname;
                auto t = getTitle(tname);

                c->titles.push_back( t );
            }

            if (n.nodeName() == "share")
            {
                Share s;
                s.company = getCompany( n.attributes().namedItem("name").nodeValue() );
                s.percent = n.attributes().namedItem("p").nodeValue().toFloat();

                c->shares.push_back( s );
                //qDebug() << "--- s ---" << s.percent << s.company;
            }
        }

        std::sort(c->titles.begin(), c->titles.end(), [](Title * l, Title * r) { return l->name < r->name; } );
        std::sort(c->shares.begin(), c->shares.end(), [](const Share& l, const Share& r) { return l.percent > r.percent; } );
    }

    nodes = dom.elementsByTagName("title-desc");

    for (int i=0; i<nodes.length(); ++i)
    {
        auto n = nodes.item(i);

        QString name = n.attributes().namedItem("name").nodeValue();
        auto t = getTitle(name);

        t->url = n.attributes().namedItem("url").nodeValue();
        t->type = n.attributes().namedItem("type").nodeValue();
        t->x = n.attributes().namedItem("x").nodeValue().toFloat();
        t->y = n.attributes().namedItem("y").nodeValue().toFloat();
        t->fixed = n.attributes().namedItem("fix").nodeValue().toInt();
    }

    calcValues_();

#ifdef QT_DEBUG
    qDebug() << toString();
#endif

    return true;
}

bool KekData::saveXml(const QString &fn)
{
    QDomDocument dom;

    auto kek = dom.createElement("kek");
    dom.appendChild(kek);

    // --- companies ---

    auto comps = dom.createElement("companies");
    kek.appendChild(comps);

    for (auto const & i : compmap_)
    {
        Company * c = i.second.get();
        auto * n = nodeForCompany(c);

        auto comp = dom.createElement("company");
        comps.appendChild(comp);

        comp.setAttribute("name", c->name);
        comp.setAttribute("url", c->url);
        comp.setAttribute("address", c->address);
        comp.setAttribute("remarks", c->remarks);
        if (n)
        {
            comp.setAttribute("x", QString::number(n->pos.x()));
            comp.setAttribute("y", QString::number(n->pos.y()));
            if (n->locked)
                comp.setAttribute("fix", "1");
        }

        for (const Share & sh : c->shares)
        {
            auto share = dom.createElement("share");
            comp.appendChild(share);
            share.setAttribute("name", sh.company->name);
            share.setAttribute("p", QString::number(sh.percent));
        }

        for (const Title * t : c->titles)
        {
            auto title = dom.createElement("title");
            comp.appendChild(title);
            title.setAttribute("name", t->name);
        }
    }

    // --- titles ---
    auto tits = dom.createElement("titles");
    kek.appendChild(tits);
    for (auto const & i : titlemap_)
    {
        Title * t = i.second.get();
        auto * n = nodeForTitle(t);

        auto tdesc = dom.createElement("title-desc");
        tits.appendChild(tdesc);

        tdesc.setAttribute("name", t->name);
        tdesc.setAttribute("url", t->url);
        tdesc.setAttribute("type", t->type);
        if (n)
        {
            tdesc.setAttribute("x", QString::number(n->pos.x()));
            tdesc.setAttribute("y", QString::number(n->pos.y()));
            if (n->locked)
                tdesc.setAttribute("fix", "1");
        }
    }

    QFile f(fn);
    if (!f.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug() << "Could not write to" << fn;
        return false;
    }
    QTextStream str(&f);
    dom.save(str, 0);

    return true;
}


/* inefficient but easy */
void KekData::calcValues_()
{
    for (auto const & i : compmap_)
    {
        Company * c = i.second.get();
        count_(c);
    }
}

struct KekData::CountStruct
{
    int total_titles, total_shares;
    float total_shares_percent;
};

void KekData::count_(Company * c) const
{
    for (auto const & i : compmap_)
    {
        Company * c = i.second.get();
        c->visited_ = false;
    }

    CountStruct cs;
    cs.total_shares = cs.total_titles = 0;
    cs.total_shares_percent = 0.;
    countSub_(c, cs);
    c->total_shares = cs.total_shares;
    c->total_titles = cs.total_titles;
    c->total_shares_percent = cs.total_shares_percent;
}

void KekData::countSub_(Company * c, CountStruct& cs) const
{
    cs.total_shares += c->shares.size();
    cs.total_titles += c->titles.size();

    for (const Share & s : c->shares)
    {
        /*cs.total_shares_percent +=
                 s.percent
               + s.percent / 100. * s.company->total_shares_percent;
        */
        if (!s.company->visited_)
        {
            s.company->visited_ = true;
            countSub_(s.company, cs);
        }

    }
}


std::vector<KekData::Title*> KekData::getIndirectTitles(Company * c)
{
    if (c->shares.empty())
        return std::vector<Title*>();

    // clear the visit flag
    for (auto const & i : compmap_)
    {
        Company * c = i.second.get();
        c->visited_ = false;
    }

    std::vector<Title*> tits;
    for (const Share & s : c->shares)
        getTitles_(s.company, tits);
    return tits;
}

void KekData::getTitles_(Company * c, std::vector<Title *> & tits)
{
    c->visited_ = true;

    for (Title * t : c->titles)
        tits.push_back(t);

    for (const Share & s : c->shares)
        if (!s.company->visited_)
            getTitles_(s.company, tits);
}

void KekData::getSpringSystem(SpringSystem * sys)
{
    sys->clear();
    cnodemap_.clear();
    nodecmap_.clear();
    tnodemap_.clear();

    // create company nodes
    for (auto const & i : compmap_)
    {
        Company * c = i.second.get();

        auto node = sys->createNode();
        node->pos.setX(c->x);
        node->pos.setY(c->y);
        node->locked = c->fixed;
        node->color = QColor(100, 100, 100);
        if (c->x == 0 && c->y == 0)
        {
            node->pos.setX(4.*      std::min(200., 20.0 * std::pow(1.*c->total_shares, 1./1.5)));
            node->pos.setY(4.*(200.-std::min(200., 20.0 * std::pow(1.*c->total_titles, 1./1.5))));
            node->pos.setX(node->pos.x() + (.5-float(rand())/RAND_MAX));
            node->pos.setY(node->pos.y() + (.5-float(rand())/RAND_MAX));
        }
        //node->min_dist = std::max(3.,std::min(35., std::sqrt(c->total_shares) + c->shares.size() / 2. ));
        node->min_dist = std::max(5.,std::min(55.,  //std::sqrt(c->total_shares_percent) / 2.
                                                    //+ 5.
                                                      1.6 * std::max(c->titles.size(), c->shares.size())
                                                    + 0.3 * std::max(c->total_titles, c->total_shares) ));
        cnodemap_.insert(std::make_pair(c, node));
        nodecmap_.insert(std::make_pair(node, c));

        // create title nodes
        for (Title * t : c->titles)
        {
            auto tnode = sys->createNode();
            tnode->pos.setX(t->x);
            tnode->pos.setY(t->y);
            tnode->locked = t->fixed;
            if (t->type == "Radio")
                tnode->color = QColor(200, 200, 90);
            else if (t->type == "Fernsehen")
                tnode->color = QColor(200, 90, 90);
            else if (t->type == "Presse")
                tnode->color = QColor(90, 200, 90);
            else // "Online"
                tnode->color = QColor(90, 90, 200);
            if (t->x == 0 && t->y == 0)
            {
                tnode->pos.setX(tnode->pos.x() + (.5-float(rand())/RAND_MAX));
                tnode->pos.setY(tnode->pos.y() + (.5-float(rand())/RAND_MAX));
            }
            tnode->min_dist = node->min_dist;

            tnodemap_.insert(std::make_pair(t, tnode));
        }

    }

    // connections

    for (auto const & i : compmap_)
    {
        Company * c = i.second.get();
        auto n1 = nodeForCompany(c);

        // connect shares
        for (const Share & s : c->shares)
        {
            auto n2 = nodeForCompany(s.company);

            float dist = 3. + 10. * (1. - s.percent/100.);

            dist = std::max(dist, std::max(n1->min_dist, n2->min_dist));
            //dist = std::max(dist, n1->min_dist + n2->min_dist);

            auto spring = sys->connect(n1, n2, dist);
            spring->stiff = 0.001 + 0.999 * std::pow(s.percent / 100., 2.);
        }

        // connect titles
        for (Title * t : c->titles)
        {
            auto n2 = nodeForTitle(t);

            float dist = std::max(n1->min_dist, n2->min_dist);

            sys->connect(n1, n2, dist);
        }
    }
}


