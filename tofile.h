#ifndef TOFILE_H
#define TOFILE_H

#include "mainwindow.h"
#include <QString>
#include <QFile>
#include <QDataStream>
#include <QDate>
#include <QTime>
#include <QVector>

class ToFile : public QObject
{
    Q_OBJECT

public:
    ToFile();
    struct Tdata{
        QString text;
        QDate date;
        QTime time;
    };
    friend QDataStream& operator <<(QDataStream& qos, const ToFile::Tdata& data);
    ~ToFile();
private:
    Tdata data;
    QFile file;
    QVector<Tdata> Vdata;
private slots:
    void HandlData();
};

#endif // TOFILE_H
