#ifndef KEKSCENE_H
#define KEKSCENE_H

#include <QGraphicsScene>

#include "springsystem.h"

class KekScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit KekScene(QObject *parent = 0);
    ~KekScene();

signals:

public slots:

    void setSpringSystem(SpringSystem * s);

    void updatePositions();

private:

    SpringSystem * sys_;
};

#endif // KEKSCENE_H
