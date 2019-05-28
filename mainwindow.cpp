//#include "listwindow.h"
//#include "listhistwindow.h"
//#include "callrem.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QMessageBox>
#include <QFileDialog>

//ListWindow *lwMW;
//ListHistWindow *lhwMW;
//CallRem *crMW;

QDataStream& operator <<(QDataStream& qos, const MainWindow::Tdata& data){
    qos<<data.text<<data.date<<data.time<<data.files<<data.dirs;
    return qos;
}
QDataStream& operator >>(QDataStream& qis, MainWindow::Tdata& data){
    qis>>data.text>>data.date>>data.time>>data.files>>data.dirs;
    return qis;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);

    ui->dateEdit->setDate(QDate::currentDate());
    ui->timeEdit->setTime(QTime::currentTime());
    //перед запуском файлы должны быть созданы
    file.setFileName(QDir::toNativeSeparators(QApplication::applicationDirPath() + "\\data.reminder"));
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_10);
    in>>Vdata;
    file.close();

    fileHist.setFileName(QDir::toNativeSeparators(QApplication::applicationDirPath() + "\\dataHist.reminder"));
    fileHist.open(QIODevice::ReadOnly);
    QDataStream inHist(&fileHist);
    inHist.setVersion(QDataStream::Qt_5_10);
    inHist>>VdataHist;
    fileHist.close();

    if(Vdata.size() > 1)
        std::sort(
            Vdata.begin(), Vdata.end(),
            [](const Tdata& a, const Tdata& b){
                if(a.date < b.date) return true;
                else if(a.date > b.date) return false;
                else if(a.date == b.date && a.time < b.time) return true;
                else if(a.date == b.date && a.time > b.time) return false;
                else return false; //убирает предупреждение In lambda function: control reaches end of non-void function [-Wreturn-type]
            }
        );
    if(VdataHist.size() > 1)
        std::sort(
            VdataHist.begin(), VdataHist.end(),
            [](const Tdata& a, const Tdata& b){
                if(a.date < b.date) return true;
                else if(a.date > b.date) return false;
                else if(a.date == b.date && a.time < b.time) return true;
                else if(a.date == b.date && a.time > b.time) return false;
                else return false; //убирает предупреждение In lambda function: control reaches end of non-void function [-Wreturn-type]
            }
        );
    connect(ui->pBAdd, SIGNAL(clicked()), this, SLOT(addData())); //добавить напоминание в файл и вектор
    connect(ui->pBList, SIGNAL(clicked(bool)), this, SLOT(showList_Sl()));
    connect(ui->pBHistList, SIGNAL(clicked(bool)), this, SLOT(showHistList_Sl()));

    //автозагрузка
    settings = new QSettings(QDir::toNativeSeparators(QApplication::applicationDirPath() + "\\settings.ini"), QSettings::IniFormat);
    settings->beginGroup("system");
    sl = settings->value("AutoLoad", "false").toBool();
    if(sl){
        ui->actAutoLoad->setChecked(true);
        ui->textEditOut->setText("<b>Автозагрузка:</b> Вкл.");
    }
    else{
        ui->actAutoLoad->setChecked(false);
        ui->textEditOut->setText("<b>Автозагрузка:</b> Откл.");
    }
    settings->endGroup();
    connect(ui->actAutoLoad, SIGNAL(triggered(bool)), this, SLOT(setAutoLoad(bool)));

    //звуки
    settings->beginGroup("sounds");

    noSound = settings->value("noSound", "true").toBool();
    if(noSound){
        ui->actNoSound->setChecked(true);
        ui->textEditOut->append("<b>Звуки:</b> Откл.");
    }
    else{
        ui->actNoSound->setChecked(false);
        ui->textEditOut->append("<b>Звуки:</b> Вкл.");
    }
    ui->textEditOut->append("<b>Выбран звук:</b>");
    currSound = settings->value("currSound", "false").toString();
    isMySound = settings->value("isMySound", "false").toBool();
    if(!isMySound){
        ui->textEditOut->append(
                    QDir::toNativeSeparators(
                     QApplication::applicationDirPath() +
                     "\\sounds\\" +
                     currSound +
                     ".wav"
                    )
                );
        if(currSound == "false") ui->actNoSound->setChecked(true);
        if(currSound == "bushorn") ui->actionbushorn->setChecked(true);
        if(currSound == "cow") ui->actioncow->setChecked(true);
        if(currSound == "hey") ui->actionhey->setChecked(true);
        if(currSound == "kostel") ui->actionkostel->setChecked(true);
        if(currSound == "phone") ui->actionphone->setChecked(true);
    }
    else{
        ui->actMySound->setChecked(true);
        ui->textEditOut->append(currSound);
    }
    settings->endGroup();

    //открывать ли каталоги автоматически при срабатывании нап.
    settings->beginGroup("autoOpenFD");
    if(settings->value("autoOpenFiles", "false").toBool()){
        isAutoOpenFiles = true;
        ui->actAutoOpenFiles->setChecked(true);
        ui->textEditOut->append("<b>Автооткрытие файлов:</b> Да.");
    }
    else{
        isAutoOpenFiles = false;
        ui->actAutoOpenFiles->setChecked(false);
        ui->textEditOut->append("<b>Автооткрытие файлов:</b> Нет.");
    }

    if(settings->value("autoOpenDirs", "false").toBool()){
        isAutoOpenDirs = true;
        ui->actAutoOpenDirs->setChecked(true);
        ui->textEditOut->append("<b>Автооткрытие каталогов:</b> Да.");
    }
    else{
        isAutoOpenDirs = false;
        ui->actAutoOpenDirs->setChecked(false);
        ui->textEditOut->append("<b>Автооткрытие каталогов:</b> Нет.");
    }
    settings->endGroup();

    ui->textEditOut->append("<b>Выбраны файлы:</b> Нет.");
    ui->textEditOut->append("<b>Выбраны каталоги:</b> Нет.");

    connect(ui->actNoSound, SIGNAL(triggered(bool)), this, SLOT(isSound(bool)));

    mapperActSound = new QSignalMapper(this);
    mapperActSound->setMapping(ui->actionbushorn, QString("bushorn"));
    mapperActSound->setMapping(ui->actioncow, QString("cow"));
    mapperActSound->setMapping(ui->actionhey, QString("hey"));
    mapperActSound->setMapping(ui->actionkostel, QString("kostel"));
    mapperActSound->setMapping(ui->actionphone, QString("phone"));
    mapperActSound->setMapping(ui->actMySound, QString("mysound"));
    connect(ui->actionbushorn, SIGNAL(triggered(bool)), mapperActSound, SLOT(map()));
    connect(ui->actioncow, SIGNAL(triggered(bool)), mapperActSound, SLOT(map()));
    connect(ui->actionhey, SIGNAL(triggered(bool)), mapperActSound, SLOT(map()));
    connect(ui->actionkostel, SIGNAL(triggered(bool)), mapperActSound, SLOT(map()));
    connect(ui->actionphone, SIGNAL(triggered(bool)), mapperActSound, SLOT(map()));
    connect(ui->actMySound, SIGNAL(triggered(bool)), mapperActSound, SLOT(map()));
    connect(mapperActSound, SIGNAL(mapped(const QString&)), this, SLOT(currSoundSl(const QString&)));

    connect(ui->actAbout, SIGNAL(triggered(bool)), this, SLOT(aboutRem()));
    connect(ui->actionReadme_txt, SIGNAL(triggered(bool)), this, SLOT(readme()));
    connect(ui->pBDonate, SIGNAL(clicked(bool)), this, SLOT(donate()));

    connect(ui->pBAddFiles, SIGNAL(clicked(bool)), this, SLOT(addFiles()));
    connect(ui->pBAddDirs, SIGNAL(clicked(bool)), this, SLOT(addDirs()));
    connect(ui->actCancelFiles, SIGNAL(triggered(bool)), this, SLOT(cancelFiles()));
    connect(ui->actCancelDirs, SIGNAL(triggered(bool)), this, SLOT(cancelDirs()));
    connect(ui->actAutoOpenFiles, SIGNAL(triggered(bool)), this, SLOT(autoOpenFiles(bool)));
    connect(ui->actAutoOpenDirs, SIGNAL(triggered(bool)), this, SLOT(autoOpenDirs(bool)));

} //MainWindow

void MainWindow::addFiles(){
    QStringList files = QFileDialog::getOpenFileNames(
                            0,
                            "Выберите файлы для напоминания",
                            "",
                            "*.*",
                            nullptr,
                            QFileDialog::ReadOnly
                        );
    if(!files.isEmpty()) data.files.append(files);
    if(!data.files.isEmpty() && !files.isEmpty()){
        ui->textEditOut->append("<b>Выбраны файлы:</b>");
        int sz = data.files.size();
        for(int i = 0; i < sz; ++i)
            ui->textEditOut->append(
                                QDir::toNativeSeparators(
                                    (data.files)[i]
                                )
                            );
    }
}

void MainWindow::addDirs(){
    QString dir = QFileDialog::getExistingDirectory(
                      0,
                      "Выберите каталог для напоминания",
                      QDir::toNativeSeparators(QApplication::applicationDirPath()),
                      QFileDialog::DontUseNativeDialog
                      | QFileDialog::ReadOnly
                  );
    if(!dir.isEmpty()) data.dirs.append(dir);
    if(!data.dirs.isEmpty() && !dir.isEmpty()){
        ui->textEditOut->append("<b>Выбраны каталоги:</b>");
        int sz = data.dirs.size();
        for(int i = 0; i < sz; ++i)
            ui->textEditOut->append(
                                QDir::toNativeSeparators(
                                    (data.dirs)[i]
                                )
                            );
    }
}

void MainWindow::cancelFiles(){
    if(!data.files.isEmpty()){
        data.files.clear();
        ui->textEditOut->append("<b>Выбраны файлы:</b> Нет.");
    }
}

void MainWindow::cancelDirs(){
    if(!data.dirs.isEmpty()){
        data.dirs.clear();
        ui->textEditOut->append("<b>Выбраны каталоги:</b> Нет.");
    }
}

void MainWindow::autoOpenFiles(bool isAof){
    if(isAof){
        settings->setValue("autoOpenFD/autoOpenFiles", "true");
        ui->textEditOut->append("<b>Автооткрытие файлов:</b> Да.");
        isAutoOpenFiles = true;
    }
    else{
        settings->setValue("autoOpenFD/autoOpenFiles", "false");
        ui->textEditOut->append("<b>Автооткрытие файлов:</b> Нет.");
        isAutoOpenFiles = false;
    }
}

void MainWindow::autoOpenDirs(bool isAod){
    if(isAod){
        settings->setValue("autoOpenFD/autoOpenDirs", "true");
        ui->textEditOut->append("<b>Автооткрытие каталогов:</b> Да.");
        isAutoOpenDirs = true;
    }
    else{
        settings->setValue("autoOpenFD/autoOpenDirs", "false");
        ui->textEditOut->append("<b>Автооткрытие каталогов:</b> Нет.");
        isAutoOpenDirs = false;
    }
}

void MainWindow::addData(){
    data.text = ui->textEdit->toPlainText();
    data.date = ui->dateEdit->date();
    data.time = ui->timeEdit->time();
    Vdata.push_back(data);
    if(Vdata.size() > 1)
        std::sort(
            Vdata.begin(), Vdata.end(),
            [](const Tdata& a, const Tdata& b){
                if(a.date < b.date) return true;
                else if(a.date > b.date) return false;
                else if(a.date == b.date && a.time < b.time) return true;
                else if(a.date == b.date && a.time > b.time) return false;
                else return false; //убирает предупреждение In lambda function: control reaches end of non-void function [-Wreturn-type]
            }
        );
    file.setFileName(QDir::toNativeSeparators(QApplication::applicationDirPath() + "\\data.reminder"));
    file.open(QIODevice::WriteOnly); //пишется, затирая старые данные.
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_10);
    out<<Vdata;
    file.close();

    ui->textEditOut->append("<b>Напоминание добавлено.</b>");
    //чтобы пути не попали в следующее нап.
    if(!data.files.isEmpty()){
        data.files.clear();
        ui->textEditOut->append("<b>Выбраны файлы:</b> Нет.");
    }
    if(!data.dirs.isEmpty()){
        data.dirs.clear();
        ui->textEditOut->append("<b>Выбраны каталоги:</b> Нет.");
    }

    emit AddedData();
}

void MainWindow::showList_Sl(){
    emit showList();
}
void MainWindow::showHistList_Sl(){
    emit showHistList();
}

void MainWindow::clock(){
    QString clText =
        QDate::currentDate().toString("dd.MM.yyyy") +
        "  " +
        QTime::currentTime().toString("hh:mm:ss");
    ui->lbClock->setText(clText);
    emit clockOther(clText);
}

void MainWindow::setAutoLoad(bool isCh){
    settings->beginGroup("system");
    if(isCh){
        #ifdef Q_OS_WIN32

        QSettings autoLoad("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        autoLoad.setValue(QApplication::applicationName(), QDir::toNativeSeparators(QApplication::applicationFilePath()));
        autoLoad.sync();

        QMessageBox::information(
            this,
            QApplication::applicationName(),
            tr("Программа добавлена в автозагрузку"),
            QMessageBox::Ok
        );
        settings->setValue("AutoLoad", "true");
        ui->textEditOut->append("<b>Автозагрузка:</b> Вкл.");
        #endif

    }
    else{
        #ifdef Q_OS_WIN32

        QSettings autoLoad("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        autoLoad.remove(QApplication::applicationName());

        QMessageBox::information(
            this,
            QApplication::applicationName(),
            tr("Программа удалена из автозагрузки"),
            QMessageBox::Ok
        );
        settings->setValue("AutoLoad", "false");
        ui->textEditOut->append("<b>Автозагрузка:</b> Откл.");
        #endif

    }
    settings->endGroup();
}

void MainWindow::icnAct(QSystemTrayIcon::ActivationReason r){
    if(r == QSystemTrayIcon::Trigger){
        if(this->isVisible()){
            this->hide();
            emit closeAllOther();
        }
        else if(!this->isVisible()) this->show();
    }
}

void MainWindow::aboutRem(){
    QString strAbout = QString("Программа-напоминалка Reminder v. 1.5\n") +
                     QString("Напоминание представляет собой заданный вами текст в окне, файлы и каталоги, которые необходимо открыть в заданное время.\n") +
                     QString("LGPLv3\n") +
                     QString("Copyright (C) 2018г. Кучеров Анатолий Александрович\n") +
                     QString(" ulvprog@gmail.com\n") +
                     QString("https://ulvprog.wordpress.com/ \n") +
                     QString("Программа бесплатна. Если хотите помочь автору программы или отблагодарить его, вы можете перевести деньги на карточку ОАО «АСБ Беларусбанк»\n") +
                     QString("Белкарт (BYN: Белорусский рубль):\n") +
                     QString("Номер карты получателя: 9112000169034028\n") +
                     QString("Срок действия карты получателя 11/21")
            ;
    QMessageBox::about(this, "О программе", strAbout);
}

void MainWindow::readme(){
    if(!QDesktopServices::openUrl(
                QUrl(
                    QUrl::fromLocalFile(
                        QDir::toNativeSeparators(QApplication::applicationDirPath() + "\\Readme.txt")
                        ).toString(), QUrl::TolerantMode
                    )
                )
            ){
        ui->textEditOut->append("<b>Файл Readme.txt не найден.</b> ");
    }

}

void MainWindow::donate(){
    QString text =
            QString("Программа бесплатна. Если хотите помочь автору программы или отблагодарить его, вы можете перевести деньги на карточку ОАО «АСБ Беларусбанк»\n") +
            QString("Белкарт (BYN: Белорусский рубль):\n") +
            QString("Номер карты получателя: 9112000169034028\n") +
            QString("Срок действия карты получателя 11/21")
    ;

    QMessageBox::information(
        this,
        "Отблагодарить автора",
        text,
        QMessageBox::Ok
    );

}

void MainWindow::isSound(bool noSound){
    settings->beginGroup("sounds");
    if(noSound){
        settings->setValue("noSound", true);
        MainWindow::noSound = true;
        ui->textEditOut->append("<b>Звуки:</b> Откл.");
    }
    else{
        settings->setValue("noSound", false);
        MainWindow::noSound = false;
        ui->textEditOut->append("<b>Звуки:</b> Вкл.");
    }
    settings->endGroup();
}

void MainWindow::currSoundSl(const QString &currSound){
    disconnect(mapperActSound, SIGNAL(mapped(const QString&)), this, SLOT(currSoundSl(const QString&)));
    if(currSound == "bushorn"){
        ui->actionbushorn->setChecked(true);
        ui->actioncow->setChecked(false);
        ui->actionhey->setChecked(false);
        ui->actionkostel->setChecked(false);
        ui->actionphone->setChecked(false);
        ui->actMySound->setChecked(false);
    }
    if(currSound == "cow"){
        ui->actionbushorn->setChecked(false);
        ui->actioncow->setChecked(true);
        ui->actionhey->setChecked(false);
        ui->actionkostel->setChecked(false);
        ui->actionphone->setChecked(false);
        ui->actMySound->setChecked(false);
    }
    if(currSound == "hey"){
        ui->actionbushorn->setChecked(false);
        ui->actioncow->setChecked(false);
        ui->actionhey->setChecked(true);
        ui->actionkostel->setChecked(false);
        ui->actionphone->setChecked(false);
        ui->actMySound->setChecked(false);
    }
    if(currSound == "kostel"){
        ui->actionbushorn->setChecked(false);
        ui->actioncow->setChecked(false);
        ui->actionhey->setChecked(false);
        ui->actionkostel->setChecked(true);
        ui->actionphone->setChecked(false);
        ui->actMySound->setChecked(false);
    }
    if(currSound == "phone"){
        ui->actionbushorn->setChecked(false);
        ui->actioncow->setChecked(false);
        ui->actionhey->setChecked(false);
        ui->actionkostel->setChecked(false);
        ui->actionphone->setChecked(true);
        ui->actMySound->setChecked(false);
    }
    if(currSound == "mysound"){
        QString pathSound = QFileDialog::getOpenFileName(
                0,
                "Выберите звуковой файл",
                "",
                "*.mp3 *.ogg *.wav",
                nullptr,
                QFileDialog::ReadOnly
             );
        if(!pathSound.isEmpty()){
            ui->actionbushorn->setChecked(false);
            ui->actioncow->setChecked(false);
            ui->actionhey->setChecked(false);
            ui->actionkostel->setChecked(false);
            ui->actionphone->setChecked(false);
            ui->actMySound->setChecked(true);
            pathSound = QDir::toNativeSeparators(pathSound);
            settings->setValue("sounds/isMySound", true);
            settings->setValue("sounds/currSound", pathSound);
            MainWindow::currSound = pathSound;
            isMySound = true;
            ui->textEditOut->append("<b>Выбран звук:</b>");
            ui->textEditOut->append(MainWindow::currSound);
        }
        if(pathSound.isEmpty()){
            if(settings->value("sounds/isMySound").toBool())
                ui->actMySound->setChecked(true);
            else
                ui->actMySound->setChecked(false);
        }
    } //if(currSound == "mysound")

    connect(mapperActSound, SIGNAL(mapped(const QString&)), this, SLOT(currSoundSl(const QString&)));

    if(currSound != "mysound"){
        settings->setValue("sounds/isMySound", false);
        settings->setValue("sounds/currSound", currSound);
        MainWindow::currSound = currSound;
        isMySound = false;
        ui->textEditOut->append("<b>Выбран звук:</b>");
        ui->textEditOut->append(QDir::toNativeSeparators(
                                   QApplication::applicationDirPath() +
                                   "\\sounds\\" +
                                   currSound +
                                   ".wav"
                               )
                        );
    }
}

//void MainWindow::getLW(){
//    lwMW = qobject_cast<ListWindow*>(QObject::sender());
//}
//void MainWindow::getLHW(){
//    lhwMW = qobject_cast<ListHistWindow*>(QObject::sender());
//}
//void MainWindow::getCR(){
//    crMW = qobject_cast<CallRem*>(QObject::sender());
//}
//void MainWindow::hideEvent(QHideEvent *event){
//    this->hide();
//}

void MainWindow::closeEvent(QCloseEvent *event){
    QMessageBox::StandardButton ans;
    ans = QMessageBox::question(
        this,
        QApplication::applicationName(),
        tr("Вы действительно хотите выйти из программы?"),
        QMessageBox::Yes|QMessageBox::No,
        QMessageBox::No
    );

    if(ans == QMessageBox::No)
        event->ignore();
    if(ans == QMessageBox::Yes){
        emit closeAllOther();
        event->accept();
        QApplication::quit();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
