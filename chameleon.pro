#-------------------------------------------------
#
# Project created by QtCreator 2010-10-05T23:23:11
# v4.8.7 --> 5.5 (steel QWebView)  --> 5.8 (steel free msvc, QWebEngine, QChart)
# 4.4 32-it  -->
#
#-------------------------------------------------


# Correct compiling is provided by
#   1. compiling with QT with openSSL support QSslSocket::supportsSsl()
#   2. Installing OpenSSL (version must be the same as QSslSocket::sslLibraryBuildVersionString())
#   3. PATH must include variable like C:\Qt\Tools\OpenSSL_102-Win32\bin\
#   4. Including this directives:

#win32: LIBS += -llibeay32
#win32: LIBS += -lssleay32

#CONFIG(release, debug|release): openssl.path = $$OUT_PWD/release
#else:CONFIG(debug, debug|release): openssl.path = $$OUT_PWD/debug
#openssl.files += C:/Qt/Tools/OpenSSL-Win32/bin/*.dll

#INSTALLS += openssl

#dlltarget.path = C:/Qt/Tools/OpenSSL-Win32/bin/
#INSTALLS += dlltarget

#LIBS += -LC:/Qt/Tools/OpenSSL-Win32/bin/ -llibeay32 - lssleay32
#LIBS += C:/Qt/Tools/OpenSSL_102-Win32/lib/libcrypto.lib
#LIBS += C:/Qt/Tools/OpenSSL_102-Win32/lib/libssl.lib
#INCLUDEPATH += C:\Qt\Tools\OpenSSL_111l-Win32\include\



QT       += core gui
QT       += script
QT       += scripttools
QT       += sql
QT       += network
QT       += printsupport
QT       += network
QT       += widgets
QT       += xml
QT       += webenginewidgets

TARGET   = chameleon

CONFIG   += app_bundle
CONFIG   -= console
CONFIG   += c++15

QT       += axcontainer
QT       += uitools

//CONFIG  += resources_big

TEMPLATE = app

SOURCES += main.cpp \
    aapplication.cpp \
    agui.cpp \
    adatabase.cpp \
    aengine.cpp \
    ajson.cpp \
    aprro.cpp \
    forms/atablewidgetstatic.cpp \
    forms/atreewidget.cpp \
    forms/awebwidget.cpp \
    globals.cpp \
    afunc.cpp \
    adataset.cpp \
    aprinter.cpp \
    adict.cpp \
    igui.cpp \
    aform.cpp \
    docs/aproduction.cpp \
    docs/adoc.cpp \
    docs/abuild.cpp \
    forms/atreeview.cpp \
    forms/atablewidget.cpp \
    media/aimage.cpp \
    pos/apos.cpp \
    pos/amap.cpp \
    pos/adiscount.cpp \
    pos/acdisplay.cpp \
    secure/asecure.cpp \
    secure/alicense.cpp \
    forms/adelegate.cpp \
    media/amplayer.cpp \
    media/aimagearea.cpp \
    aservice.cpp \
    media/avideo.cpp \
    media/apush.cpp \
    avariant.cpp \
    docs/awires.cpp \
    forms/acombocheckbox.cpp \
    aexportxml.cpp \
    stylehelper.cpp

HEADERS += aapplication.h \
    agui.h \
    adatabase.h \
    aform.h \
    aengine.h \
    ajson.h \
    aprro.h \
    forms/atablewidgetstatic.h \
    forms/atreewidget.h \
    forms/awebwidget.h \
    globals.h \
    afunc.h \
    adataset.h \
    aprinter.h \
    adict.h \
    igui.h \
    grdapi.h \
    docs/aproduction.h \
    docs/adoc.h \
    docs/abuild.h \
    forms/atreeview.h \
    forms/atablewidget.h \
    media/aimage.h \
    pos/apos.h \
    pos/amap.h \
    pos/adiscount.h \
    pos/acdisplay.h \
    secure/asecure.h \
    secure/alicense.h \
    forms/adelegate.h \
    media/amplayer.h \
    media/aimagearea.h \
    aservice.h \
    media/avideo.h \
    media/apush.h \
    avariant.h \
    docs/awires.h \
    forms/acombocheckbox.h \
    aexportxml.h \
    stylehelper.h

FORMS += uimainwindow.ui \
    form.ui \
    uimainwindow_info.ui \
    uimainwindow_posadmin.ui \
    uionewidget.ui \
    uistartscreen.ui

LIBS += -Lc:/qtp/chameleon_062 -lgrdapi32

INCLUDEPATH += c:/qtp/chameleon_062

DESTDIR += c:/Chameleon_dev/

RESOURCES += \
    resources.qrc

RC_FILE = myproject.rc
