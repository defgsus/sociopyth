#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>

#include "kekscene.h"


class KekItem : public QGraphicsEllipseItem
{
public:

    KekScene * scene;
    SpringSystem::Node * node;

    KekItem(KekScene * scene, SpringSystem::Node * node, QGraphicsItem * parent = 0)
        : QGraphicsEllipseItem  (parent)
        , scene                 (scene)
        , node                  (node)
    {
        setFlag(QGraphicsItem::ItemIsMovable);
        //setFlag(QGraphicsItem::ItemSendsScenePositionChanges);

        setBrush(QBrush(node->color));
        QPen pen(node->color.darker());
        pen.setWidthF(0.1);
        setPen(pen);

        setRadius( std::max(2.,std::min(10., 0.4 * node->min_dist )) );
    }

    void setRadius(qreal r) { setRect(-r/2., -r/2., r, r); }


    //QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mousePressEvent(QGraphicsSceneMouseEvent * e)
    {
        QGraphicsEllipseItem::mousePressEvent(e);
        if (e->button() == Qt::LeftButton)
        emit scene->nodeSelected(node);
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent * e)
    {
        QGraphicsEllipseItem::mouseMoveEvent(e);

        // notify SpringSystem
        if (e->buttons() & Qt::LeftButton)
        {
            // XXX not threadsafe
            node->pos.setX(pos().x());
            node->pos.setY(pos().y());
            node->intertia = SpringSystem::vec2(0,0);
            node->locked = true;
            emit scene->nodeChanged(node);
        }
    }

    void mouseReleaseEvent(QGraphicsSceneMouseEvent * e)
    {
        QGraphicsEllipseItem::mouseReleaseEvent(e);
        if (e->button() == Qt::RightButton)
        {
            node->locked = false;
        }
    }
};


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

        auto e = new KekItem(this, n);

        n->user = e;

        addItem(e);
    }

    for (auto & i : sys_->springs())
    {
        SpringSystem::Spring * s = i.get();

        auto e = new QGraphicsLineItem();
        QPen pen(s->n1->color);
        pen.setWidthF(0.1 + s->stiff);
        e->setPen(pen);
        e->setZValue(-1);

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
        SpringSystem::Spring * s = i.get();

        auto e = static_cast<QGraphicsLineItem*>(s->user);

        e->setLine(s->n1->pos.x(), s->n1->pos.y(),
                   s->n2->pos.x(), s->n2->pos.y());
    }

    update();
}
