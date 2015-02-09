#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>

#include "kekscene.h"
#include "kekdata.h"
#include "springsystem.h"

class QTreeView;
class QScrollBar;
class QSortFilterProxyModel;
class KekModel;
class CompanyView;
class KekView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

    void load();
    void save();

private slots:

    void onCompanyClicked_(KekData::Company*);
    void onCompanySelected_(const QModelIndex&);
private:

    void createWidgets_();
    QDockWidget * createDock_(QWidget * w, const QString& title, Qt::DockWidgetArea area = Qt::LeftDockWidgetArea);
    //void createSys_();

    KekData kek_;
    KekModel * model_;
    QSortFilterProxyModel * fmodel_;

    CompanyView * compView_;
    QTreeView * list_;
    KekView * kekView_;
};

#endif // MAINWINDOW_H
