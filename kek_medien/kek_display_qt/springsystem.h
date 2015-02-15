#ifndef SPRINGSYSTEM_H
#define SPRINGSYSTEM_H

#include <memory>
#include <vector>

#include <QVector2D>
#include <QColor>


/** A very generic spring system.
    Not per-se related to the KEK data. */
class SpringSystem
{
public:

    typedef float Float;
    typedef QVector2D vec2;

    struct Node
    {
        vec2 pos, intertia;
        Float min_dist;
        bool locked;
        void * user;

        QColor color;
    };

    struct Spring
    {
        Node * n1, * n2;
        Float dist, rest_dist, stiff;
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

    // -------- properties -----------

    void setDelta(Float s) { delta_ = s; }
    // these are all multipliers
    void setStiffness(Float s) { stiffness_ = s; }
    void setRadiusStiffness(Float s) { rstiffness_ = s; }
    void setMinimumRadius(Float r) { minRadius_ = r; }
    void setRestDistance(Float d) { restDistance_ = d; }
    void setInertance(Float d) { inertance_ = d; }

    // debug
    QString toString() const;

    // --------- physics -------------

    void step() { step(delta_); }
    void step(Float delta);

    void relaxSprings(Float delta);
    void relaxDistance(Float delta);
    void applyIntertia(Float delta, Float damp);

    // -------- threading ------------

    bool isRunning() const;

    void startThread();
    /** Blocking until full stop */
    void stopThread();

private:

    class Thread;
    Thread * thread_;

    std::vector<std::shared_ptr<Node>> nodes_;
    std::vector<std::shared_ptr<Spring>> springs_;

    int frame_;
    Float delta_,
        stiffness_,
        rstiffness_,
        inertance_,
        minRadius_,
        restDistance_;
};

#endif // SPRINGSYSTEM_H
