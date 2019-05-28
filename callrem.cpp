#include "mainwindow.h"
#include "ui_callrem.h"
#include "ui_mainwindow.h"
#include "listwindow.h"
#include <QSignalMapper>
#include <QtDebug>
#include <QShortcut>
#include <QDesktopServices>
#include <QApplication>
#include <QUrl>

MainWindow* mwCR;
ListWindow* lwCR;

CallRem::CallRem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CallRem),
    cntCall(0)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->timeEdit_2->setTime(QTime::currentTime());
    ui->timeEdit->setTime(QTime(0, 10, 0, 0));

    connect(ui->pBTy, SIGNAL(clicked(bool)), this, SLOT(closeCall()));
    connect(ui->pBRep, SIGNAL(clicked(bool)), this, SLOT(callRep()));
    connect(ui->pBRepIn, SIGNAL(clicked(bool)), this, SLOT(callRepIn()));

    QSignalMapper *mapper = new QSignalMapper(this);
    mapper->setMapping(ui->pBRep5, QString("00:05"));
    mapper->setMapping(ui->pBRep10, QString("00:10"));
    mapper->setMapping(ui->pBRep15, QString("00:15"));
    mapper->setMapping(ui->pBRep30, QString("00:30"));
    mapper->setMapping(ui->pBRep45, QString("00:45"));
    mapper->setMapping(ui->pBRep60, QString("01:00"));

    connect(ui->pBRep5, SIGNAL(clicked(bool)), mapper, SLOT(map()));
    connect(ui->pBRep10, SIGNAL(clicked(bool)), mapper, SLOT(map()));
    connect(ui->pBRep15, SIGNAL(clicked(bool)), mapper, SLOT(map()));
    connect(ui->pBRep30, SIGNAL(clicked(bool)), mapper, SLOT(map()));
    connect(ui->pBRep45, SIGNAL(clicked(bool)), mapper, SLOT(map()));
    connect(ui->pBRep60, SIGNAL(clicked(bool)), mapper, SLOT(map()));

    connect(mapper, SIGNAL(mapped(const QString&)), this, SLOT(callTime(const QString&)));

    QShortcut *CRkeyEsc = new QShortcut(this);
    CRkeyEsc->setKey(Qt::Key_Escape);
    connect(CRkeyEsc, SIGNAL(activated()), this, SLOT(close()));

    connect(ui->pBOpenFiles, SIGNAL(clicked(bool)), this, SLOT(openFiles()));
    connect(ui->pBOpenDirs, SIGNAL(clicked(bool)), this, SLOT(openDirs()));
//    connect(this, SIGNAL(autoOpenFiles()), this, SLOT(openFiles()));
//    connect(this, SIGNAL(autoOpenDirs()), this, SLOT(openDirs()));

} //CallRem

void CallRem::getMW(){
    mwCR = qobject_cast<MainWindow*>(QObject::sender());
}

void CallRem::getLW(){
    lwCR = qobject_cast<ListWindow*>(QObject::sender());
}

void CallRem::call(){
    if(
     !cntCall &&  //если текущие напоминания не обрабатываются сейчас
     mwCR &&
     lwCR &&
     !(mwCR->Vdata).isEmpty() &&
     (
        ( (mwCR->Vdata)[0].date < QDate::currentDate() ) ||
        ( (mwCR->Vdata)[0].date == QDate::currentDate() && (mwCR->Vdata)[0].time <= QTime::currentTime() )
     )
    ){
        cntCall = 1;
        for(int i = 1; i < (mwCR->Vdata).size(); ++i){
            if(
             (mwCR->Vdata)[i].date == (mwCR->Vdata)[0].date &&
             (mwCR->Vdata)[i].time == (mwCR->Vdata)[0].time
            )
                cntCall++;
            else break;
        }
        for(int i = 0; i < cntCall; ++i){
            crCall = new CallRem;
            if(crCall){
                crCall->ui->textEdit->setText((mwCR->Vdata)[0].text);

                crCall->ui->textEditOut->setText("<b>Было запланировано на:</b> " +
                                                 (mwCR->Vdata)[0].date.toString("dd.MM.yyyy") +
                        " " +
                        (mwCR->Vdata)[0].time.toString("hh:mm")
                        );

                //печать путей к выбранным файлам и директориям
                if((mwCR->Vdata)[0].files.size()){
                    crCall->ui->textEditOut->append("<b>Заданные файлы:</b>");
                    for(int i = 0; i<(mwCR->Vdata)[0].files.size(); ++i)
                        crCall->ui->textEditOut->append(
                                    QDir::toNativeSeparators(
                                        ((mwCR->Vdata)[0].files)[i]
                                    )
                                );
                    crCall->files = (mwCR->Vdata)[0].files;
                } else {
                    crCall->ui->textEditOut->append("<b>Заданные файлы:</b> Нет");
                    crCall->ui->pBOpenFiles->setEnabled(false);
                }
                if((mwCR->Vdata)[0].dirs.size()){
                    crCall->ui->textEditOut->append("<b>Заданные каталоги:</b>");
                    for(int i = 0; i<(mwCR->Vdata)[0].dirs.size(); ++i)
                        crCall->ui->textEditOut->append(
                                    QDir::toNativeSeparators(
                                        ((mwCR->Vdata)[0].dirs)[i]
                                    )
                                );
                    crCall->dirs = (mwCR->Vdata)[0].dirs;
                } else {
                    crCall->ui->textEditOut->append("<b>Заданные каталоги:</b> Нет");
                    crCall->ui->pBOpenDirs->setEnabled(false);
                }

                //печать информации о звуках
                if(!(mwCR->noSound)){
                    crCall->ui->textEditOut->append("<b>Звуки:</b> Вкл.");
                    crCall->ui->textEditOut->append("<b>Текущий звук:</b>");
                    if(!(mwCR->isMySound))
                        crCall->ui->textEditOut->append(
                                    QDir::toNativeSeparators(
                                        QApplication::applicationDirPath() +
                                        "\\sounds\\" +
                                        mwCR->currSound +
                                        ".wav"
                                        )
                                    );
                    if(mwCR->isMySound)
                        crCall->ui->textEditOut->append(mwCR->currSound);

                } else crCall->ui->textEditOut->append("<b>Звуки:</b> Откл.");

                mwCR->VptrCR.push_back(crCall);
                crCall->show();

                lwCR->rowcl = 0;
                lwCR->colcl = 0;
                emit delCall();

                //открытие файлов и каталогов
                if(mwCR->isAutoOpenFiles && !crCall->files.isEmpty())
                    crCall->openFiles();
                if(mwCR->isAutoOpenDirs && !crCall->dirs.isEmpty())
                    crCall->openDirs();

            } //if(crCall)
        } //for(int i = 0; i < cntCall; ++i)
        cntCall = 0;

        if(crCall && !mwCR->noSound){
            if(!mwCR->isMySound){
                QSound::play(
                    QDir::toNativeSeparators(
                        QApplication::applicationDirPath() +
                        "\\sounds\\" +
                        mwCR->currSound +
                        ".wav"
                    )
                );
            }
            if(mwCR->isMySound){
                QMediaPlayer *pls = new QMediaPlayer(crCall);
                pls->setMedia(QUrl::fromLocalFile(mwCR->currSound));
                pls->setVolume(50);
                pls->play();
            }
        } //if(crCall && !mwCR->noSound)
    }
} //void CallRem::call()

void CallRem::callRep(){
    mwCR->ui->textEdit->setText(this->ui->textEdit->toPlainText());

    QDate currDate = QDate::currentDate();
    QTime currTime = QTime::currentTime();
    int mSecsInDay = 86400000;
    QDate callDate = currDate;

    int currMsTime = currTime.msecsSinceStartOfDay();
    int deltaMsTime = this->ui->timeEdit->time().msecsSinceStartOfDay(); //через сколько прозвонить опять

    int callMsTime = currMsTime + deltaMsTime;
    //если время переходит в след. сутки
    if(callMsTime >= mSecsInDay){
        callMsTime = callMsTime - mSecsInDay;
        callDate = callDate.addDays(1);
    }
    QTime callTime = QTime::fromMSecsSinceStartOfDay(callMsTime);
    mwCR->ui->dateEdit->setDate(callDate);
    mwCR->ui->timeEdit->setTime(callTime);
    connect(this, SIGNAL(addData()), mwCR, SLOT(addData())); //добавить напоминание из звонка
    emit addData();
    this->close();
}

void CallRem::callRepIn(){
    mwCR->ui->textEdit->setText(this->ui->textEdit->toPlainText());
    mwCR->ui->dateEdit->setDate(this->ui->dateEdit->date());
    mwCR->ui->timeEdit->setTime(this->ui->timeEdit_2->time());
    if(!this->files.isEmpty())
        mwCR->data.files = this->files;
    if(!this->dirs.isEmpty())
        mwCR->data.dirs = this->dirs;

    connect(this, SIGNAL(addData()), mwCR, SLOT(addData())); //добавить напоминание из звонка
    emit addData();
    this->close();
}

void CallRem::openFiles(){
    if(!this->files.isEmpty()){
        this->ui->textEditOut->append("<b>Открытие файлов...</b> ");
        int sz = this->files.size();
        for(int i = 0; i < sz; ++i)
            if(!QDesktopServices::openUrl(
                        QUrl(
                            QUrl::fromLocalFile(
                                QDir::toNativeSeparators(this->files[i])
                                ).toString(), QUrl::TolerantMode
                            )
                        )
                    ){
                this->ui->textEditOut->append("<b>Файл не найден:</b> ");
                this->ui->textEditOut->append(QDir::toNativeSeparators(this->files[i]));
            }
    }
}

void CallRem::openDirs(){
    if(!this->dirs.isEmpty()){
        this->ui->textEditOut->append("<b>Открытие каталогов...</b> ");
        int sz = this->dirs.size();
        for(int i = 0; i < sz; ++i)
            if(!QDesktopServices::openUrl(
                        QUrl(
                            QUrl::fromLocalFile(
                                QDir::toNativeSeparators(this->dirs[i])
                                ).toString(), QUrl::TolerantMode
                            )
                        )
                    ){
                this->ui->textEditOut->append("<b>Каталог не найден:</b> ");
                this->ui->textEditOut->append(QDir::toNativeSeparators(this->dirs[i]));
            }
    }
}

void CallRem::callTime(const QString& stime){
    QTime time = QTime::fromString(stime, "hh:mm");
    ui->timeEdit->setTime(time);
}

void CallRem::clock(QString &clText){
    if(!mwCR->VptrCR.isEmpty()){
        int sz = mwCR->VptrCR.size();
        for(int i = 0; i<sz; ++i)
            mwCR->VptrCR[i]->ui->lbClock->setText(clText);
    }
}

void CallRem::closeEvent(QCloseEvent *event){
    if(!mwCR->VptrCR.isEmpty()){
        int sz = mwCR->VptrCR.size();
        for(int i = 0; i<sz; ++i)
            if(this == mwCR->VptrCR[i]){
                if(sz > 1){
                    mwCR->VptrCR.remove(i);
                    break;
                }
                else{
                    mwCR->VptrCR.clear();
                    break;
                }
            }
    }
    event->accept();
}

void CallRem::closeCall(){
    this->close();
}

void CallRem::closeAllCall(){
    if(!mwCR->VptrCR.isEmpty()){
        int sz = mwCR->VptrCR.size();
        for(int i = 0; i < sz; ++i)
            (mwCR->VptrCR)[0]->close();
    }
}

CallRem::~CallRem()
{
    delete ui;
}
