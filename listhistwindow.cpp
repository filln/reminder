#include "listhistwindow.h"
#include "mainwindow.h"
#include "ui_listhistwindow.h"
#include "ui_mainwindow.h"
#include <QDate>
#include <QTime>
#include <QMessageBox>
#include <QDebug>

MainWindow *mwLHW;

QDataStream& operator <<(QDataStream& qos, const MainWindow::Tdata& data);

ListHistWindow::ListHistWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ListHistWindow),
    cc(3), //текст, дата, время
    rowcl(-1), colcl(-1)
{
//    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //запретить редактирование

    ui->tableWidget->setColumnCount(cc); //задать кол. колонок
    ui->tableWidget->horizontalHeader()->resizeSection(0, 200);
    ui->tableWidget->horizontalHeader()->resizeSection(1, 110);
    ui->tableWidget->horizontalHeader()->resizeSection(2, 60);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); //растянуть 0-е строки
    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("Текст"));
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Дата"));
    ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("Время"));

    connect(ui->tableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(changCellKey()));
    connect(ui->tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(changCellClick(int, int)));

    connect(ui->pBRest, SIGNAL(clicked(bool)), this, SLOT(restore())); //восстановить нап.
    connect(ui->pBDelHist, SIGNAL(clicked(bool)), this, SLOT(del())); //удалить текущую строку
    connect(ui->pBChange, SIGNAL(clicked(bool)), this, SLOT(openChange_Sl()));
}

void ListHistWindow::getMW(){
    mwLHW = qobject_cast<MainWindow*>(QObject::sender());
}

void ListHistWindow::showList(){
    if(mwLHW){
        rc = (mwLHW->VdataHist).size(); //кол. строк(напоминаний)
        ui->tableWidget->setRowCount(rc);
        for(int row = 0; row<rc; ++row)
            for(int col = 0; col<cc; ++col){
                QTableWidgetItem* item = new QTableWidgetItem();
                switch (col) {
                case 0:
                    item->setText((mwLHW->VdataHist)[row].text);
                    break;
                case 1:
                    item->setText((mwLHW->VdataHist)[row].date.toString("dd.MM.yyyy"));
                    break;
                case 2:
                    item->setText((mwLHW->VdataHist)[row].time.toString("hh:mm"));
                    break;
                default:
                    break;
                }
                ui->tableWidget->setItem(row, col, item);
            }
        this->show();
        rowcl = colcl = -1;
    }
    else{
        ui->tableWidget->setRowCount(1);
        QTableWidgetItem* item = new QTableWidgetItem();
        item->setText("mwLHW == nullptr");
        ui->tableWidget->setItem(0, 0, item);
        this->show();
    }
}

void ListHistWindow::changCellClick(int row, int column){
    rowcl = ui->tableWidget->currentRow();
    colcl = ui->tableWidget->currentColumn();
//    qDebug()<<rowcl<<colcl;
}

void ListHistWindow::changCellKey(){
    rowcl = ui->tableWidget->currentRow();
    colcl = ui->tableWidget->currentColumn();
//    qDebug()<<rowcl<<colcl;
}

void ListHistWindow::openChange_Sl(){
    if(rowcl >= 0 && colcl >= 0){
        //если окно "подробнее/изменить" для текущей строки не открыто сейчас, то открыть его
        bool isOpen = false;
        if(!OpenRow.isEmpty()){
            int sz = OpenRow.size();
            for(int i = 0; i < sz; ++i)
                if(OpenRow[i] == rowcl){
                    isOpen = true;
                    break;
                }
        }
        if(!isOpen){
            OpenRow.push_back(rowcl);
            emit openChange();
        }
    }
}

void ListHistWindow::restore(){
    int sz = OpenRow.size();
    if(mwLHW && colcl>=0 && rowcl>=0){
        //проверяем, редактируется ли ячейка
        bool isChange = false;
        if(sz)
            for(int i = 0; i < sz; ++i)
                if(rowcl == OpenRow[i]){
                    isChange = true;
                    break;
                }
        if(!isChange){
            mwLHW->ui->textEdit->setText(
                        ui->tableWidget->item(rowcl, 0)->text()
                        );
            mwLHW->ui->dateEdit->setDate(
                        QDate::fromString(
                            ui->tableWidget->item(rowcl, 1)->text(), "dd.MM.yyyy"
                            )
                        );
            mwLHW->ui->timeEdit->setTime(
                        QTime::fromString(
                            ui->tableWidget->item(rowcl, 2)->text(), "hh:mm"
                            )
                        );

            if(!mwLHW->VdataHist[rowcl].files.isEmpty()){
                if(!mwLHW->data.files.isEmpty()){
                    mwLHW->data.files.clear();
                    mwLHW->ui->textEditOut->append("<b>Выбраны файлы:</b> Нет.");
                }
                mwLHW->data.files = mwLHW->VdataHist[rowcl].files;

                mwLHW->ui->textEditOut->append("<b>Выбраны файлы:</b>");
                int sz = mwLHW->data.files.size();
                for(int i = 0; i < sz; ++i)
                    mwLHW->ui->textEditOut->append(
                                QDir::toNativeSeparators(
                                    (mwLHW->data.files)[i]
                                    )
                                );
            }
            if(!mwLHW->VdataHist[rowcl].dirs.isEmpty()){
                if(!mwLHW->data.dirs.isEmpty()){
                    mwLHW->data.dirs.clear();
                    mwLHW->ui->textEditOut->append("<b>Выбраны каталоги:</b> Нет.");
                }
                mwLHW->data.dirs = mwLHW->VdataHist[rowcl].dirs;

                mwLHW->ui->textEditOut->append("<b>Выбраны каталоги:</b>");
                int sz = mwLHW->data.dirs.size();
                for(int i = 0; i < sz; ++i)
                    mwLHW->ui->textEditOut->append(
                                QDir::toNativeSeparators(
                                    (mwLHW->data.dirs)[i]
                                    )
                                );
            }

            emit addData();

            ui->cBDel->setCheckState(Qt::Unchecked);
            del();
        } //if(!isChange)
        else{
            QMessageBox::warning(0,
                                 QApplication::applicationName(),
                                 "Перед восстановлением ячейки закройте окно просмотра для нее!",
                                 QMessageBox::Ok);
        }
    } //if(mwLHW && colcl>=0 && rowcl>=0)
} //void ListHistWindow::restore()

void ListHistWindow::del(){
    int sz = OpenRow.size();
    if(!ui->cBDel->isChecked() && mwLHW && colcl>=0 && rowcl>=0 && rowcl<mwLHW->VdataHist.size()){
        //проверяем, редактируется ли ячейка
        bool isChange = false;
        if(sz)
            for(int i = 0; i<sz; ++i)
                if(rowcl == OpenRow[i]){
                    isChange = true;
                    break;
                }
        if(!isChange){
            //удаляем из базы истории
            if(mwLHW->VdataHist.size() > 1)
                mwLHW->VdataHist.remove(rowcl);
            else mwLHW->VdataHist.clear();
            fileHist.setFileName(QDir::toNativeSeparators(QApplication::applicationDirPath() + "\\dataHist.reminder"));
            fileHist.open(QIODevice::WriteOnly); //пишется, затирая старые данные.
            QDataStream outHist(&fileHist);
            outHist.setVersion(QDataStream::Qt_5_10);
            outHist<<mwLHW->VdataHist;
            fileHist.close();
            ui->tableWidget->removeRow(rowcl); //при удалении последней ячейки будет rowcl = -1;
        } //if(!isChange)
        else{
            QMessageBox::warning(0,
                                 QApplication::applicationName(),
                                 "Перед удалением ячейки закройте окно просмотра для нее!",
                                 QMessageBox::Ok);

        }
    } //if(!ui->cBDel->isChecked() && mwLHW && colcl>=0 && rowcl>=0 && rowcl<mwLHW->VdataHist.size())
    if(ui->cBDel->isChecked() && !mwLHW->VdataHist.isEmpty()){
        if(!sz){
            //удаляем из базы истории
            ui->tableWidget->clear(); //при удалении последней ячейки будет rowcl = -1;
            ui->tableWidget->setRowCount(0);
            mwLHW->VdataHist.clear();
            fileHist.setFileName(QDir::toNativeSeparators(QApplication::applicationDirPath() + "\\dataHist.reminder"));
            fileHist.open(QIODevice::WriteOnly); //пишется, затирая старые данные.
            QDataStream outHist(&fileHist);
            outHist.setVersion(QDataStream::Qt_5_10);
            outHist<<mwLHW->VdataHist;
            fileHist.close();
            ui->cBDel->setCheckState(Qt::Unchecked);
        } //if(!sz)
        else{
            QMessageBox::warning(0,
                                 QApplication::applicationName(),
                                 "Перед удалением закройте все окна просмотра!",
                                 QMessageBox::Ok);
        }
    } //if(ui->cBDel->isChecked() && !mwLHW->VdataHist.isEmpty())
    rowcl = colcl = -1; //для удаления необходимо снова выделить ячейку
} //void ListHistWindow::del()

void ListHistWindow::clock(QString &clText){
    if(this->isVisible())
        ui->lbClock->setText(clText);
}

void ListHistWindow::closeEvent(QCloseEvent *event){
    emit closeAllOther();
    event->accept();
}

ListHistWindow::~ListHistWindow()
{
    delete ui;
}
