#include <QLayout>
#include <QThread>

#include "mainwindow.h"


class Thread : public QThread
{
public:
    Thread(SpringSystem * s, QObject * p) : QThread(p), sys(s), stop(false) { }

    void run() Q_DECL_OVERRIDE
    {
        stop = false;
        while (!stop)
        {
            sys->step(0.0001);
        }
    }

    SpringSystem * sys;
    volatile bool stop;
};




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setMinimumSize(512, 512);

    createWidgets_();


    sys_ = new SpringSystem();
    createSys_();

    thread_ = new Thread(sys_, this);
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

    std::vector<SpringSystem::Node*> nodes;

    for (int i=0; i<100; ++i)
    {
        nodes.push_back(
                    sys_->createNode()
                    );
    }

    for (int i=0; i<100; ++i)
    {
        int n1 = rand() % nodes.size(),
            n2 = rand() % nodes.size();
        sys_->connect(nodes[n1], nodes[n2], 5.);
    }

    scene_->setSpringSystem(sys_);

    QTransform t;
    t.scale(13., 13.);
    //view_->setTransform(t);
}

void MainWindow::start()
{
    thread_->start();
    timer_->start();
}

void MainWindow::stop()
{
    timer_->stop();
    thread_->stop = true;
    thread_->wait();
    updateView();
}

void MainWindow::updateView()
{
    scene_->updatePositions();
    QTransform t;
    t.scale(13., 13.);
    view_->setTransform(t);
}
