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

    struct Share
    {
        Company * company;
        float percent;
    };

    struct Title
    {
        QString name, url;
    };

    struct Company
    {
        QString name, url;
        std::vector<Share> shares;
        std::vector<Title> titles;
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

    QString toString() const;

    /** Always returns a Company struct.
        Ownership stays with this class. */
    Company * getCompany(const QString& name);

    /** Returns the associated springsys node, or NULL */
    SpringSystem::Node * nodeForCompany(Company *);

    void getSpringSystem(SpringSystem *);

private:

    void calcValues_();
    void count_(Company * ) const;

    std::map<QString, std::shared_ptr<Company>> companies_;
    std::map<Company*, SpringSystem::Node*> nodemap_;
};

#endif // KEKDATA_H
