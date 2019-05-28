//Reminder Copyright (C) 2018г. Кучеров Анатолий Александрович

#include <QApplication>
#include <QTimer>
#include <QLockFile>
#include <QDir>
#include <QMessageBox>
#include <QStyleFactory>
#include <QFont>
#include "mainwindow.h"
#include "listwindow.h"
#include "listhistwindow.h"
#include "callrem.h"
#include "changewindow.h"
#include "trayrem.h"
#include "shortcutrem.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //не запускать копий приложения
    QLockFile lockFile(QDir::temp().absoluteFilePath("reminder.lock"));
    if(!lockFile.tryLock(99)){
        QMessageBox::warning(0,
                             QApplication::applicationName(),
                             "Программа уже запущена!",
                             QMessageBox::Ok);
        return 1;
    }

    QApplication::setQuitOnLastWindowClosed(false);

    QFont currFont("MS Shell Dlg 2", 10, true);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication::setFont(currFont);

    MainWindow* mw = new MainWindow;
    QTimer *tmrCall = new QTimer(mw);
    QTimer *tmrClock = new QTimer(mw);
    TrayRem *tr = new TrayRem(mw);
    ListWindow* lw = new ListWindow;
    ListHistWindow* lhw = new ListHistWindow;
    CallRem* cr = new CallRem;
    ChangeWindow *cw = new ChangeWindow;
    shortCutRem *sc = new shortCutRem;

    //делимся указателями
    QWidget::connect(mw, SIGNAL(sendMW()), lw, SLOT(getMW())); //отправить *mw к ListWindow
    QWidget::connect(mw, SIGNAL(sendMW()), lhw, SLOT(getMW())); //отправить *mw к ListHistWindow
    QWidget::connect(mw, SIGNAL(sendMW()), cr, SLOT(getMW())); //отправить *mw к CallRem
    QWidget::connect(mw, SIGNAL(sendMW()), sc, SLOT(getMW())); //отправить *mw к shortCutRem
    QWidget::connect(mw, SIGNAL(sendMW()), cw, SLOT(getMW())); //отправить *mw к ChangeWindow
    mw->sendMW();
    QWidget::connect(lw, SIGNAL(sendLW()), cr, SLOT(getLW())); //отправить *lw к CallRem
    QWidget::connect(lw, SIGNAL(sendLW()), sc, SLOT(getLW())); //отправить *lw к shortCutRem
    QWidget::connect(lw, SIGNAL(sendLW()), cw, SLOT(getLW())); //отправить *lw к ChangeWindow
//    QWidget::connect(lw, SIGNAL(sendLW()), mw, SLOT(getLW())); //отправить *lw к MainWindow
    lw->sendLW();
    QWidget::connect(lhw, SIGNAL(sendLHW()), sc, SLOT(getLHW())); //отправить *lhw к shortCutRem
    QWidget::connect(lhw, SIGNAL(sendLHW()), cw, SLOT(getLHW())); //отправить *lhw к ChangeWindow
    lhw->sendLHW();
//    QWidget::connect(lhw, SIGNAL(sendLHW()), mw, SLOT(getLHW())); //отправить *lhw к MainWindow
//    hw->sendLHW();
//    QWidget::connect(cr, SIGNAL(sendCR()), sc, SLOT(getCR())); //отправить *cr к shortCutRem
//    cr->sendCR();
//    QWidget::connect(cr, SIGNAL(sendCR()), mw, SLOT(getCR())); //отправить *cr к MainWindow
//    cr->sendCR();

    QWidget::connect(mw, SIGNAL(showList()), lw, SLOT(showList())); //открыть окно со списком напоминаний по кнопке pbList
    QWidget::connect(mw, SIGNAL(showHistList()), lhw, SLOT(showList())); //открыть окно истории напоминаний по кнопке pbHistList
    QWidget::connect(mw, SIGNAL(AddedData()), lw, SLOT(showList())); //открыть окно со списком напоминаний по кнопке pbAdd
    QWidget::connect(cr, SIGNAL(delCall()), lw, SLOT(del())); //удалить прозвонившее напоминание
    QWidget::connect(cr, SIGNAL(addData()), mw, SLOT(addData())); //добавить напоминание из звонка
    QWidget::connect(lhw, SIGNAL(addData()), mw, SLOT(addData())); //добавить напоминание из истории
    QWidget::connect(lw, SIGNAL(openChange()), cw, SLOT(openChangeWindow())); //открыть окно "подробнее/изменить"
    QWidget::connect(lhw, SIGNAL(openChange()), cw, SLOT(openChangeWindow())); //открыть окно "подробнее/изменить"

    //показать часы в других окнах
    QWidget::connect(mw, SIGNAL(clockOther(QString&)), lw, SLOT(clock(QString&)));
    QWidget::connect(mw, SIGNAL(clockOther(QString&)), lhw, SLOT(clock(QString&)));
    QWidget::connect(mw, SIGNAL(clockOther(QString&)), cr, SLOT(clock(QString&)));
    QWidget::connect(mw, SIGNAL(clockOther(QString&)), cw, SLOT(clock(QString&)));

    QWidget::connect(tmrClock, SIGNAL(timeout()), mw, SLOT(clock()));
    QWidget::connect(tmrCall, SIGNAL(timeout()), cr, SLOT(call()));
    tmrCall->start(10500); //таймеры не должны срабатывать одновременно
    tmrClock->start(1000);

    QWidget::connect(mw, SIGNAL(closeAllOther()), lw, SLOT(close()));
    QWidget::connect(mw, SIGNAL(closeAllOther()), lhw, SLOT(close()));
    QWidget::connect(mw, SIGNAL(closeAllOther()), cr, SLOT(closeAllCall())); //закрыть все окна звонков.
    QWidget::connect(lw, SIGNAL(closeAllOther()), cw, SLOT(closeAllCW())); //закрыть все окна просмотра/редактирования
    QWidget::connect(lhw, SIGNAL(closeAllOther()), cw, SLOT(closeAllCW())); //закрыть все окна просмотра/редактирования

    sc->createShortCut();

    mw->show();

    return a.exec();
}
