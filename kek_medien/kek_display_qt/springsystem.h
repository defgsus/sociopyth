#ifndef SPRINGSYSTEM_H
#define SPRINGSYSTEM_H

#include <memory>
#include <vector>
#include <QVector2D>

class SpringSystem
{
public:

    typedef float Float;
    typedef QVector2D vec2;

    struct Node
    {
        vec2 pos, intertia;
        bool locked;
        void * user;
    };

    struct Spring
    {
        Node * n1, * n2;
        Float dist, rest_dist;
        void * user;
    };

    // ------------- ctor ------------

    SpringSystem();
    ~SpringSystem();

    // ---------- setup --------------

    void clear();

    Node * createNode(void * usr = 0);
    Spring * connect(Node *, Node *, Float rest_distance, void * usr = 0);

    // --------- access --------------

    const std::vector<std::shared_ptr<Node>> & nodes() const { return nodes_; }
    const std::vector<std::shared_ptr<Spring>> & springs() const { return springs_; }

    // --------- physics -------------

    void step(Float delta);

    void relaxSprings(Float delta);
    void relaxDistance(Float delta, Float min_dist);
    void applyIntertia(Float delta, Float damp);

    // -------- threading ------------

    void startThread();
    /** Blocking until full stop */
    void stopThread();

private:

    class Thread;
    Thread * thread_;

    std::vector<std::shared_ptr<Node>> nodes_;
    std::vector<std::shared_ptr<Spring>> springs_;
};

#endif // SPRINGSYSTEM_H
