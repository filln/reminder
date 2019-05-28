#ifndef CALLREM_H
#define CALLREM_H
#include <QWidget>
#include <QCloseEvent>
#include <QSound>
#include <QMediaPlayer>
#include <QDir>
#include <QString>
#include <QStringList>

namespace Ui {
class CallRem;
}

class CallRem : public QWidget
{
    Q_OBJECT

public:
    explicit CallRem(QWidget *parent = 0);
    ~CallRem();

private:
    friend class MainWindow;
    Ui::CallRem *ui;
    CallRem *crCall;
    int cntCall; //кол. одновременных напоминаний. call() не сработает, пока не обработаются текущие cntCall напоминаний
    QStringList files;
    QStringList dirs;
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void getMW(); //берем *mw
    void getLW(); //берем *lw
    void call(); //показать окна со звонками по таймеру.
    void callRep(); //сделать напоминание из звонка (повторить позже через ... минут)
    void callRepIn(); //сделать напоминание из звонка (повторить позже по дате и времени)
    void callTime(const QString &stime); //вписать в timeEdit время
    void clock(QString& clText);
    void closeCall(); //закрыть окно
    void closeAllCall(); //закрыть все окна перед закрытием программы
    void openFiles(); //открыть файлы
    void openDirs(); //открыть каталоги
signals:
    void delCall(); //удалить напоминание из базы
    void addData(); //занести звонок в напоминание
    void sendCR();
    void autoOpenFiles(); //открыть файлы
    void autoOpenDirs(); //открыть каталоги
};

#endif // CALLREM_H
