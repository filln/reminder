#ifndef SHORTCUTREM_H
#define SHORTCUTREM_H
#include "mainwindow.h"
#include <QShortcut>

class shortCutRem : public MainWindow
{
    Q_OBJECT
private:
//главное окно
QShortcut *MWkeyCtrlD; //добавить напоминание
QShortcut *MWkeyCtrlL; //список нап.
QShortcut *MWkeyCtrlH; //история нап.
QShortcut *MWkeyCtrlQ; //свернуть в трей (скрыть)
QShortcut *MWkeyEsc; //закрыть окно
//список напоминаний
QShortcut *LWkeyEsc; //закрыть окно
QShortcut *LWkeyDel; //удалить нап. из списка
//история напоминаний
QShortcut *LHWkeyEsc; //закрыть окно
QShortcut *LHWkeyDel; //удалить нап. из истории
QShortcut *LHWkeyCtrlR; //восстановить нап. из истории
////окно звонка
//QShortcut *CRkeyEsc; //закрыть окно

public:
    shortCutRem();
    void createShortCut();
    ~shortCutRem();

signals:

private slots:
    void getMW(); //берем *mw
    void getLW(); //берем *lw
    void getLHW(); //берем *lhw
//void getCR();
};

#endif // SHORTCUTREM_H
