/** @file mainwindow.h

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 15.02.2015</p>
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTreeView;
class BioData;
class BioListModel;
class FilterModel;
class DetailView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

    void load();

private slots:

    void onListClicked_(const QModelIndex&);
    void onListFilterChanged_(const QString&);
    void onTextFilterChanged_(const QString&);

private:

    void createWidgets_();
    QDockWidget * createDock_(QWidget * w, const QString& title, Qt::DockWidgetArea area = Qt::LeftDockWidgetArea);

    BioData * data_;

    BioListModel * model_;
    FilterModel * fmodel_;

    QTreeView * list_;
    DetailView * detail_;
};

#endif // MAINWINDOW_H
