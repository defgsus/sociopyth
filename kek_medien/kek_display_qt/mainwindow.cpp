#include <QLayout>
#include <QThread>
#include <QTreeView>
#include <QSortFilterProxyModel>
#include <QMenu>
#include <QMenuBar>

#include "mainwindow.h"
#include "kekdata.h"
#include "kekmodel.h"




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow   (parent)
    , model_        (0)
    , scene_        (0)
{
    setMinimumSize(512, 512);

    createWidgets_();


    sys_ = new SpringSystem();
    createSys_();

    timer_ = new  QTimer(this);
    timer_->setSingleShot(false);
    timer_->setInterval(1000 / 4);
    connect(timer_, SIGNAL(timeout()), this, SLOT(updateView()));

//    start();

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

    auto lh = new QHBoxLayout(w);

        list_ = new QTreeView(w);
        lh->addWidget(list_, 1);
        list_->setSortingEnabled(true);


        view_ = new QGraphicsView(w);
        lh->addWidget(view_, 2);


    scene_ = new KekScene(w);
    view_->setScene(scene_);
    view_->setBackgroundBrush(QBrush(Qt::black));


    QMenu * main = new QMenu(this);
    menuBar()->addMenu(main);
    auto a = main->addAction(tr("start"));
    a->setShortcut(Qt::Key_F7);
    connect(a, SIGNAL(triggered()), this, SLOT(start()));
    a = main->addAction(tr("stop"));
    a->setShortcut(Qt::Key_F8);
    connect(a, SIGNAL(triggered()), this, SLOT(stop()));
    a = main->addAction(tr("load"));
    a->setShortcut(Qt::CTRL + Qt::Key_L);
    connect(a, SIGNAL(triggered()), this, SLOT(load()));
    a = main->addAction(tr("save"));
    a->setShortcut(Qt::CTRL + Qt::Key_S);
    connect(a, SIGNAL(triggered()), this, SLOT(save()));
}



void MainWindow::createSys_()
{
    kek_.clear();
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

    delete model_;
    model_ = new KekModel(&kek_);
    auto sfm = new QSortFilterProxyModel(this);
    sfm->setSourceModel(model_);
    list_->setModel(sfm);

    scene_->setSpringSystem(sys_);
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

void MainWindow::load()
{
    stop();
    createSys_();
}

void MainWindow::save()
{
    kek_.saveXml("../kek_edit.xml");
}

void MainWindow::updateView()
{
    scene_->updatePositions();
    QTransform t;
    t.scale(6., 6.);
    view_->setTransform(t);
}
