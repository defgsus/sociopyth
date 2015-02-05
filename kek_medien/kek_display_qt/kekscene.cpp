#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>

#include "kekscene.h"



KekScene::KekScene(QObject *parent)
    : QGraphicsScene    (parent),
      sys_              (0)
{

}

KekScene::~KekScene()
{

}



void KekScene::setSpringSystem(SpringSystem *s)
{
    sys_ = s;

    clear();

    for (auto & i : sys_->nodes())
    {
        SpringSystem::Node * n = i.get();

        auto e = new QGraphicsEllipseItem(-.5, -.5, 1., 1.);
        e->setBrush(QBrush(Qt::gray));
        QPen pen(Qt::white);
        pen.setWidthF(0.1);
        e->setPen(pen);

        n->user = e;

        addItem(e);
    }

    for (auto & i : sys_->springs())
    {
        SpringSystem::Spring * s = i.get();

        auto e = new QGraphicsLineItem();
        QPen pen(Qt::green);
        pen.setWidthF(0.1);
        e->setPen(pen);

        s->user = e;

        addItem(e);
    }

    updatePositions();
}

void KekScene::updatePositions()
{
    for (auto & i : sys_->nodes())
    {
        SpringSystem::Node * n = i.get();

        auto e = static_cast<QGraphicsEllipseItem*>(n->user);

        e->setPos(n->pos.x(), n->pos.y());
    }

    for (auto & i : sys_->springs())
    {
        SpringSystem::Spring * n = i.get();

        auto e = static_cast<QGraphicsLineItem*>(n->user);

        e->setLine(n->n1->pos.x(), n->n1->pos.y(),
                   n->n2->pos.x(), n->n2->pos.y());
    }

    update();
}
