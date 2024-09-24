#include "acdisplay.h"
#include "QLibrary"
#include <QDebug>
#include <QTime>
#include "globals.h"

ACDisplay::ACDisplay(QObject *parent) : QObject(parent) {
    cd_map["posua_vfd"].fullname = "POSUA VFD Customer Display";
    cd_map["posua_vfd"].library = "LPOS_VFD_LCD.dll";
    cd_map["posua_vfd"].columns = 20;
    cd_map["posua_vfd"].rows = 2;
    cd_map["posua_vfd"].home_str = "home_str";
    cd_map["posua_vfd"].end_str = "end_str";
    cd_map["posua_vfd"].home_all = "home_all";
    cd_map["posua_vfd"].end_all = "end_all";
    cd_map["posua_vfd"].cursor_en = "cursor_en";
    cd_map["posua_vfd"].cursor_dis = "cursor_dis";
//    Function moveXY(Offs_X, Offs_Y: byte):integer;
    cd_map["posua_vfd"].overwrite = "overwrite";
    cd_map["posua_vfd"].horizontal = "horizontal";
    cd_map["posua_vfd"].vertical = "vertical";
//    Function send_hex(hex_data:byte):integer;
    cd_map["posua_vfd"].clear_disp = "clear_disp";
    cd_map["posua_vfd"].send_char = "send_char";
//    Function get_sym(bit_f: Pointer; font_val: byte):integer;
//    Function load_sym(bit_f: Pointer; font_val: byte):integer;
    cd_map["posua_vfd"].en_user_char = "en_user_char";
    cd_map["posua_vfd"].dis_user_char = "dis_user_char";
//    Function en_sym(font_val: byte):integer;
//    Function dis_sym(font_val: byte):integer;
//    Function font_set(font_lo, font_hi, EEPROM_en: byte):integer;
    cd_map["posua_vfd"].send_str = "send_str";
}

bool ACDisplay::setCurrent(QString name) {
    //    qDebug() << "ACDisplay::setCurrent" << name;
    if (cd_map.keys().contains(name)) {
        //        qDebug() << "a1";
        if (this->load(globals["library_path"]+cd_map[name].library)) {
            //            qDebug() << "a2";
            current_display = name;
            home_str = (P_base) lib.resolve(cd_map[current_display].home_str.toLatin1());
            end_str = (P_base) lib.resolve(cd_map[current_display].end_str.toLatin1());
            home_all = (P_base) lib.resolve(cd_map[current_display].home_all.toLatin1());
            end_all = (P_base) lib.resolve(cd_map[current_display].end_all.toLatin1());
            cursor_en = (P_base) lib.resolve(cd_map[current_display].cursor_en.toLatin1());
            cursor_dis = (P_base) lib.resolve(cd_map[current_display].cursor_dis.toLatin1());
            overwrite = (P_base) lib.resolve(cd_map[current_display].overwrite.toLatin1());
            horizontal = (P_base) lib.resolve(cd_map[current_display].horizontal.toLatin1());
            vertical = (P_base) lib.resolve(cd_map[current_display].vertical.toLatin1());
            clear_disp = (P_base) lib.resolve(cd_map[current_display].clear_disp.toLatin1()); //if (!clear_disp) {qWarning() << "Error clear_disp";} else {qDebug() << "all good";}
            en_user_char = (P_base) lib.resolve(cd_map[current_display].en_user_char.toLatin1());
            dis_user_char = (P_base) lib.resolve(cd_map[current_display].dis_user_char.toLatin1());
            send_char = (P_send_char) lib.resolve(cd_map[current_display].send_char.toLatin1());
            send_str = (P_send_str) lib.resolve(cd_map[current_display].send_str.toLatin1());
            return true;
        } else {
            qWarning() << "Error set customer display " << name << ". Can't load " << cd_map[name].library << "library.";
            return false;
        }
    }
    return true;
}

bool ACDisplay::load(QString library) {
    lib.setFileName(library);
    if(!lib.load())
        return false;
    else
        return true;
}

#include "string.h"
bool ACDisplay::echoString(QString str, QString align, int row) {
    int columns = cd_map[current_display].columns;
    int startpos=0;
//    qDebug() << str.length() << cd_map[current_display].columns << spaces;
//    qDebug() << "ACDisplay::echoString" << str << columns << str.length();
    if (columns>str.length()) {
        if (align=="center") {startpos = (columns - str.length())/2;}
        if (align=="right") {startpos = (columns - str.length());}
    }
//    qDebug() << "ACDisplay::echoString" << startpos;
    QByteArray ba = str.toLatin1();
    //55char chr[columns];
//    qDebug() << startpos;
    for  (int i=0;i<columns;i++) {
        if (i>=startpos && i<startpos+ba.count()) {
            //55chr[i] = ba.at(i-startpos);
        } else {
            //55chr[i] = ' ';
        }
    }
    //55send_str(&chr, row, sizeof(chr));
    return true;
}


////    QByteArray ba = str.toAscii();
////    qDebug() << "ba:" << ba;
//    QTime time;
//    for (int i=0;i<10;i++) {
////        qDebug() << i << ba[i];
//        QString a = "ап";
//        send_char(a.toAscii()[0]);
//        time.start();
//        for(;time.elapsed() < 100;){}
//    }
////    qDebug() << "a2";

