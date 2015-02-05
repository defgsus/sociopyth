#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QTimer>

#include "kekscene.h"
#include "kekdata.h"

class QTreeView;
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

private:

    void createWidgets_();
    void createSys_();

    KekData kek_;
    KekModel * model_;
    SpringSystem * sys_;
    KekScene * scene_;
    QGraphicsView * view_;
    QTreeView * list_;
    QTimer * timer_;
};

#endif // MAINWINDOW_H
