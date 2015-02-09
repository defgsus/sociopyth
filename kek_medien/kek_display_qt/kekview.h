/** @file kekview.h

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 09.02.2015</p>
*/

#ifndef KEKVIEW_H
#define KEKVIEW_H

#include <QWidget>

#include "springsystem.h"
#include "kekdata.h"

class QGraphicsView;
class QDoubleSpinBox;
class QVBoxLayout;
class KekScene;

/** Wrapper around QGraphicsView with some controls */
class KekView : public QWidget
{
    Q_OBJECT
public:
    explicit KekView(QWidget *parent = 0);
    ~KekView();

    KekData * kekData() const { return kek_; }
    KekScene * kekScene() const { return scene_; }
    SpringSystem * springSystem() const { return sys_; }
    QGraphicsView * graphicsView() const { return view_; }

    QWidget * controlWidget() const { return controlWidget_; }

signals:

    /** A company was clicked in the map */
    void companyClicked(KekData::Company *);

    /** A title was clicked in the map */
    void titleClicked(KekData::Title *);

public slots:

    void setKekData(KekData *);

    /** Updates all the graphics items */
    void updateView();
    void updateViewIfNotRunning();

    void focusOnCompany(KekData::Company *);

    void start();
    void stop();

private slots:

    void onNodeSelected_(SpringSystem::Node *);
    void onValueChanged_();

private:

    QDoubleSpinBox * createDoubleSpinBox_(const QString& name, double value, double rangeMin, double rangeMax);

    KekData * kek_;
    SpringSystem * sys_;
    KekScene * scene_;
    QGraphicsView * view_;
    QWidget * controlWidget_;
    QVBoxLayout * controlLayout_;

    QTimer * timer_;
    QDoubleSpinBox * sbScale_, *sbStiff_, *sbDelta_;
};

#endif // KEKVIEW_H
