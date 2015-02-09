/** @file kekview.cpp

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 09.02.2015</p>
*/

#include <cassert>

#include <QGraphicsView>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QGraphicsItem>
#include <QTimer>
#ifdef QT_DEBUG
#include <QDebug>
#endif

#include "kekview.h"
#include "kekscene.h"


KekView::KekView(QWidget *parent)
    : QWidget       (parent)
    , kek_          (0)
    , sys_          (new SpringSystem())
    , scene_        (new KekScene(this))
{
    connect(scene_, SIGNAL(nodeSelected(SpringSystem::Node*)),
            this, SLOT(onNodeSelected_(SpringSystem::Node*)));
    connect(scene_, SIGNAL(nodeChanged(SpringSystem::Node*)),
            this, SLOT(updateViewIfNotRunning()));

    timer_ = new  QTimer(this);
    timer_->setSingleShot(false);
    timer_->setInterval(1000 / 2);
    connect(timer_, SIGNAL(timeout()), this, SLOT(updateView()));

    // ------- own widgets -------------------

    auto lv = new QVBoxLayout(this);

        view_ = new QGraphicsView(this);
        view_->setBackgroundBrush(QBrush(Qt::black));
        lv->addWidget(view_);

    // ------- controller widget -------------

    controlWidget_ = new QWidget(this);
    controlLayout_ = new QVBoxLayout(controlWidget_);
    controlWidget_->setObjectName("ControlWidget");

        auto cbRun = new QCheckBox(tr("run physics"), this);
        controlLayout_->addWidget(cbRun);
        connect(cbRun, &QCheckBox::stateChanged, [=]()
        {
            if (cbRun->isChecked())
                start();
            else
                stop();
        });

        sbScale_ = createDoubleSpinBox_(tr("view scale"), 1., 0.01, 100.);
        sbScale_->setSingleStep(.1);
        sbStiff_ = createDoubleSpinBox_(tr("stiffness"), 1., 0.0, 1000.);
        sbDistance_ = createDoubleSpinBox_(tr("distance"), 1., 0.001, 1000.);
        sbRadius_ = createDoubleSpinBox_(tr("radius"), 1., 0.001, 1000.);
        sbDelta_ = createDoubleSpinBox_(tr("physics speed"), 0.02, 0.00001, 1.);
        sbDelta_->setSingleStep(0.001);
}

QDoubleSpinBox * KekView::createDoubleSpinBox_(const QString &name, double value, double rangeMin, double rangeMax)
{
    auto lh = new QHBoxLayout;
    controlLayout_->addLayout(lh);

    auto label = new QLabel(name, controlWidget_);
    lh->addWidget(label);

    auto sb = new QDoubleSpinBox(controlWidget_);
    lh->addWidget(sb);
    sb->setRange(rangeMin, rangeMax);
    sb->setValue(value);
    sb->setDecimals(7);
    sb->setSingleStep(0.01);

    connect(sb, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged_()));

    return sb;
}

KekView::~KekView()
{
    stop();

    delete sys_;
}

void KekView::setKekData(KekData * kek)
{
    kek_ = kek;
    kek_->getSpringSystem(sys_);

#ifdef QT_DEBUG
    qDebug() << sys_->toString();
#endif

    scene_->setSpringSystem(sys_);
    view_->setScene(scene_);
    updateView();
}

void KekView::updateView()
{
    scene_->updatePositions();

    QTransform t;
    const qreal s = sbScale_->value();
    t.scale(s, s);
    view_->setTransform(t);
}

void KekView::updateViewIfNotRunning()
{
    if (!sys_->isRunning())
        updateView();
}

void KekView::focusOnCompany(KekData::Company * c)
{
    assert(kek_);

    auto n = kek_->nodeForCompany(c);
    QGraphicsItem * it = (QGraphicsItem*)n->user;
    auto r = it->boundingRect();
    r.moveTo(it->scenePos());
    view_->ensureVisible(r);
}

void KekView::start()
{
    sys_->startThread();
    timer_->start();
}

void KekView::stop()
{
    timer_->stop();
    sys_->stopThread();
    if (kek_)
        updateView();
}

void KekView::onValueChanged_()
{
    sys_->setDelta(sbDelta_->value());
    sys_->setStiffness(sbStiff_->value());
    sys_->setMinimumRadius(sbRadius_->value());
    sys_->setRestDistance(sbDistance_->value());

    updateViewIfNotRunning();
}

void KekView::onNodeSelected_(SpringSystem::Node * n)
{
    assert(kek_);

    if (auto c = kek_->companyForNode(n))
        emit companyClicked(c);
    // XXX title
}
