#ifndef KEKDATA_H
#define KEKDATA_H

#include <map>
#include <set>
#include <vector>
#include <memory>

#include <QString>

#include "springsystem.h"

class KekData
{
public:

    struct Company;

    // for spring-sys
    struct Position
    {
        float x, y;
        bool fixed;
        Position() : x(0), y(0), fixed(false) { }
    };

    // recursively calculated values
    struct Heuristics
    {
        int total_titles, total_shares, cluster_size;
        float total_titles_percent,
              total_shares_percent;
    };

    struct Share
    {
        Company * company;
        float percent;
    };

    struct Title : public Position
    {
        QString name, url, type;

        QString fullUrl() const { return KekData::fullUrl(url); }
    };

    struct Company : public Position, public Heuristics
    {
        QString name, url, address, remarks;
        std::vector<Share> shares, owners;
        std::vector<Title*> titles;

        QString fullUrl() const { return KekData::fullUrl(url); }

        /** Link to parent container */
        KekData * kek;
        unsigned long index, clusterId, clusterMembers;
    private:
        friend KekData;
        bool visited_;
    };

    // ---------------- ctor -------------------

    KekData();
    ~KekData();

    /** Returns the full url, with kek-online... at the front */
    static QString fullUrl(QString url);

    // ----------------- handling --------------

    void clear();

    bool loadXml(const QString& fn);
    bool saveXml(const QString& fn);

    QString toString() const;

    QColor clusterColor(unsigned long) const;

    /** Always returns a Company struct.
        Ownership stays with this class. */
    Company * getCompany(const QString& name);
    Title * getTitle(const QString& name);

    const std::vector<Company*>& companies() const { return companies_; }
    const std::vector<Title*>& titles() const { return titles_; }

    /** Returns the associated springsys node, or NULL */
    SpringSystem::Node * nodeForCompany(Company *);
    SpringSystem::Node * nodeForTitle(Title *);
    Company * companyForNode(SpringSystem::Node*);

    /** Returns the index of the company (arbitrary - but needed for the list model) */
    int index(Company *) const;

    void getSpringSystem(SpringSystem *);

    /** Returns all indirect titles of the company,
        that are connected through shares */
    std::vector<Title*> getIndirectTitles(Company *);

private:

    void clearVisited_();
    void calcValues_();
    void count_(Company *);
    void countSub_(Company *, Heuristics&h, float factor);
    int countCluster_(Company * );
    void getOwners_();
    void getIndirectTitles_(Company *, std::set<Title*>&);

    void getClusters_();

    std::map<QString, std::shared_ptr<Company>> compmap_;
    std::map<QString, std::shared_ptr<Title>> titlemap_;
    std::map<Company*, SpringSystem::Node*> cnodemap_;
    std::map<Title*, SpringSystem::Node*> tnodemap_;
    std::map<SpringSystem::Node*, Company*> nodecmap_;

    // for linear access
    std::vector<Company*> companies_;
    std::vector<Title*> titles_;
};

#endif // KEKDATA_H
