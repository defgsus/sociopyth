/** @file mainwindow.cpp

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 15.02.2015</p>
*/

#include <cassert>

#include <QTreeView>
#include <QDockWidget>
#include <QMessageBox>
#include <QLineEdit>

#include "mainwindow.h"
#include "biodata.h"
#include "biolistmodel.h"
#include "filtermodel.h"
#include "detailview.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow       (parent)
    , data_             (new BioData(this))
    , model_            (0)
    , fmodel_           (0)

{
    setMinimumSize(512, 512);

    createWidgets_();

    load();
}

MainWindow::~MainWindow()
{

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
    auto edit = new QLineEdit(this);
    edit->setObjectName("NameFilterInput");
    createDock_(edit, tr("name filter"));
    connect(edit, SIGNAL(textChanged(QString)),
            this, SLOT(onListFilterChanged_(QString)));

    edit = new QLineEdit(this);
    edit->setObjectName("TextFilterInput");
    createDock_(edit, tr("fulltext filter"));
    connect(edit, SIGNAL(textChanged(QString)),
            this, SLOT(onTextFilterChanged_(QString)));

    list_ = new QTreeView(this);
    list_->setObjectName("PersonList");
    list_->setSortingEnabled(true);
    connect(list_, SIGNAL(clicked(QModelIndex)),
            this, SLOT(onListClicked_(QModelIndex)));
    createDock_(list_, tr("Personen"));

    detail_ = new DetailView(this);
    detail_->setObjectName("DetailView");
    createDock_(detail_, tr("Details"), Qt::RightDockWidgetArea);
}


void MainWindow::load()
{
    data_->loadXml("../../parlament_bio18.xml");
    data_->loadXml("../../../landtag/landtag_th.xml");
        //QMessageBox::critical(this, tr("error"), tr("Could not load xml file"));

    // update models
    delete fmodel_;
    delete model_;
    model_ = new BioListModel(data_);
    fmodel_ = new FilterModel;
    fmodel_->setFilterCaseSensitivity(Qt::CaseInsensitive);
    fmodel_->setSourceModel(model_);
    fmodel_->setBioData(data_);
    list_->setModel(fmodel_);
}

void MainWindow::onListClicked_(const QModelIndex & idx)
{
    int row = fmodel_->mapToSource(idx).row();
    if (row < 0 || row >= (int)data_->numPersons())
        return;

    auto p = data_->person(row);

    detail_->setPerson(&p);
}

void MainWindow::onListFilterChanged_(const QString & s)
{
    fmodel_->setFilterFixedString(s);
}

void MainWindow::onTextFilterChanged_(const QString & s)
{
    fmodel_->setFulltextFilter(s);
    detail_->setTextFilter(s);
}
