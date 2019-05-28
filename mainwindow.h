#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QDataStream>
#include <QDate>
#include <QTime>
#include <QVector>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QSettings>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QSignalMapper>
#include "callrem.h"
//#include "changewindow.h"
//#include <QHideEvent>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    friend class ListWindow;
    friend class ListHistWindow;
    friend class CallRem;
    friend class ChangeWindow;

    Ui::MainWindow *ui;
    struct Tdata{
        QString text;
        QDate date;
        QTime time;
        QStringList files;
        QStringList dirs;
    }data;
    //перед запуском файлы должны быть созданы
    QFile file; //текущие нап.
    QFile fileHist; //история нап. (удаленные из Vdata и fileHist)

    QVector<Tdata> Vdata; //текущие нап.
    QVector<Tdata> VdataHist; //история нап. (удаленные из Vdata и fileHist)
    QVector<CallRem*> VptrCR; //указатели на текущие объекты нап.
    QSettings *settings; //настройки "\\settings.ini"
    bool sl; //автозагрузка
    bool noSound; //проигрывать ли звуки
    bool isMySound; //выбран ли пользовательскй звук
    bool isAutoOpenFiles; //открывать ли файлы автоматически при срабатывании нап.
    bool isAutoOpenDirs; //открывать ли каталоги автоматически при срабатывании нап.
    QString currSound; //имя текущего файла звука(из стандартных) или полный путь к пользовательскому звуку
    QSignalMapper *mapperActSound;
protected:
    void closeEvent(QCloseEvent *event);
public:
    explicit MainWindow(QWidget *parent = 0);
//    Ui::MainWindow* getUi() const {
//        return ui;
//    }
    friend QDataStream& operator <<(QDataStream& qos, const MainWindow::Tdata& data);
    friend QDataStream& operator >>(QDataStream& qis, MainWindow::Tdata& data);

    ~MainWindow();
signals:
    void sendMW(); //отправить *mw
    void AddedData(); //добавлены напоминания
    void callRem();

    void showList(); //показать окно со списком напоминаний
    void showHistList(); //показать окно истории напоминаний

    void clockOther(QString& clText); //показать часы в других окнах
    void closeAllOther(); //закрыть другие окна
private slots:
    void addData(); //добавить напоминания

    void showList_Sl(); //отправить showList()
    void showHistList_Sl(); //отправить showHistList()

    void clock(); //часы
    void icnAct(QSystemTrayIcon::ActivationReason r); //скрыть или показать окно по клику на иконке в трее
    void setAutoLoad(bool isCh); //автозагрузка

    void aboutRem(); //окно с информацией о программе
    void readme(); //открыть файл Readme.txt
    void donate(); //окно с информацией о донате

    void isSound(bool noSound); //не проигрывать звуки
    void currSoundSl(const QString& currSound); //какой звук проиграть

    void addFiles(); //по кнопке выбрать файлы для напоминания
    void addDirs(); //по кнопке выбрать каталог для напоминания
    void cancelFiles(); //отменить выбор файлов
    void cancelDirs(); //отменить выбор каталогов
    void autoOpenFiles(bool isAof); //открывать ли файлы автоматически при срабатывании нап.
    void autoOpenDirs(bool isAod); //открывать ли каталоги автоматически при срабатывании нап.
//    void getLW(); //взять *lw
//    void getLHW(); //взять *lhw
//    void getCR(); //взять *cr
};

#endif // MAINWINDOW_H
