#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>

#include "kekscene.h"


class KekItem : public QGraphicsEllipseItem
{
public:

    SpringSystem::Node * node;

    KekItem(SpringSystem::Node * node, QGraphicsItem * parent = 0)
        : QGraphicsEllipseItem(parent),
          node(node)
    {
        setFlag(QGraphicsItem::ItemIsMovable);
        //setFlag(QGraphicsItem::ItemSendsScenePositionChanges);

        setBrush(QBrush(node->color));
        QPen pen(node->color.lighter());
        pen.setWidthF(0.1);
        setPen(pen);

        setRadius( std::max(1.,std::min(5., 0.4 * node->min_dist )) );
    }

    void setRadius(qreal r) { setRect(-r/2., -r/2., r, r); }


    //QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mousePressEvent(QGraphicsSceneMouseEvent * e)
    {
        QGraphicsEllipseItem::mousePressEvent(e);
        if (e->button() == Qt::LeftButton)
        if (auto kek = qobject_cast<KekScene*>(scene()))
        {
            emit kek->nodeSelected(node);
        }
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

        auto e = new KekItem(n);

        n->user = e;

        addItem(e);
    }

    for (auto & i : sys_->springs())
    {
        SpringSystem::Spring * s = i.get();

        auto e = new QGraphicsLineItem();
        QPen pen(Qt::green);
        pen.setWidthF(s->stiff);
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
        SpringSystem::Spring * s = i.get();

        auto e = static_cast<QGraphicsLineItem*>(s->user);

        e->setLine(s->n1->pos.x(), s->n1->pos.y(),
                   s->n2->pos.x(), s->n2->pos.y());
    }

    update();
}
