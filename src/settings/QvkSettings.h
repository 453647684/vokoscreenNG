#ifndef QVKSETTINGS_H 
#define QVKSETTINGS_H

#include "ui_formMainWindow.h"

#include <QObject>
#include <QSettings>
#include <QDebug>
#include <QMainWindow>

class QvkSettings: public QObject
{ 
    Q_OBJECT
public:
    QvkSettings();
    virtual ~QvkSettings();
    void saveAll(Ui_formMainWindow *ui_mainwindow, QMainWindow *parent );
    void readAll(Ui_formMainWindow *ui_mainwindow, QMainWindow *parent);

    QString getFileName();


public slots:


private slots:


signals:


protected:  


private:

};

#endif
