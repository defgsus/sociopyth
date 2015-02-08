#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QTimer>

#include "kekscene.h"
#include "kekdata.h"
#include "springsystem.h"

class QTreeView;
class QScrollBar;
class QSortFilterProxyModel;
class KekModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

    void start();
    void stop();
    void load();
    void save();

    void updateView();

private slots:

    void onNodeSelected_(SpringSystem::Node *);
    void onCompanySelected_(const QModelIndex&);
private:

    void createWidgets_();
    void createSys_();

    KekData kek_;
    KekModel * model_;
    QSortFilterProxyModel * fmodel_;
    SpringSystem * sys_;
    KekScene * scene_;
    QGraphicsView * view_;
    QTreeView * list_;
    QTimer * timer_;
    QScrollBar * sbScale_;
};

#endif // MAINWINDOW_H
