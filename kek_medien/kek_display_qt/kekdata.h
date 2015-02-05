#ifndef KEKDATA_H
#define KEKDATA_H

#include <QString>


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
        std::vector<Share*> shares;
        std::vector<Title*> titles;
    };


    KekData();
    ~KekData();
};

#endif // KEKDATA_H
