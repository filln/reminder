#ifndef LISTWINDOW_H
#define LISTWINDOW_H
#include <QObject>
#include <QtDebug>
#include <QMainWindow>
#include <QTableWidget>
#include <QFile>
#include <QDir>
#include <QVector>
#include "changewindow.h"
#include <QCloseEvent>

namespace Ui {
class ListWindow;
}

class ListWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ListWindow(QWidget *parent = 0);

    ~ListWindow();

private:
    friend class CallRem;
    friend class ChangeWindow;
    Ui::ListWindow *ui;
    int rc, cc, rowcl, colcl; //количество строк rc и столбцов cc, кликнутая строка rowcl, столбец colcl
    QFile file;
    QFile fileHist;
    QVector<int> OpenRow; //номера строк, для которых открыто окно "подробнее/изменить"
    QVector<ChangeWindow*> VptrCW; //указатели на текущие объекты окон редактирования
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void getMW(); //берем *mw
    void showList(); //показывает окно со списком напоминаний
    void changCellKey(); //текущая ячейка (смена ячеек с клавиатуры)
    void changCellClick(int row, int column); //текущая ячейка (смена ячеек кликом)
    void del(); //удалить нап.
    void clock(QString& clText);
    void openChange_Sl(); //открыть окно "подробнее/изменить"
signals:
    void sendLW(); //отправить *lw
    void openChange(); //открыть окно "подробнее/изменить"
    void closeAllOther(); //закрыть другие окна

};

#endif // LISTWINDOW_H
