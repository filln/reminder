#ifndef LISTHISTWINDOW_H
#define LISTHISTWINDOW_H
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
class ListHistWindow;
}

class ListHistWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ListHistWindow(QWidget *parent = 0);

    ~ListHistWindow();

private:
    friend class ChangeWindow;
    Ui::ListHistWindow *ui;
    int rc, cc, rowcl, colcl; //количество строк rc и столбцов cc, кликнутая строка rowcl, столбец colcl
    QFile fileHist;
    QVector<int> OpenRow;
    QVector<ChangeWindow*> VptrCW; //указатели на текущие объекты окон редактирования
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void getMW(); //берем *mw
    void showList(); //показывает окно истории напоминаний
    void changCellKey(); //текущая ячейка (смена ячеек с клавиатуры)
    void changCellClick(int row, int column); //текущая ячейка (смена ячеек кликом)
    void restore(); //восстановить нап.
    void del(); //удалить нап. навсегда
    void clock(QString& clText);
    void openChange_Sl(); //открыть окно "подробнее/изменить"
signals:
    void addData();
    void sendLHW();
    void openChange(); //открыть окно "подробнее/изменить"
    void closeAllOther(); //закрыть другие окна

};

#endif // LISTHISTWINDOW_H
