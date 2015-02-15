
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
//http://kek-online.de/no_cache/information/mediendatenbank.html?L=0%23&m=735&mt=1,2,3,4&s=&f=0
QString KekData::fullUrl(QString url)
{
    static const QString base_url = "http://www.kek-online.de/";

    if (!url.startsWith(base_url))
        url.prepend(base_url);

    // also fix the stupid & at the end
    while (url.endsWith("&"))
        url = url.left(url.size()-1);

    return url;
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
    c->index = compmap_.size();
    c->clusterId = 0;

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

#if 0
    auto tmp = compmap_;
    compmap_.clear();
    for (auto & i : tmp)
    {
        Company * c = i.second.get();
        if (c->shares.size()>=4)
            compmap_.insert(std::make_pair(c->name, i.second));
    }
#endif

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


// ------------------------- value calculation --------------------

// Well, this stuff is quite inefficient and probably wrong
// i wasn't spending too much time here yet.

void KekData::clearVisited_()
{
    // clear the visit flag
    for (auto const & i : compmap_)
    {
        Company * c = i.second.get();
        c->visited_ = false;
    }
}

void KekData::calcValues_()
{
    getOwners_();
    getClusters_();

    for (auto const & i : compmap_)
    {
        Company * c = i.second.get();
        count_(c);
    }
}


void KekData::count_(Company * c)
{
    clearVisited_();

    Heuristics h;
    h.total_shares = h.total_titles = 0;
    h.total_shares_percent = h.total_titles_percent = 0.;
    countSub_(c, h, 1.);
    c->total_shares = h.total_shares;
    c->total_titles = h.total_titles;
    c->total_titles_percent = h.total_titles_percent;
    c->total_shares_percent = h.total_shares_percent;

    clearVisited_();
    c->cluster_size = countCluster_(c);
}

void KekData::countSub_(Company * c, Heuristics& h, float factor)
{
    h.total_shares += c->shares.size();
    h.total_titles += c->titles.size();
    h.total_titles_percent += factor * c->titles.size();
    h.total_shares_percent += factor * c->shares.size();
    h.cluster_size++;

    if (!c->visited_)
    for (const Share & s : c->shares)
    {
        s.company->visited_ = true;
        countSub_(s.company, h, factor * s.percent / 100.);
    }
}

int KekData::countCluster_(Company * c)
{
    int ret = c->shares.size() + c->owners.size();

    c->visited_ = true;

    for (const Share & s : c->shares)
        if (!s.company->visited_)
            ret += countCluster_(s.company);
    for (const Share & s : c->owners)
        if (!s.company->visited_)
            ret += countCluster_(s.company);
    return ret;
}

void KekData::getOwners_()
{
    for (auto const & i : compmap_)
    {
        Company * c = i.second.get();

        for (const Share & s : c->shares)
        {
            Share o;
            o.company = c;
            o.percent = s.percent;
            s.company->owners.push_back(o);
        }
    }

    for (auto const & i : compmap_)
    {
        Company * c = i.second.get();
        std::sort(c->owners.begin(), c->owners.end(), [](const Share& l, const Share& r) { return l.percent > r.percent; } );
    }
}


std::vector<KekData::Title*> KekData::getIndirectTitles(Company * c)
{
    if (c->shares.empty())
        return std::vector<Title*>();

    clearVisited_();

    std::set<Title*> tits;
    for (const Share & s : c->shares)
        getIndirectTitles_(s.company, tits);

    std::vector<Title*> ret;
    for (Title * t : tits)
        ret.push_back(t);
    return ret;
}


void KekData::getIndirectTitles_(Company * c, std::set<Title *> & tits)
{
    c->visited_ = true;

    for (Title * t : c->titles)
        tits.insert(t);

    for (const Share & s : c->shares)
        if (!s.company->visited_)
            getIndirectTitles_(s.company, tits);
}

QColor KekData::clusterColor(unsigned long i) const
{
    return QColor(
                (int)(155.+100.*std::sin(i*1.01)),
                (int)(155.+100.*std::sin(i*1.313)),
                (int)(155.+100.*std::sin(i*1.531)));
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
        node->color = clusterColor(c->clusterId);//QColor(100, 100, 100);
        if (c->x == 0 && c->y == 0)
        {
            /*
            if (c->cluster_size < 100)
            {
                node->pos.setX(-200. - c->total_shares * 100.);
                node->pos.setY(500. + c->total_titles * 100.);
            }
            else
            {
                node->pos.setX(4.*      std::min(400., 20.0 * std::pow(1.*c->total_shares, 1./1.6)));
                node->pos.setY(4.*(400.-std::min(400., 20.0 * std::pow(1.*c->total_titles, 1./1.6))));
            }
            */
            node->pos.setX(std::min(1000., c->clusterMembers * 40.));
            node->pos.setY(c->clusterId);
            if (c->cluster_size < 100)
                node->pos.setX(node->pos.x() - 300.);

            node->pos.setX(node->pos.x() + (.5-float(rand())/RAND_MAX));
            node->pos.setY(node->pos.y() + (.5-float(rand())/RAND_MAX));
        }
        node->min_dist = std::max(5.,std::min(75.,
                              1.1 * c->titles.size()
                            + 0.5 * std::max(2.0 * c->shares.size(), 1.0 * c->owners.size())
                            + 0.6 * std::max(c->total_titles_percent, c->total_shares_percent) ));
//        if (node->min_dist>74.)
//            node->locked = true;

        cnodemap_.insert(std::make_pair(c, node));
        nodecmap_.insert(std::make_pair(node, c));

#define DO_TITLES
#ifdef DO_TITLES
        // create title nodes
        for (Title * t : c->titles)
        {
            auto tnode = sys->createNode();
            tnode->pos.setX(t->x);
            tnode->pos.setY(t->y);
            tnode->locked = t->fixed;
#if 0
            if (t->type == "Radio")
                tnode->color = QColor(200, 200, 90);
            else if (t->type == "Fernsehen")
                tnode->color = QColor(200, 90, 90);
            else if (t->type == "Presse")
                tnode->color = QColor(90, 200, 90);
            else // "Online"
                tnode->color = QColor(90, 90, 200);
#else
            tnode->color = node->color;
#endif
            if (t->x == 0 && t->y == 0)
            {
                tnode->pos.setX(node->pos.x() + (.5-float(rand())/RAND_MAX));
                tnode->pos.setY(node->pos.y() + (.5-float(rand())/RAND_MAX));
            }
            tnode->min_dist = node->min_dist;

            tnodemap_.insert(std::make_pair(t, tnode));
        }
#endif
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
#if 0
            if (c->clusterId != s.company->clusterId)
                dist = std::min(150., dist * 5.);
#endif

            auto spring = sys->connect(n1, n2, dist);
            spring->stiff = 0.001 + 0.999 * std::pow(s.percent / 100., 2.);
        }
#ifdef DO_TITLES
        // connect titles
        for (Title * t : c->titles)
        {
            auto n2 = nodeForTitle(t);

            float dist = std::max(n1->min_dist, n2->min_dist);

            sys->connect(n1, n2, dist);
        }
#endif
    }
}


