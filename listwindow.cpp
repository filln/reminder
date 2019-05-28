#include "listwindow.h"
#include "ui_listwindow.h"
#include "mainwindow.h"
#include <QMessageBox>

MainWindow* mwLW;

QDataStream& operator <<(QDataStream& qos, const MainWindow::Tdata& data);

ListWindow::ListWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ListWindow),
    cc(3), //текст, дата, время
    rowcl(-1), colcl(-1)
{
    ui->setupUi(this);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //запретить редактирование

//    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->tableWidget->setColumnCount(cc); //задать кол. колонок
    ui->tableWidget->horizontalHeader()->resizeSection(0, 200);
    ui->tableWidget->horizontalHeader()->resizeSection(1, 110);
    ui->tableWidget->horizontalHeader()->resizeSection(2, 60);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("Текст"));
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Дата"));
    ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("Время"));

    connect(ui->tableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(changCellKey()));
    connect(ui->tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(changCellClick(int, int)));

    connect(ui->pBDel, SIGNAL(clicked(bool)), this, SLOT(del())); //удалить текущую строку
    connect(ui->pBChange, SIGNAL(clicked(bool)), this, SLOT(openChange_Sl()));
}

void ListWindow::getMW(){
    mwLW = qobject_cast<MainWindow*>(QObject::sender());
}

void ListWindow::showList(){
    if(mwLW){
        rc = (mwLW->Vdata).size(); //кол. строк(напоминаний)
        ui->tableWidget->setRowCount(rc);
        for(int row = 0; row<rc; ++row)
            for(int col = 0; col<cc; ++col){
                QTableWidgetItem* item = new QTableWidgetItem();
                switch (col) {
                case 0:
                    item->setText((mwLW->Vdata)[row].text);
                    break;
                case 1:
                    item->setText((mwLW->Vdata)[row].date.toString("dd.MM.yyyy"));
                    break;
                case 2:
                    item->setText((mwLW->Vdata)[row].time.toString("hh:mm"));
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
        item->setText("mwLW == nullptr");
        ui->tableWidget->setItem(0, 0, item);
        this->show();
    }
}

void ListWindow::changCellClick(int row, int column){
    rowcl = ui->tableWidget->currentRow();
    colcl = ui->tableWidget->currentColumn();
//    qDebug()<<rowcl<<colcl;
}

void ListWindow::changCellKey(){
    rowcl = ui->tableWidget->currentRow();
    colcl = ui->tableWidget->currentColumn();
//    qDebug()<<rowcl<<colcl;
}

void ListWindow::del(){
    int sz = OpenRow.size();
    if(!ui->cBDel->isChecked() && mwLW && colcl>=0 && rowcl>=0 && rowcl<mwLW->Vdata.size()){
        //проверяем, редактируется ли ячейка
        bool isChange = false;
        if(sz)
            for(int i = 0; i<sz; ++i)
                if(rowcl == OpenRow[i]){
                    isChange = true;
                    break;
                }
        if(!isChange){
            //добавляем в базу истории нап.
            mwLW->VdataHist.push_back((mwLW->Vdata)[rowcl]);
            if(mwLW->VdataHist.size() > 1)
                std::sort(
                            (mwLW->VdataHist).begin(), (mwLW->VdataHist).end(),
                            [](const MainWindow::Tdata& a, const MainWindow::Tdata& b){
                    if(a.date < b.date) return true;
                    else if(a.date > b.date) return false;
                    else if(a.date == b.date && a.time < b.time) return true;
                    else if(a.date == b.date && a.time > b.time) return false;
                    else return false; //убирает предупреждение In lambda function: control reaches end of non-void function [-Wreturn-type]
                }
                );

            fileHist.setFileName(QDir::toNativeSeparators(QApplication::applicationDirPath() + "\\dataHist.reminder"));
            fileHist.open(QIODevice::WriteOnly);
            QDataStream outHist(&fileHist);
            outHist.setVersion(QDataStream::Qt_5_10);
            outHist<<mwLW->VdataHist;
            fileHist.close();

            //удаляем из основной базы
            if(mwLW->Vdata.size() > 1)
                mwLW->Vdata.remove(rowcl);
            else mwLW->Vdata.clear();
            file.setFileName(QDir::toNativeSeparators(QApplication::applicationDirPath() + "\\data.reminder"));
            file.open(QIODevice::WriteOnly); //пишется, затирая старые данные.
            QDataStream out(&file);
            out.setVersion(QDataStream::Qt_5_10);
            out<<mwLW->Vdata;
            file.close();

            ui->tableWidget->removeRow(rowcl); //при удалении последней ячейки будет rowcl = -1;
        }//if(!isChange)
        else{
            QMessageBox::warning(0,
                                 QApplication::applicationName(),
                                 "Перед удалением ячейки закройте окно просмотра и редактирования для нее!",
                                 QMessageBox::Ok);
        }
    } //if(!ui->cBDel->isChecked() && mwLW && colcl>=0 && rowcl>=0 && rowcl<mwLW->Vdata.size())

    if(ui->cBDel->isChecked() && !mwLW->Vdata.isEmpty()){
        if(!sz){
            for(int i = 0; i < mwLW->Vdata.size(); ++i)
                mwLW->VdataHist.push_back((mwLW->Vdata)[i]);
            if(mwLW->VdataHist.size() > 1)
                std::sort(
                            (mwLW->VdataHist).begin(), (mwLW->VdataHist).end(),
                            [](const MainWindow::Tdata& a, const MainWindow::Tdata& b){
                    if(a.date < b.date) return true;
                    else if(a.date > b.date) return false;
                    else if(a.date == b.date && a.time < b.time) return true;
                    else if(a.date == b.date && a.time > b.time) return false;
                    else return false; //убирает предупреждение In lambda function: control reaches end of non-void function [-Wreturn-type]
                }
                );

            fileHist.setFileName(QDir::toNativeSeparators(QApplication::applicationDirPath() + "\\dataHist.reminder"));
            fileHist.open(QIODevice::WriteOnly);
            QDataStream outHist(&fileHist);
            outHist.setVersion(QDataStream::Qt_5_10);
            outHist<<mwLW->VdataHist;
            fileHist.close();

            //удаляем из основной базы
            ui->tableWidget->clear(); //при удалении последней ячейки будет rowcl = -1;
            ui->tableWidget->setRowCount(0);
            mwLW->Vdata.clear();
            file.setFileName(QDir::toNativeSeparators(QApplication::applicationDirPath() + "\\data.reminder"));
            file.open(QIODevice::WriteOnly); //пишется, затирая старые данные.
            QDataStream out(&file);
            out.setVersion(QDataStream::Qt_5_10);
            out<<mwLW->Vdata;
            file.close();
            ui->cBDel->setCheckState(Qt::Unchecked);
        }//if(!sz)
        else{
            QMessageBox::warning(0,
                                 QApplication::applicationName(),
                                 "Перед удалением закройте все окна просмотра и редактирования!",
                                 QMessageBox::Ok);
        }
    } //if(ui->cBDel->isChecked() && !mwLW->Vdata.isEmpty())
    rowcl = colcl = -1; //для удаления необходимо снова выделить ячейку
} //void ListWindow::del()

void ListWindow::openChange_Sl(){
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

void ListWindow::clock(QString &clText){
    if(this->isVisible())
        ui->lbClock->setText(clText);
}

void ListWindow::closeEvent(QCloseEvent *event){
    emit closeAllOther();
    event->accept();
}

ListWindow::~ListWindow()
{
    delete ui;
}
