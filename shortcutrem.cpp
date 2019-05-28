#include "shortcutrem.h"
#include "listwindow.h"
#include "listhistwindow.h"
//#include "callrem.h"

MainWindow *mwSC;
ListWindow *lwSC;
ListHistWindow *lhwSC;
//CallRem *crSC;

shortCutRem::shortCutRem(){}

void shortCutRem::getMW(){
    mwSC = qobject_cast<MainWindow*>(QObject::sender());
}
void shortCutRem::getLW(){
    lwSC = qobject_cast<ListWindow*>(QObject::sender());
}
void shortCutRem::getLHW(){
    lhwSC = qobject_cast<ListHistWindow*>(QObject::sender());
}
//void shortCutRem::getCR(){
//    crSC = qobject_cast<CallRem*>(QObject::sender());
//}
void shortCutRem::createShortCut(){
    //чтобы клавиши работали, родитель для QShortcut должен быть видим и включен. Для каждого окна свой объект QShortcut
    MWkeyCtrlD = new QShortcut(mwSC);
    MWkeyCtrlL = new QShortcut(mwSC);
    MWkeyCtrlH = new QShortcut(mwSC);
    MWkeyCtrlQ = new QShortcut(mwSC);
    MWkeyEsc = new QShortcut(mwSC);
    MWkeyCtrlD->setKey(Qt::CTRL + Qt::Key_D);
    MWkeyCtrlL->setKey(Qt::CTRL + Qt::Key_L);
    MWkeyCtrlH->setKey(Qt::CTRL + Qt::Key_H);
    MWkeyCtrlQ->setKey(Qt::CTRL + Qt::Key_Q);
    MWkeyEsc->setKey(Qt::Key_Escape);
    connect(MWkeyCtrlD, SIGNAL(activated()), mwSC, SLOT(addData()));
    connect(MWkeyCtrlL, SIGNAL(activated()), mwSC, SLOT(showList_Sl()));
    connect(MWkeyCtrlH, SIGNAL(activated()), mwSC, SLOT(showHistList_Sl()));
    connect(MWkeyCtrlQ, SIGNAL(activated()), mwSC, SLOT(hide()));
    connect(MWkeyEsc, SIGNAL(activated()), mwSC, SLOT(close()));

    LWkeyEsc = new QShortcut(lwSC);
    LWkeyDel = new QShortcut(lwSC);
    LWkeyEsc->setKey(Qt::Key_Escape);
    LWkeyDel->setKey(Qt::Key_Delete);
    connect(LWkeyEsc, SIGNAL(activated()), lwSC, SLOT(close()));
    connect(LWkeyDel, SIGNAL(activated()), lwSC, SLOT(del()));

    LHWkeyEsc = new QShortcut(lhwSC);
    LHWkeyDel = new QShortcut(lhwSC);
    LHWkeyCtrlR  = new QShortcut(lhwSC);
    LHWkeyEsc->setKey(Qt::Key_Escape);
    LHWkeyDel->setKey(Qt::Key_Delete);
    LHWkeyCtrlR->setKey(Qt::CTRL + Qt::Key_R);
    connect(LHWkeyEsc, SIGNAL(activated()), lhwSC, SLOT(close()));
    connect(LHWkeyDel, SIGNAL(activated()), lhwSC, SLOT(del()));
    connect(LHWkeyCtrlR, SIGNAL(activated()), lhwSC, SLOT(restore()));

}

shortCutRem::~shortCutRem(){}
