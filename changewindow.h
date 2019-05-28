#ifndef CHANGEWINDOW_H
#define CHANGEWINDOW_H

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QDate>
#include <QTime>
#include <QFile>
#include <QCloseEvent>

namespace Ui {
class ChangeWindow;
}

class ChangeWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChangeWindow(QWidget *parent = 0);
    ~ChangeWindow();

private:
    Ui::ChangeWindow *ui;
    ChangeWindow *cwNew;
    int rowcl; //для сохранения номера редактируемой строки(индекса вектора данных)
    QString whoIs; //какое окно вызвало редактирование: ListWindow, ListHistWindow
    struct Tdata{
        QString text;
        QDate date;
        QTime time;
        QStringList files;
        QStringList dirs;
    }data;
    QFile file;
    QFile fileHist;
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void openChangeWindow(); //открыть окно "подробнее/изменить" из окна списка нап. История не редактируется (кроме даты и времени для корректного восстановления в список нап.).
    void saveChanges(); //сохранить изменения и закрыть окно
    void openFiles(); //открыть файлы
    void openDirs(); //открыть каталоги
    void addFiles(); //по кнопке выбрать файлы для напоминания
    void addDirs(); //по кнопке выбрать каталог для напоминания
    void cancelFiles(); //отменить выбор файлов
    void cancelDirs(); //отменить выбор каталогов
    void getMW(); //берем *mw
    void clock(QString& clText);
    void closeCW(); //закрыть окно (без сохранения)
    void closeAllCW(); //закрыть все окна перед закрытием программы
    void getLW(); //берем *lw
    void getLHW(); //берем *lhw

};

#endif // CHANGEWINDOW_H
