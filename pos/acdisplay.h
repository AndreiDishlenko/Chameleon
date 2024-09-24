#ifndef ACDISPLAY_H
#define ACDISPLAY_H

#include <QObject>
#include <QMap>
#include <QLibrary>

namespace cd_params {
    class myclass {
        public:
            QString fullname;
            QString library;
            int columns;
            int rows;

            QString home_str;
            QString end_str;
            QString home_all;
            QString end_all;
            QString cursor_en;
            QString cursor_dis;
            QString overwrite;
            QString horizontal;
            QString vertical;
            QString clear_disp;
            QString en_user_char;
            QString dis_user_char;
            QString send_char;
            QString send_str;
        };
}

//using namespace cd_params;
typedef QMap<QString, QString> (*mytype)(void);

class ACDisplay : public QObject
{
    Q_OBJECT
public:
    explicit ACDisplay(QObject *parent = 0);


    typedef long (*P_base)(void);
    P_base home_str;
    P_base end_str;
    P_base home_all;
    P_base end_all;
    P_base cursor_en;
    P_base cursor_dis;
    P_base overwrite;
    P_base horizontal;
    P_base vertical;
    P_base clear_disp;
    P_base en_user_char;
    P_base dis_user_char;


    typedef long (*P_send_char)(char);
    P_send_char send_char;

    typedef long (*P_send_str)(void* str, char row, int len);
    P_send_str send_str;


private:
    QMap<QString, cd_params::myclass> cd_map;
    QString current_display;

    QLibrary lib;

    bool load(QString library);


signals:

public slots:
    bool setCurrent(QString type);
    bool echoString(QString str, QString align, int row);
};

#endif // ACDISPLAY_H
