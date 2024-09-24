/********************************************   ********************************
**
** Metadata for AcroPDFLib generated by dumpcpp from type library
** C:\Program Files\Common Files\Adobe\Acrobat\ActiveX\AcroPDF.dll
**
****************************************************************************/

#define QAX_DUMPCPP_ACROPDFLIB_NOINLINES
#include "-acropdflib.h"

using namespace AcroPDFLib;

static const uint qt_meta_data_AcroPDFLib__AcroPDF[] = {

 // content:
       1,       // revision
       0,       // classname
       2,    10, // classinfo
       36,    14, // methods
       2,    194, // properties
       0,    0, // enums/sets

 // classinfo: key, value
       20, 38, 
       60, 72, 

 // signals: signature, parameters, type, tag, flags
       87, 97, 98, 99, 5,
       100, 112, 113, 114, 5,
       115, 154, 176, 177, 5,
       178, 203, 208, 209, 5,
       210, 236, 251, 252, 5,

 // slots: signature, parameters, type, tag, flags
       253, 267, 268, 277, 9,
       278, 296, 305, 310, 9,
       311, 319, 320, 321, 9,
       322, 344, 353, 354, 9,
       355, 373, 374, 375, 9,
       376, 393, 394, 395, 9,
       396, 412, 413, 414, 9,
       415, 430, 431, 432, 9,
       433, 448, 449, 450, 9,
       451, 470, 471, 472, 9,
       473, 495, 504, 505, 9,
       506, 517, 518, 519, 9,
       520, 538, 550, 551, 9,
       552, 572, 580, 581, 9,
       582, 610, 630, 631, 9,
       632, 650, 651, 652, 9,
       653, 690, 698, 699, 9,
       700, 738, 746, 747, 9,
       748, 768, 770, 771, 9,
       772, 795, 806, 807, 9,
       808, 836, 851, 852, 9,
       853, 875, 885, 886, 9,
       887, 908, 917, 918, 9,
       919, 943, 946, 947, 9,
       948, 969, 972, 973, 9,
       974, 990, 994, 995, 9,
       996, 1013, 1022, 1023, 9,
       1024, 1065, 1087, 1088, 9,
       1089, 1119, 1135, 1136, 9,
       1137, 1153, 1161, 1162, 9,
       1163, 1199, 1216, 1217, 9,

 // properties: name, type, flags
       1218, 1233, 0xff015003, 		 // QVariant messageHandler
       1242, 1246, 0x0a015003, 		 // QString src

        0        // eod
};

static const char *qt_meta_stringdata_AcroPDFLib__AcroPDF() {
    static const char stringdata0[] = {
    "AcroPDFLib::AcroPDF\0"
    "Event Interface 1\0_IAcroAXDocShimEvents\0Interface 0\0IAcroAXDocShim\0"
    "OnError()\0\0\0\0OnMessage()\0\0\0\0exception(int,QString,QString,QString)\0code,source,disc,help\0\0\0propertyChanged(QString)\0name\0\0\0signal(QString,int,void*)\0"
    "name,argc,argv\0\0\0"
    "GetVersions()\0\0QVariant\0\0LoadFile(QString)\0fileName\0bool\0\0Print()\0\0\0\0execCommand(QVariant)\0strArray\0\0\0goBackwardStack()\0\0\0\0goForwardStack()\0\0\0\0gotoFirstPage()\0\0\0\0gotoLastPage()\0\0\0\0gotoNextPage()\0"
    "\0\0\0gotoPreviousPage()\0\0\0\0postMessage(QVariant)\0strArray\0\0\0printAll()\0\0\0\0printAllFit(bool)\0shrinkToFit\0\0\0printPages(int,int)\0from,to\0\0\0printPagesFit(int,int,bool)\0from,to,shrinkToFit\0\0\0printWithDialog()\0"
    "\0\0\0setCurrentHighlight(int,int,int,int)\0a,b,c,d\0\0\0setCurrentHightlight(int,int,int,int)\0a,b,c,d\0\0\0setCurrentPage(int)\0n\0\0\0setLayoutMode(QString)\0layoutMode\0\0\0setMessageHandler(QVariant)\0messageHandler\0"
    "\0\0setNamedDest(QString)\0namedDest\0\0\0setPageMode(QString)\0pageMode\0\0\0setShowScrollbars(bool)\0On\0\0\0setShowToolbar(bool)\0On\0\0\0setSrc(QString)\0src\0\0\0setView(QString)\0viewMode\0\0\0setViewRect(double,double,double,double)\0"
    "left,top,width,height\0\0\0setViewScroll(QString,double)\0viewMode,offset\0\0\0setZoom(double)\0percent\0\0\0setZoomScroll(double,double,double)\0percent,left,top\0\0\0"
    "messageHandler\0QVariant\0src\0QString\0"
    };
    static char data[sizeof(stringdata0) + 0];
    if (!data[0]) {
        int index = 0;
        memcpy(data + index, stringdata0, sizeof(stringdata0) - 1);
        index += sizeof(stringdata0) - 1;
    }

    return data;
};

const QMetaObject AcroPDF::staticMetaObject = {
{ &QWidget::staticMetaObject,
qt_meta_stringdata_AcroPDFLib__AcroPDF(),
qt_meta_data_AcroPDFLib__AcroPDF }
};

void *AcroPDF::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AcroPDFLib__AcroPDF()))
        return static_cast<void*>(const_cast<AcroPDF*>(this));
    return QAxWidget::qt_metacast(_clname);
}

static const uint qt_meta_data_AcroPDFLib__IAcroAXDocShim[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       34,    10, // methods
       3,    180, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
       27, 66, 88, 89, 5,
       90, 115, 120, 121, 5,
       122, 148, 163, 164, 5,

 // slots: signature, parameters, type, tag, flags
       165, 179, 180, 189, 9,
       190, 208, 217, 222, 9,
       223, 231, 232, 233, 9,
       234, 256, 265, 266, 9,
       267, 285, 286, 287, 9,
       288, 305, 306, 307, 9,
       308, 324, 325, 326, 9,
       327, 342, 343, 344, 9,
       345, 360, 361, 362, 9,
       363, 382, 383, 384, 9,
       385, 407, 416, 417, 9,
       418, 429, 430, 431, 9,
       432, 450, 462, 463, 9,
       464, 484, 492, 493, 9,
       494, 522, 542, 543, 9,
       544, 562, 563, 564, 9,
       565, 602, 610, 611, 9,
       612, 650, 658, 659, 9,
       660, 680, 682, 683, 9,
       684, 707, 718, 719, 9,
       720, 748, 763, 764, 9,
       765, 787, 797, 798, 9,
       799, 820, 829, 830, 9,
       831, 855, 858, 859, 9,
       860, 881, 884, 885, 9,
       886, 902, 906, 907, 9,
       908, 925, 934, 935, 9,
       936, 977, 999, 1000, 9,
       1001, 1031, 1047, 1048, 9,
       1049, 1065, 1073, 1074, 9,
       1075, 1111, 1128, 1129, 9,

 // properties: name, type, flags
       1130, 1138, 0x0a055003, 		 // QString control
       1146, 1161, 0xff015003, 		 // QVariant messageHandler
       1170, 1174, 0x0a015003, 		 // QString src

        0        // eod
};

static const char *qt_meta_stringdata_AcroPDFLib__IAcroAXDocShim() {
    static const char stringdata0[] = {
    "AcroPDFLib::IAcroAXDocShim\0"
    "exception(int,QString,QString,QString)\0code,source,disc,help\0\0\0propertyChanged(QString)\0name\0\0\0signal(QString,int,void*)\0name,argc,argv\0\0\0"
    "GetVersions()\0\0QVariant\0\0LoadFile(QString)\0fileName\0bool\0\0Print()\0"
    "\0\0\0execCommand(QVariant)\0strArray\0\0\0goBackwardStack()\0\0\0\0goForwardStack()\0\0\0\0gotoFirstPage()\0\0\0\0gotoLastPage()\0\0\0\0gotoNextPage()\0\0\0\0gotoPreviousPage()\0\0\0\0postMessage(QVariant)\0strArray\0\0\0printAll()\0\0\0\0"
    "printAllFit(bool)\0shrinkToFit\0\0\0printPages(int,int)\0from,to\0\0\0printPagesFit(int,int,bool)\0from,to,shrinkToFit\0\0\0printWithDialog()\0\0\0\0setCurrentHighlight(int,int,int,int)\0a,b,c,d\0\0\0setCurrentHightlight(int,int,int,int)\0"
    "a,b,c,d\0\0\0setCurrentPage(int)\0n\0\0\0setLayoutMode(QString)\0layoutMode\0\0\0setMessageHandler(QVariant)\0messageHandler\0\0\0setNamedDest(QString)\0namedDest\0\0\0setPageMode(QString)\0pageMode\0\0\0setShowScrollbars(bool)\0"
    "On\0\0\0setShowToolbar(bool)\0On\0\0\0setSrc(QString)\0src\0\0\0setView(QString)\0viewMode\0\0\0setViewRect(double,double,double,double)\0left,top,width,height\0\0\0setViewScroll(QString,double)\0viewMode,offset\0\0\0setZoom(double)\0"
    "percent\0\0\0setZoomScroll(double,double,double)\0percent,left,top\0\0\0"
    "control\0QString\0messageHandler\0QVariant\0src\0QString\0"
    };
    static char data[sizeof(stringdata0) + 0];
    if (!data[0]) {
        int index = 0;
        memcpy(data + index, stringdata0, sizeof(stringdata0) - 1);
        index += sizeof(stringdata0) - 1;
    }

    return data;
};

const QMetaObject IAcroAXDocShim::staticMetaObject = {
{ &QObject::staticMetaObject,
qt_meta_stringdata_AcroPDFLib__IAcroAXDocShim(),
qt_meta_data_AcroPDFLib__IAcroAXDocShim }
};

void *IAcroAXDocShim::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AcroPDFLib__IAcroAXDocShim()))
        return static_cast<void*>(const_cast<IAcroAXDocShim*>(this));
    return QAxObject::qt_metacast(_clname);
}

static const uint qt_meta_data_AcroPDFLib__AdobeSPOpenDocuments[] = {

 // content:
       1,       // revision
       0,       // classname
       1,    10, // classinfo
       26,    12, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // classinfo: key, value
       33, 45, 

 // signals: signature, parameters, type, tag, flags
       62, 101, 123, 124, 5,
       125, 150, 155, 156, 5,
       157, 183, 198, 199, 5,

 // slots: signature, parameters, type, tag, flags
       200, 237, 285, 290, 11,
       291, 333, 395, 400, 9,
       401, 438, 481, 486, 11,
       487, 533, 586, 591, 9,
       592, 627, 672, 677, 9,
       678, 725, 776, 781, 9,
       782, 812, 836, 841, 9,
       842, 864, 885, 890, 11,
       891, 922, 953, 958, 9,
       959, 993, 1020, 1025, 11,
       1026, 1069, 1106, 1111, 9,
       1112, 1151, 1192, 1197, 11,
       1198, 1246, 1297, 1302, 9,
       1303, 1340, 1380, 1381, 9,
       1382, 1403, 1404, 1409, 9,
       1410, 1432, 1453, 1458, 11,
       1459, 1490, 1521, 1526, 9,
       1527, 1561, 1588, 1593, 11,
       1594, 1637, 1674, 1679, 9,
       1680, 1718, 1754, 1759, 11,
       1760, 1807, 1853, 1858, 9,
       1859, 1920, 1968, 1969, 11,
       1970, 2040, 2098, 2099, 9,

        0        // eod
};

static const char *qt_meta_stringdata_AcroPDFLib__AdobeSPOpenDocuments() {
    static const char stringdata0[] = {
    "AcroPDFLib::AdobeSPOpenDocuments\0"
    "Interface 0\0ISPOpenDocuments\0"
    "exception(int,QString,QString,QString)\0code,source,disc,help\0\0\0propertyChanged(QString)\0name\0\0\0signal(QString,int,void*)\0name,argc,argv\0\0\0"
    "CheckinDocument(QString,int,QString)\0"
    "bstrDocumentLocation,CheckinType,CheckinComment\0bool\0\0CheckinDocument(QString,int,QString,bool)\0bstrDocumentLocation,CheckinType,CheckinComment,bKeepCheckout\0bool\0\0CheckoutDocumentPrompt(QString,bool)\0"
    "bstrDocumentLocationRaw,fEditAfterCheckout\0bool\0\0CheckoutDocumentPrompt(QString,bool,QVariant)\0bstrDocumentLocationRaw,fEditAfterCheckout,varProgID\0bool\0\0CreateNewDocument(QString,QString)\0bstrTemplateLocation,bstrDefaultSaveLocation\0"
    "bool\0\0CreateNewDocument2(IDispatch*,QString,QString)\0pDisp,bstrTemplateLocation,bstrDefaultSaveLocation\0bool\0\0DiscardLocalCheckout(QString)\0bstrDocumentLocationRaw\0bool\0\0EditDocument(QString)\0bstrDocumentLocation\0"
    "bool\0\0EditDocument(QString,QVariant)\0bstrDocumentLocation,varProgID\0bool\0\0EditDocument2(IDispatch*,QString)\0pDisp,bstrDocumentLocation\0bool\0\0EditDocument2(IDispatch*,QString,QVariant)\0pDisp,bstrDocumentLocation,varProgID\0"
    "bool\0\0EditDocument3(IDispatch*,QString,bool)\0pDisp,bstrDocumentLocation,fUseLocalCopy\0bool\0\0EditDocument3(IDispatch*,QString,bool,QVariant)\0pDisp,bstrDocumentLocation,fUseLocalCopy,varProgID\0bool\0\0NewBlogPost(QString,QString,QString)\0"
    "bstrProviderId,bstrBlogUrl,bstrBlogName\0\0\0PromptedOnLastOpen()\0\0bool\0\0ViewDocument(QString)\0bstrDocumentLocation\0bool\0\0ViewDocument(QString,QVariant)\0bstrDocumentLocation,varProgID\0bool\0\0ViewDocument2(IDispatch*,QString)\0"
    "pDisp,bstrDocumentLocation\0bool\0\0ViewDocument2(IDispatch*,QString,QVariant)\0pDisp,bstrDocumentLocation,varProgID\0bool\0\0ViewDocument3(IDispatch*,QString,int)\0pDisp,bstrDocumentLocation,OpenType\0bool\0\0ViewDocument3(IDispatch*,QString,int,QVariant)\0"
    "pDisp,bstrDocumentLocation,OpenType,varProgID\0bool\0\0ViewInExcel(QString,QString,QString,QString,QString,int,int)\0SiteUrl,fileName,SessionId,Cmd,Sheet,Row,Column\0\0\0ViewInExcel(QString,QString,QString,QString,QString,int,int,QVariant)\0"
    "SiteUrl,fileName,SessionId,Cmd,Sheet,Row,Column,varProgID\0\0\0"
    };
    static char data[sizeof(stringdata0) + 0];
    if (!data[0]) {
        int index = 0;
        memcpy(data + index, stringdata0, sizeof(stringdata0) - 1);
        index += sizeof(stringdata0) - 1;
    }

    return data;
};

const QMetaObject AdobeSPOpenDocuments::staticMetaObject = {
{ &QObject::staticMetaObject,
qt_meta_stringdata_AcroPDFLib__AdobeSPOpenDocuments(),
qt_meta_data_AcroPDFLib__AdobeSPOpenDocuments }
};

void *AdobeSPOpenDocuments::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AcroPDFLib__AdobeSPOpenDocuments()))
        return static_cast<void*>(const_cast<AdobeSPOpenDocuments*>(this));
    return QAxObject::qt_metacast(_clname);
}

static const uint qt_meta_data_AcroPDFLib__ISPOpenDocuments[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       26,    10, // methods
       1,    140, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
       29, 68, 90, 91, 5,
       92, 117, 122, 123, 5,
       124, 150, 165, 166, 5,

 // slots: signature, parameters, type, tag, flags
       167, 204, 252, 257, 11,
       258, 300, 362, 367, 9,
       368, 405, 448, 453, 11,
       454, 500, 553, 558, 9,
       559, 594, 639, 644, 9,
       645, 692, 743, 748, 9,
       749, 779, 803, 808, 9,
       809, 831, 852, 857, 11,
       858, 889, 920, 925, 9,
       926, 960, 987, 992, 11,
       993, 1036, 1073, 1078, 9,
       1079, 1118, 1159, 1164, 11,
       1165, 1213, 1264, 1269, 9,
       1270, 1307, 1347, 1348, 9,
       1349, 1370, 1371, 1376, 9,
       1377, 1399, 1420, 1425, 11,
       1426, 1457, 1488, 1493, 9,
       1494, 1528, 1555, 1560, 11,
       1561, 1604, 1641, 1646, 9,
       1647, 1685, 1721, 1726, 11,
       1727, 1774, 1820, 1825, 9,
       1826, 1887, 1935, 1936, 11,
       1937, 2007, 2065, 2066, 9,

 // properties: name, type, flags
       2067, 2075, 0x0a055003, 		 // QString control

        0        // eod
};

static const char *qt_meta_stringdata_AcroPDFLib__ISPOpenDocuments() {
    static const char stringdata0[] = {
    "AcroPDFLib::ISPOpenDocuments\0"
    "exception(int,QString,QString,QString)\0code,source,disc,help\0\0\0propertyChanged(QString)\0name\0\0\0signal(QString,int,void*)\0name,argc,argv\0\0\0"
    "CheckinDocument(QString,int,QString)\0bstrDocumentLocation,CheckinType,CheckinComment\0"
    "bool\0\0CheckinDocument(QString,int,QString,bool)\0bstrDocumentLocation,CheckinType,CheckinComment,bKeepCheckout\0bool\0\0CheckoutDocumentPrompt(QString,bool)\0bstrDocumentLocationRaw,fEditAfterCheckout\0bool\0"
    "\0CheckoutDocumentPrompt(QString,bool,QVariant)\0bstrDocumentLocationRaw,fEditAfterCheckout,varProgID\0bool\0\0CreateNewDocument(QString,QString)\0bstrTemplateLocation,bstrDefaultSaveLocation\0bool\0\0CreateNewDocument2(IDispatch*,QString,QString)\0"
    "pDisp,bstrTemplateLocation,bstrDefaultSaveLocation\0bool\0\0DiscardLocalCheckout(QString)\0bstrDocumentLocationRaw\0bool\0\0EditDocument(QString)\0bstrDocumentLocation\0bool\0\0EditDocument(QString,QVariant)\0bstrDocumentLocation,varProgID\0"
    "bool\0\0EditDocument2(IDispatch*,QString)\0pDisp,bstrDocumentLocation\0bool\0\0EditDocument2(IDispatch*,QString,QVariant)\0pDisp,bstrDocumentLocation,varProgID\0bool\0\0EditDocument3(IDispatch*,QString,bool)\0pDisp,bstrDocumentLocation,fUseLocalCopy\0"
    "bool\0\0EditDocument3(IDispatch*,QString,bool,QVariant)\0pDisp,bstrDocumentLocation,fUseLocalCopy,varProgID\0bool\0\0NewBlogPost(QString,QString,QString)\0bstrProviderId,bstrBlogUrl,bstrBlogName\0\0\0PromptedOnLastOpen()\0"
    "\0bool\0\0ViewDocument(QString)\0bstrDocumentLocation\0bool\0\0ViewDocument(QString,QVariant)\0bstrDocumentLocation,varProgID\0bool\0\0ViewDocument2(IDispatch*,QString)\0pDisp,bstrDocumentLocation\0bool\0\0ViewDocument2(IDispatch*,QString,QVariant)\0"
    "pDisp,bstrDocumentLocation,varProgID\0bool\0\0ViewDocument3(IDispatch*,QString,int)\0pDisp,bstrDocumentLocation,OpenType\0bool\0\0ViewDocument3(IDispatch*,QString,int,QVariant)\0pDisp,bstrDocumentLocation,OpenType,varProgID\0"
    "bool\0\0ViewInExcel(QString,QString,QString,QString,QString,int,int)\0SiteUrl,fileName,SessionId,Cmd,Sheet,Row,Column\0\0\0ViewInExcel(QString,QString,QString,QString,QString,int,int,QVariant)\0SiteUrl,fileName,SessionId,Cmd,Sheet,Row,Column,varProgID\0"
    "\0\0"
    "control\0QString\0"
    };
    static char data[sizeof(stringdata0) + 0];
    if (!data[0]) {
        int index = 0;
        memcpy(data + index, stringdata0, sizeof(stringdata0) - 1);
        index += sizeof(stringdata0) - 1;
    }

    return data;
};

const QMetaObject ISPOpenDocuments::staticMetaObject = {
{ &QObject::staticMetaObject,
qt_meta_stringdata_AcroPDFLib__ISPOpenDocuments(),
qt_meta_data_AcroPDFLib__ISPOpenDocuments }
};

void *ISPOpenDocuments::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AcroPDFLib__ISPOpenDocuments()))
        return static_cast<void*>(const_cast<ISPOpenDocuments*>(this));
    return QAxObject::qt_metacast(_clname);
}

