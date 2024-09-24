#ifndef ADATABASE_H
#define ADATABASE_H

#include <QObject>
#include <QMap>
#include <QtSql>
#include <QSqlQuery>
#include <QAbstractItemModel>
#include <QVariantList>
#include <QList>

class ADatabase: public QObject {
Q_OBJECT
public:
    explicit ADatabase(QObject *parent = 0);
    ~ADatabase();

    QString lasterror;
    QString queryList;
    QString connectionName;
    bool ifTransaction;
    //    QSqlDatabase db;
    //    QSqlQuery * queryTrans;

    static QString getConnectionName(QString conName);   // Восстанавливает имя соединения, в случае передачи неверного параметра
    static QString getConnectionDriver(QString conName); // Получает имя драйвера (QODBC3, SQLITE) указанного соединения, так как QSqlDatabase:database(name).drivername требует активного подключнея и, в случае отсутствия соединения вызывает паузу.

    static bool checkConnection(QString conName, QString sender="");                           // Проверяет соединение с БД, восстанавливает или создает его

    static bool pingSQLServer(QString conName);

    static QString createConnection(QString typeofdb="", QString name="");  // Создает именованое соединение с БД (QODBC3,QSQLITE)
    static bool openConnection(QString name);                               // [private] Открывает соединение с выбранной БД
    static bool closeConnection(QString name="");                           // Закрывает указанное соединение с БД

    static QSqlQuery * newSelectSql(QString querystring, QString conName="", QString sender="");
    static QSqlQueryModel * selectSql(QString querystring, QString conName="", QSqlQueryModel *qModel=new QSqlQueryModel());
    static int insertSql(QString querystring, QString conName="");
    //static QSqlQueryModel * selectSql(QString querystring, QSqlQueryModel *qModel=new QSqlQueryModel());
    //static QSqlQuery * execSql2(QString querystring, QSqlQuery *query, QString conName="");
    //QSqlQuery * execSqlTrans(QString querystring, QSqlQuery *query);

public slots:
    static bool execSql(QString querystring, QString conName="");

    static void beginTransaction(QString conName="");
    static void commitTransaction(QString conName="");
    static void rollbackTransaction(QString conName="");

    static QList<QString> selectFieldsList(QString querystring, QString conName="");
    static QSqlQueryModel * selectParams(QString dataSource, QString fields, QString filter = "");
    static QList<QMap<QString, QVariant> > selectList(QString querystring, QString conName="");
    static QVariantList selectVList(QString querystring);
    static QMap<int, QMap<QString, QVariant> > selectMap(QString querystring, QString conName="", QString sender=""); // int - id записи
    static QVariantMap selectVMap(QString querystring, QString keyfield="");
    //static QMap<QString, QVariant> selectRecord(QString querystring, QList<QString> fields=QList<QString>());

    static int insertParam(QString dataSource, QString fields, QString values, QString conName="");
    static bool updateParam(QString dataSource, QString update, QString conditions = "", QString conName="");
    static bool deleteParam(QString dataSource, QString conditions = "", QString conName="");

    //int insertParamTransaction(QString dataSource, QString fields, QString values, QString conName);
    //bool updateParamTransaction(QString dataSource, QString update, QString conditions, QString conName);
    //bool deleteParamTransaction(QString dataSource, QString conditions, QString conName);

    static QVariant DMin(QString field, QString dict, QString criteria, QString conName="");
    static QVariant DMax(QString field, QString dict, QString criteria, QString conName="");
    static QVariant DLookUpQuery(QString field, QString querystring, QString conName="", QString sender="");
    static QVariant DLookUp(QString field, QString dict, QString criteria, QString conName="", QString sender="");
    static QVariant DSum(QString field, QString dict, QString criteria, QString conName="");

    //static QString getDBServer();
    static bool checkTable(QString tableName, QString conName="");
    static QList<QVariant> getTableFields(QString tableName, QString conName="");

    static bool reindexTables();
    static void shrinkDb();

    static QString masterServerReplication(QString tablename);

    //static int lastInsertedKey(QString dataSource);
    void DebugCurrentStateOfNetworkConnections();
};

#endif  // ADATABASE_H
