
#include <random>

#include <QThread>
#include <QTextStream>

#include "springsystem.h"


class SpringSystem::Thread : public QThread
{
public:
    Thread(SpringSystem * s, QObject * p = 0) : QThread(p), sys(s), stop(false) { }

    void run() Q_DECL_OVERRIDE
    {
        stop = false;
        while (!stop)
        {
            sys->step();
        }
    }

    SpringSystem * sys;
    volatile bool stop;
};




SpringSystem::SpringSystem()
    : thread_       (0),
      frame_        (0),
      stiffness_    (1.),
      delta_        (0.02)
{

}

SpringSystem::~SpringSystem()
{
    stopThread();
    delete thread_;
}

void SpringSystem::clear()
{
    nodes_.clear();
    springs_.clear();
}

SpringSystem::Node * SpringSystem::createNode(void *usr)
{
    static std::mt19937 rnd;

    auto n = new Node;
    n->user = usr;
    n->pos = vec2(Float(rnd()) / rnd.max() - .5, Float(rnd()) / rnd.max() - .5) * 10.;
    n->locked = false;
    n->min_dist = 1;

    nodes_.push_back(std::shared_ptr<Node>(n));
    return n;
}

SpringSystem::Spring * SpringSystem::connect(Node * n1, Node * n2, Float rest_distance, void *usr)
{
    auto s = new Spring;
    s->n1 = n1;
    s->n2 = n2;
    s->rest_dist = rest_distance;
    s->dist = n1->pos.distanceToPoint(n2->pos);
    s->user = usr;
    s->stiff = 1;

    springs_.push_back(std::shared_ptr<Spring>(s));
    return s;
}

QString SpringSystem::toString() const
{
    QString t;
    QTextStream str(&t);

    for (auto i : nodes_)
    {
        Node * n = i.get();
        str << "Node(pos=<" << n->pos.x() << ", " << n->pos.y()
            << ">; vel=<" << n->intertia.x() << ", " << n->intertia.y() << ">)\n";
    }

    for (auto i : springs_)
    {
        Spring * s = i.get();
        str << "Spring(" << s->n1 << "-" << s->n2 << "; "
            << "dist=" << s->dist << "; rest=" << s->rest_dist << "; stiff=" << s->stiff << ")\n";
    }

    return t;
}


void SpringSystem::startThread()
{
    if (!thread_)
        thread_ = new Thread(this);
    thread_->start();
}

void SpringSystem::stopThread()
{
    if (thread_ && thread_->isRunning())
    {
        thread_->stop = true;
        thread_->wait();
    }
}

bool SpringSystem::isRunning() const
{
    return thread_ && thread_->isRunning();
}


void SpringSystem::step(Float delta)
{
    if (frame_ % 50 == 0)
        relaxDistance(delta);
    relaxSprings(delta);
    applyIntertia(delta, 0.2);

    ++frame_;
}

void SpringSystem::relaxSprings(Float delta)
{
    for (auto & i : springs_)
    {
        Spring * s = i.get();

        // messure direction/distance
        vec2 dir = s->n2->pos - s->n1->pos;
        s->dist = dir.length();

        // spring force
        vec2 f = std::max(Float(-.1),std::min(Float(.1), Float(.5) * delta * stiffness_ * s->stiff * (s->rest_dist - s->dist)))
                    * (dir / s->dist);
        if (!s->n1->locked)
            s->n1->intertia -= f;
        if (!s->n2->locked)
            s->n2->intertia += f;
    }
}

void SpringSystem::relaxDistance(Float delta)
{
    for (uint i=0; i<nodes_.size(); ++i)
    {
        Node * n1 = nodes_[i].get();

        for (uint j=i+1; j<nodes_.size(); ++j)
        {
            Node * n2 = nodes_[j].get();

            if (n1->locked && n2->locked)
                continue;

            Float mind = std::max(n1->min_dist, n2->min_dist);
            mind = std::max(mind, 10.f);

            vec2 dir = n2->pos - n1->pos;
            Float ds = dir.lengthSquared();

            if (ds < mind*mind && ds > Float(0))
            {
                Float d = std::sqrt(ds);
                vec2 f = delta * .5 * (mind - d) * (dir / d);
                if (!n1->locked)
                    n1->intertia -= f;
                if (!n2->locked)
                    n2->intertia += f;
            }
        }
    }
}

void SpringSystem::applyIntertia(Float delta, Float damp)
{
    for (auto & i : nodes_)
    {
        Node * n = i.get();
        if (n->locked)
            continue;

        n->pos += delta * n->intertia;

        n->intertia -= delta * damp * n->intertia;
    }
}

