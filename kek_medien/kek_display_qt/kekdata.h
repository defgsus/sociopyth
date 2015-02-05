#ifndef KEKDATA_H
#define KEKDATA_H

#include <map>
#include <vector>
#include <memory>

#include <QString>

#include "springsystem.h"

class KekData
{
public:

    struct Company;

    struct Position
    {
        // spring-sys
        float x, y;
        bool fixed;
    };

    struct Share
    {
        Company * company;
        float percent;
    };

    struct Title : public Position
    {
        QString name, url;
    };

    struct Company : public Position
    {
        QString name, url;
        std::vector<Share> shares;
        std::vector<Title*> titles;
        int total_titles, total_shares;
        float total_shares_percent;

    private:
        friend KekData;
        bool counted_;
    };

    // ---------------- ctor -------------------

    KekData();
    ~KekData();

    // ----------------- handling --------------

    void clear();

    bool loadXml(const QString& fn);
    bool saveXml(const QString& fn);

    QString toString() const;

    /** Always returns a Company struct.
        Ownership stays with this class. */
    Company * getCompany(const QString& name);
    Title * getTitle(const QString& name);

    const std::vector<Company*>& companies() const { return companies_; }
    const std::vector<Title*>& titles() const { return titles_; }

    /** Returns the associated springsys node, or NULL */
    SpringSystem::Node * nodeForCompany(Company *);
    SpringSystem::Node * nodeForTitle(Title *);

    void getSpringSystem(SpringSystem *);

private:

    void calcValues_();
    void count_(Company * ) const;

    std::map<QString, std::shared_ptr<Company>> compmap_;
    std::map<QString, std::shared_ptr<Title>> titlemap_;
    std::map<Company*, SpringSystem::Node*> nodemap_;
    std::map<Title*, SpringSystem::Node*> tnodemap_;

    // for linear access
    std::vector<Company*> companies_;
    std::vector<Title*> titles_;
};

#endif // KEKDATA_H
