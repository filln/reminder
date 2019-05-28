#include "trayrem.h"
#include <QApplication>
#include <QStyle>

TrayRem::TrayRem(MainWindow *mwTR)
{
    trI = new QSystemTrayIcon(mwTR);
    trM = new QMenu(mwTR);

    trAMShow = new QAction("Добавить напоминание", mwTR);
    trAMList = new QAction("Список напоминаний", mwTR);
    trAMListHist = new QAction("История напоминаний", mwTR);
    trAMExit = new QAction("Выход", mwTR);

//    trI->setIcon(mwTR->style()->standardIcon(QStyle::SP_ComputerIcon));
    trI->setIcon(QIcon(":/images/reminder.ico"));
    trI->setToolTip(QApplication::applicationName());
    trM->addAction(trAMShow);
    trM->addAction(trAMList);
    trM->addAction(trAMListHist);
    trM->addSeparator();
    trM->addAction(trAMExit);
    trI->setContextMenu(trM);

    trI->show();
    connect(trI, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), mwTR, SLOT(icnAct(QSystemTrayIcon::ActivationReason)));
    connect(trAMShow, SIGNAL(triggered(bool)), mwTR, SLOT(show()));
    connect(trAMList, SIGNAL(triggered(bool)), mwTR, SLOT(showList_Sl()));
    connect(trAMListHist, SIGNAL(triggered(bool)), mwTR, SLOT(showHistList_Sl()));
    connect(trAMExit, SIGNAL(triggered(bool)), mwTR, SLOT(close()));

}



//TrayRem::~TrayRem(){
//    delete trI;
//    delete trM; //меню иконки
//   //кнопки меню
//    delete trAMList;
//    delete trAMListHist;
//    delete trAMShow;
//    delete trAMExit;

//}
