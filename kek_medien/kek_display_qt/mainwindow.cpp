#include <cassert>

#include <QLayout>
#include <QThread>
#include <QTreeView>
#include <QSortFilterProxyModel>
#include <QMenu>
#include <QMenuBar>
#include <QGraphicsItem>
#include <QDockWidget>
#include <QDebug>
#include <QSettings>

#include "mainwindow.h"
#include "kekdata.h"
#include "kekmodel.h"
#include "kekview.h"
#include "companyview.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow   (parent)
    , model_        (0)
    , fmodel_       (0)
    , settings_     (new QSettings(QSettings::IniFormat, QSettings::UserScope, "modular-audio-graphics", "kek-viewer", this))
{
    setMinimumSize(512, 512);

    createWidgets_();

    if (settings_->contains("window-geom"))
        restoreGeometry( settings_->value("window-geom").toByteArray() );
    if (settings_->contains("window-state"))
        restoreState( settings_->value("window-state").toByteArray() );

    load();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent * e)
{
    settings_->setValue("window-geom", saveGeometry());
    settings_->setValue("window-state", saveState());
    QMainWindow::closeEvent(e);
}

QDockWidget * MainWindow::createDock_(QWidget *w, const QString& title, Qt::DockWidgetArea area)
{
    assert(!w->objectName().isEmpty());

    QDockWidget * dock = new QDockWidget(title, centralWidget());
    dock->setObjectName("_dock_" + w->objectName());
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setWidget(w);
    addDockWidget(area, dock);
    return dock;
}


void MainWindow::createWidgets_()
{

    list_ = new QTreeView(this);
    list_->setObjectName("CompanyList");
    list_->setSortingEnabled(true);
    connect(list_, SIGNAL(clicked(QModelIndex)),
            this, SLOT(onCompanySelected_(QModelIndex)));
    createDock_(list_, tr("company list"));

    compView_ = new CompanyView(this);
    compView_->setObjectName("CompanyView");
    createDock_(compView_, tr("company view"));

    kekView_ = new KekView(this);
    kekView_->setObjectName("MapView");
    createDock_(kekView_, tr("media map"), Qt::RightDockWidgetArea);
    createDock_(kekView_->controlWidget(), tr("map controls"), Qt::RightDockWidgetArea);
    connect(kekView_, SIGNAL(companyClicked(KekData::Company*)),
            this, SLOT(onCompanyClicked_(KekData::Company*)));

    // XXX


    QMenu * main = new QMenu(this);
/*    auto a = main->addAction(tr("start"));
    a->setShortcut(Qt::Key_F7);
    connect(a, SIGNAL(triggered()), this, SLOT(start()));
    a = main->addAction(tr("stop"));
    a->setShortcut(Qt::Key_F8);
    connect(a, SIGNAL(triggered()), this, SLOT(stop()));
    */
    auto a = main->addAction(tr("load"));
    a->setShortcut(Qt::CTRL + Qt::Key_L);
    connect(a, SIGNAL(triggered()), this, SLOT(load()));
    a = main->addAction(tr("save"));
    a->setShortcut(Qt::CTRL + Qt::Key_S);
    connect(a, SIGNAL(triggered()), this, SLOT(save()));

    menuBar()->addMenu(main);
}


void MainWindow::load()
{
    kekView_->stop();

    //kek_.loadXml("../kek_data.xml");
    kek_.loadXml("../kek_edit.xml");

    // update map
    kekView_->setKekData(&kek_);

    // update models
    delete fmodel_;
    delete model_;
    model_ = new KekModel(&kek_);
    fmodel_ = new QSortFilterProxyModel;
    fmodel_->setSourceModel(model_);
    list_->setModel(fmodel_);
}

void MainWindow::save()
{
    kek_.saveXml("../kek_edit.xml");
}


void MainWindow::onCompanyClicked_(KekData::Company * c)
{
    compView_->setCompany(c);

    int idx = kek_.index(c);
    list_->setCurrentIndex(fmodel_->mapFromSource(model_->index(idx,0)));
}

void MainWindow::onCompanySelected_(const QModelIndex & idx)
{
    int row = fmodel_->mapToSource(idx).row();
    if (row < 0 || row >= (int)kek_.companies().size())
        return;
    auto c = kek_.companies()[row];

    compView_->setCompany(c);
    kekView_->focusOnCompany(c);
}
