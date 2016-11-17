/** @file biodata.h

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 15.02.2015</p>
*/

#ifndef BIODATA_H
#define BIODATA_H

#include <QObject>
#include <QStringList>
#include <QVector>

class BioData : public QObject
{
    Q_OBJECT
public:
    struct Person
    {
        QString
            name,
            occupation,
            birth,
            url,
            img_url,
            wahlkreis;
        QStringList statements;
        QVector<QStringList> memberships;
    };

    explicit BioData(QObject *parent = 0);

    // --------------- handling -----------------

    void clear();

    bool loadXml(const QString& filename);

    // ------------- access ---------------------

    uint numPersons() const { return persons_.size(); }

    const Person& person(uint index) const { return persons_[index]; }

    bool isFulltextMatch(uint index, const QString& text) const;

signals:

public slots:

private:

    QList<Person> persons_;
};

#endif // BIODATA_H
