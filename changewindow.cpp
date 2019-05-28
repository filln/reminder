#include "changewindow.h"
#include "ui_changewindow.h"
#include "mainwindow.h"
#include "listwindow.h"
#include "ui_listwindow.h"
#include "listhistwindow.h"
#include "ui_listhistwindow.h"
#include <QShortcut>
#include <QDir>
#include <QDesktopServices>
#include <QApplication>
#include <QUrl>
#include <QFileDialog>
#include <QtDebug>

MainWindow *mwCW;
ListWindow *lwCW;
ListHistWindow *lhwCW;

QDataStream& operator <<(QDataStream& qos, const MainWindow::Tdata& data);

ChangeWindow::ChangeWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChangeWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->timeEdit->setTime(QTime::currentTime());

    QShortcut *CWkeyEsc = new QShortcut(this);
    CWkeyEsc->setKey(Qt::Key_Escape);
    connect(CWkeyEsc, SIGNAL(activated()), this, SLOT(close()));

    connect(ui->pBSave, SIGNAL(clicked(bool)), this, SLOT(saveChanges()));
    connect(ui->pBClose, SIGNAL(clicked(bool)), this, SLOT(closeCW()));
    connect(ui->pBOpenFiles, SIGNAL(clicked(bool)), this, SLOT(openFiles()));
    connect(ui->pBOpenDirs, SIGNAL(clicked(bool)), this, SLOT(openDirs()));
    connect(ui->pBAddFiles, SIGNAL(clicked(bool)), this, SLOT(addFiles()));
    connect(ui->pBAddDir, SIGNAL(clicked(bool)), this, SLOT(addDirs()));
    connect(ui->pBCancelFiles, SIGNAL(clicked(bool)), this, SLOT(cancelFiles()));
    connect(ui->pBCancelDirs, SIGNAL(clicked(bool)), this, SLOT(cancelDirs()));


}

void ChangeWindow::getMW(){
    mwCW = qobject_cast<MainWindow*>(QObject::sender());
}
void ChangeWindow::getLW(){
    lwCW = qobject_cast<ListWindow*>(QObject::sender());
}
void ChangeWindow::getLHW(){
    lhwCW = qobject_cast<ListHistWindow*>(QObject::sender());
}

void ChangeWindow::saveChanges(){
    int rowcl = this->rowcl;
    if(lwCW && mwCW && this->whoIs == "lw"){
        mwCW->Vdata[rowcl].text = this->ui->textEdit->toPlainText();
        mwCW->Vdata[rowcl].date = this->ui->dateEdit->date();
        mwCW->Vdata[rowcl].time = this->ui->timeEdit->time();
        mwCW->Vdata[rowcl].files = this->data.files;
        mwCW->Vdata[rowcl].dirs = this->data.dirs;
        lwCW->ui->tableWidget->item(rowcl, 0)->setText(this->ui->textEdit->toPlainText());
        lwCW->ui->tableWidget->item(rowcl, 1)->setText(this->ui->dateEdit->date().toString("dd.MM.yyyy"));
        lwCW->ui->tableWidget->item(rowcl, 2)->setText(this->ui->timeEdit->time().toString("hh:mm"));

        if(mwCW->Vdata.size() > 1)
            std::sort(
                (mwCW->Vdata).begin(), (mwCW->Vdata).end(),
                [](const MainWindow::Tdata& a, const MainWindow::Tdata& b){
                    if(a.date < b.date) return true;
                    else if(a.date > b.date) return false;
                    else if(a.date == b.date && a.time < b.time) return true;
                    else if(a.date == b.date && a.time > b.time) return false;
                    else return false; //убирает предупреждение In lambda function: control reaches end of non-void function [-Wreturn-type]
                }
            );

        this->file.setFileName(QDir::toNativeSeparators(QApplication::applicationDirPath() + "\\data.reminder"));
        this->file.open(QIODevice::WriteOnly); //пишется, затирая старые данные.
        QDataStream out(&(this->file));
        out.setVersion(QDataStream::Qt_5_10);
        out<<mwCW->Vdata;
        this->file.close();
    }//if(lwcW && this->whoIs == "lw")

    if(lhwCW && mwCW && this->whoIs == "lhw"){
        mwCW->VdataHist[rowcl].text = this->ui->textEdit->toPlainText();
        mwCW->VdataHist[rowcl].date = this->ui->dateEdit->date();
        mwCW->VdataHist[rowcl].time = this->ui->timeEdit->time();
        mwCW->VdataHist[rowcl].files = this->data.files;
        mwCW->VdataHist[rowcl].dirs = this->data.dirs;
        lhwCW->ui->tableWidget->item(rowcl, 0)->setText(this->ui->textEdit->toPlainText());
        lhwCW->ui->tableWidget->item(rowcl, 1)->setText(this->ui->dateEdit->date().toString("dd.MM.yyyy"));
        lhwCW->ui->tableWidget->item(rowcl, 2)->setText(this->ui->timeEdit->time().toString("hh:mm"));

        if(mwCW->VdataHist.size() > 1)
            std::sort(
                (mwCW->VdataHist).begin(), (mwCW->VdataHist).end(),
                [](const MainWindow::Tdata& a, const MainWindow::Tdata& b){
                    if(a.date < b.date) return true;
                    else if(a.date > b.date) return false;
                    else if(a.date == b.date && a.time < b.time) return true;
                    else if(a.date == b.date && a.time > b.time) return false;
                    else return false; //убирает предупреждение In lambda function: control reaches end of non-void function [-Wreturn-type]
                }
            );

        this->fileHist.setFileName(QDir::toNativeSeparators(QApplication::applicationDirPath() + "\\dataHist.reminder"));
        this->fileHist.open(QIODevice::WriteOnly); //пишется, затирая старые данные.
        QDataStream outHist(&(this->fileHist));
        outHist.setVersion(QDataStream::Qt_5_10);
        outHist<<mwCW->VdataHist;
        fileHist.close();
    }//if(lhwCW && this->whoIs == "lhw")

    this->close();
}//void ChangeWindow::saveChanges()

void ChangeWindow::openChangeWindow(){
    cwNew = new ChangeWindow;
    if(cwNew){
        if(qobject_cast<ListWindow*>(QObject::sender())) cwNew->whoIs = "lw";
        if(qobject_cast<ListHistWindow*>(QObject::sender())) cwNew->whoIs = "lhw";

        if(lwCW && cwNew->whoIs == "lw"){
            cwNew->rowcl = lwCW->rowcl; //номер текущий строки будет меткой окна
            int rowcl = cwNew->rowcl;
            cwNew->data.text = mwCW->Vdata[rowcl].text;
            cwNew->data.date = mwCW->Vdata[rowcl].date;
            cwNew->data.time = mwCW->Vdata[rowcl].time;
            cwNew->data.files = mwCW->Vdata[rowcl].files;
            cwNew->data.dirs = mwCW->Vdata[rowcl].dirs;

            lwCW->VptrCW.push_back(cwNew);
        }
        if(lhwCW && cwNew->whoIs == "lhw"){

            //запретить редактирование
            cwNew->ui->textEdit->setReadOnly(true);
//            cwNew->ui->pBSave->setEnabled(false);
//            cwNew->ui->dateEdit->setEnabled(false);
//            cwNew->ui->timeEdit->setEnabled(false);
            cwNew->ui->pBAddFiles->setEnabled(false);
            cwNew->ui->pBAddDir->setEnabled(false);
            cwNew->ui->pBCancelFiles->setEnabled(false);
            cwNew->ui->pBCancelDirs->setEnabled(false);
            cwNew->setWindowTitle("Reminder: Просмотр истории");

            cwNew->rowcl = lhwCW->rowcl; //номер текущий строки будет меткой окна
            int rowcl = cwNew->rowcl;
            cwNew->data.text = mwCW->VdataHist[rowcl].text;
            cwNew->data.date = mwCW->VdataHist[rowcl].date;
            cwNew->data.time = mwCW->VdataHist[rowcl].time;
            cwNew->data.files = mwCW->VdataHist[rowcl].files;
            cwNew->data.dirs = mwCW->VdataHist[rowcl].dirs;

            lhwCW->VptrCW.push_back(cwNew);
        }

        cwNew->ui->textEdit->setText(cwNew->data.text);
        cwNew->ui->dateEdit->setDate(cwNew->data.date);
        cwNew->ui->timeEdit->setTime(cwNew->data.time);
        cwNew->ui->textEditOut->setText("<b>Было запланировано на:</b>");
        cwNew->ui->textEditOut->append((cwNew->data.date).toString("dd.MM.yyyy"));
        cwNew->ui->textEditOut->append((cwNew->data.time).toString("hh:mm"));

        if(!cwNew->data.files.isEmpty()){
            cwNew->ui->textEditOut->append("<b>Заданные файлы:</b>");
            int sz = cwNew->data.files.size();
            for(int i = 0; i < sz; ++i)
                cwNew->ui->textEditOut->append(
                            QDir::toNativeSeparators(
                                (cwNew->data.files)[i]
                                )
                            );
        } else
            cwNew->ui->textEditOut->append("<b>Заданные файлы:</b> Нет.");

        if(!cwNew->data.dirs.isEmpty()){
            cwNew->ui->textEditOut->append("<b>Заданные каталоги:</b>");
            int sz = cwNew->data.dirs.size();
            for(int i = 0; i < sz; ++i)
                cwNew->ui->textEditOut->append(
                            QDir::toNativeSeparators(
                                (cwNew->data.dirs)[i]
                                )
                            );
        } else
            cwNew->ui->textEditOut->append("<b>Заданные каталоги:</b> Нет.");

        cwNew->show();
    } //if(cwNew)
} //void ChangeWindow::openChangeWindow()

void ChangeWindow::openFiles(){
    if(!this->data.files.isEmpty()){
        int sz = this->data.files.size();
        for(int i = 0; i < sz; ++i)
            if(!QDesktopServices::openUrl(
                        QUrl(
                            QUrl::fromLocalFile(
                                QDir::toNativeSeparators(this->data.files[i])
                                ).toString(), QUrl::TolerantMode
                            )
                        )
                    ){
                this->ui->textEditOut->append("<b>Файл не найден:</b> ");
                this->ui->textEditOut->append(QDir::toNativeSeparators(this->data.files[i]));
            }
    }
}

void ChangeWindow::openDirs(){
    if(!this->data.dirs.isEmpty()){
        int sz = this->data.dirs.size();
        for(int i = 0; i < sz; ++i)
            if(!QDesktopServices::openUrl(
                        QUrl(
                            QUrl::fromLocalFile(
                                QDir::toNativeSeparators(this->data.dirs[i])
                                ).toString(), QUrl::TolerantMode
                            )
                        )
                    ){
                this->ui->textEditOut->append("<b>Каталог не найден:</b> ");
                this->ui->textEditOut->append(QDir::toNativeSeparators(this->data.dirs[i]));
            }
    }
}

void ChangeWindow::addFiles(){
    QStringList files = QFileDialog::getOpenFileNames(
                            0,
                            "Выберите файлы для напоминания",
                            "",
                            "*.*",
                            nullptr,
                            QFileDialog::ReadOnly
                        );
    if(!files.isEmpty()) this->data.files.append(files);
    if(!this->data.files.isEmpty() && !files.isEmpty()){
        ui->textEditOut->append("<b>Выбраны файлы:</b>");
        int sz = this->data.files.size();
        for(int i = 0; i < sz; ++i)
            ui->textEditOut->append(
                                QDir::toNativeSeparators(
                                    (this->data.files)[i]
                                )
                            );
    }

}

void ChangeWindow::addDirs(){
    QString dir = QFileDialog::getExistingDirectory(
                      0,
                      "Выберите каталог для напоминания",
                      QDir::toNativeSeparators(QApplication::applicationDirPath()),
                      QFileDialog::DontUseNativeDialog
                      | QFileDialog::ReadOnly
                  );
    if(!dir.isEmpty()) this->data.dirs.append(dir);
    if(!this->data.dirs.isEmpty() && !dir.isEmpty()){
        ui->textEditOut->append("<b>Выбраны каталоги:</b>");
        int sz = this->data.dirs.size();
        for(int i = 0; i < sz; ++i)
            ui->textEditOut->append(
                                QDir::toNativeSeparators(
                                    (this->data.dirs)[i]
                                )
                            );
    }

}

void ChangeWindow::cancelFiles(){
    if(!this->data.files.isEmpty()){
        this->data.files.clear();
        this->ui->textEditOut->append("<b>Выбраны файлы:</b> Нет.");
    }
}

void ChangeWindow::cancelDirs(){
    if(!this->data.dirs.isEmpty()){
        this->data.dirs.clear();
        this->ui->textEditOut->append("<b>Выбраны каталоги:</b> Нет.");
    }
}

void ChangeWindow::clock(QString &clText){
    if(lwCW && !lwCW->VptrCW.isEmpty()){
        int sz = lwCW->VptrCW.size();
        for(int i = 0; i<sz; ++i)
            lwCW->VptrCW[i]->ui->lbClock->setText(clText);
    }
    if(lhwCW && !lhwCW->VptrCW.isEmpty()){
        int sz = lhwCW->VptrCW.size();
        for(int i = 0; i<sz; ++i)
            lhwCW->VptrCW[i]->ui->lbClock->setText(clText);
    }
}

void ChangeWindow::closeEvent(QCloseEvent *event){
    if(lwCW && this->whoIs == "lw"){
        if(!lwCW->OpenRow.isEmpty()){
            int sz = lwCW->OpenRow.size();
            for(int i = 0; i < sz; ++i)
                if(this->rowcl == lwCW->OpenRow[i]){
                    if(sz > 1){
                        lwCW->OpenRow.remove(i);
                        break;
                    }
                    else{
                        lwCW->OpenRow.clear();
                        break;
                    }
                }
        }

        if(!lwCW->VptrCW.isEmpty()){
            int sz = lwCW->VptrCW.size();
            for(int i = 0; i < sz; ++i)
                if(this == lwCW->VptrCW[i]){
                    if(sz > 1){
                        lwCW->VptrCW.remove(i);
                        break;
                    }
                    else{
                        lwCW->VptrCW.clear();
                        break;
                    }
                }
        }
    }

    if(lhwCW && this->whoIs == "lhw"){
        if(!lhwCW->OpenRow.isEmpty()){
            int sz = lhwCW->OpenRow.size();
            for(int i = 0; i < sz; ++i)
                if(this->rowcl == lhwCW->OpenRow[i]){
                    if(sz > 1){
                        lhwCW->OpenRow.remove(i);
                        break;
                    }
                    else{
                        lhwCW->OpenRow.clear();
                        break;
                    }
                }
        }

        if(!lhwCW->VptrCW.isEmpty()){
            int sz = lhwCW->VptrCW.size();
            for(int i = 0; i < sz; ++i)
                if(this == lhwCW->VptrCW[i]){
                    if(sz > 1){
                        lhwCW->VptrCW.remove(i);
                        break;
                    }
                    else{
                        lhwCW->VptrCW.clear();
                        break;
                    }
                }
        }
    }
    event->accept();
}

void ChangeWindow::closeCW(){
    this->close();
}

void ChangeWindow::closeAllCW(){
    if(qobject_cast<ListWindow*>(QObject::sender()) && lwCW && !lwCW->VptrCW.isEmpty()){
        int sz = lwCW->VptrCW.size();
        for(int i = 0; i < sz; ++i)
            (lwCW->VptrCW)[0]->close();
    }
    if(qobject_cast<ListHistWindow*>(QObject::sender()) && lhwCW && !lhwCW->VptrCW.isEmpty()){
        int sz = lhwCW->VptrCW.size();
        for(int i = 0; i < sz; ++i)
            (lhwCW->VptrCW)[0]->close();
    }

}

ChangeWindow::~ChangeWindow()
{
    delete ui;
}
