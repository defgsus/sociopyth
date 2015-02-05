#include <QLayout>
#include <QThread>

#include "mainwindow.h"
#include "kekdata.h"





MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setMinimumSize(512, 512);

    createWidgets_();


    sys_ = new SpringSystem();
    createSys_();

    timer_ = new  QTimer(this);
    timer_->setSingleShot(false);
    timer_->setInterval(1000 / 10);
    connect(timer_, SIGNAL(timeout()), this, SLOT(updateView()));

    start();

}

MainWindow::~MainWindow()
{
    stop();
    delete sys_;
}



void MainWindow::createWidgets_()
{
    auto w = new QWidget(this);
    setCentralWidget(w);

    auto lv = new QVBoxLayout(w);

        view_ = new QGraphicsView(w);
        lv->addWidget(view_);


    scene_ = new KekScene(w);
    view_->setScene(scene_);
    view_->setBackgroundBrush(QBrush(Qt::black));
}



void MainWindow::createSys_()
{
    sys_->clear();

#if 0
    std::vector<SpringSystem::Node*> nodes;
    for (int i=0; i<100; ++i)
    {
        nodes.push_back(
                    sys_->createNode()
                    );
    }

    for (int i=0; i<10; ++i)
    {
        int n1 = rand() % nodes.size(),
            n2 = rand() % nodes.size();
        if (n1 != n2)
            sys_->connect(nodes[n1], nodes[n2], float(rand())/RAND_MAX * 30.);
    }
#else
    kek_.loadXml("../kek_owner.xml");
    kek_.getSpringSystem(sys_);
#endif

    scene_->setSpringSystem(sys_);

    QTransform t;
    t.scale(13., 13.);
    //view_->setTransform(t);
}

void MainWindow::start()
{
    sys_->startThread();
    timer_->start();
}

void MainWindow::stop()
{
    sys_->stopThread();
    updateView();
}

void MainWindow::updateView()
{
    scene_->updatePositions();
    QTransform t;
    t.scale(13., 13.);
    view_->setTransform(t);
}
