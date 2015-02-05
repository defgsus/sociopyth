#include <random>
#include "springsystem.h"

SpringSystem::SpringSystem()
{

}

SpringSystem::~SpringSystem()
{

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

    springs_.push_back(std::shared_ptr<Spring>(s));
    return s;
}





void SpringSystem::step(Float delta)
{
    relax(delta);
    applyIntertia(delta, 0.5);
}

void SpringSystem::relax(Float delta)
{
    for (auto & i : springs_)
    {
        Spring * s = i.get();

        // messure direction/distance
        vec2 dir = s->n2->pos - s->n1->pos;
        s->dist = dir.length();

        // spring force
        vec2 f = delta * (s->rest_dist - s->dist) * (dir / s->dist);
        s->n1->intertia -= f;
        s->n2->intertia += f;
    }
}

void SpringSystem::applyIntertia(Float delta, Float damp)
{
    for (auto & i : nodes_)
    {
        Node * n = i.get();

        n->pos += delta * n->intertia;

        n->intertia -= delta * damp * n->intertia;
    }
}
