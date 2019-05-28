#include "tofile.h"
#include "ui_mainwindow.h"

ToFile::ToFile() : QObject() {}

QDataStream& operator <<(QDataStream& qos, const ToFile::Tdata& data){
    qos<<data.text<<data.date<<data.time;
    return qos;
}

void ToFile::HandlData(){
    data.text = ui->textEdit->toPlainText();
    data.date = ui->dateEdit->date();
    data.time = ui->timeEdit->time();
    Vdata.push_back(data);
    file.setFileName("data.reminder");
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_10);
    out<<Vdata;
    file.close();
}

ToFile::~ToFile(){}
