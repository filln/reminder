#ifndef TRAYREM_H
#define TRAYREM_H

#include <mainwindow.h>
#include <QWidget>
#include <QAction>
#include <QMenu>

class TrayRem : public MainWindow
{
    Q_OBJECT
private:
    QSystemTrayIcon *trI; // иконка в трее
    QMenu *trM; //меню иконки
   //кнопки меню
    QAction *trAMList;
    QAction *trAMListHist;
    QAction *trAMShow;
    QAction *trAMExit;

public:
    TrayRem(MainWindow *mwTR);
//    ~TrayRem();

signals:

private slots:

};

#endif // TRAYREM_H
