/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Класс ADict реализовывает работу с одной таблицей-справочником.
/// Справочник это таблица, имеющая обязательное поле id (autoincrement, unique, not null)
/// Обязательные параметры:
///     - соединение
///     - имя таблицы
///
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ADICT_H
#define ADICT_H

#include "QVariant"

class ADict: public QObject {
Q_OBJECT
public:
    explicit ADict(QString dictName="", QString fields="", QObject *parent=0, QString conName="");
    ~ADict();


    QString lasterror;
    int currentRecordI;                     // Номер текущей записи (курсора) в стеке

private:
    QString currentTable;
    QList<QVariant> currentTableFields;     // Существующие поля таблицы БД
    QList< QMap<QString, QVariant> > stack; // Стек с несохраненными данными родительского справочника
    QList<int> stack_todelete;
    QString query_workfields;               // Строка с полями для запросов БД
    QString connection;

    //QMap<QString, QVariant> currentRecord;
    //void inStack(); // Заносит текущую запись в стек, очищает переменную currentRecord

public slots:
    void setCurrentConnection(QString conName);
    void clearStack();                                          // Очищает стек, создает первую запись
    bool setCurrentDict(QString dictName, QString fields = ""); // Устанавливает активную таблицу
    void setWorkFields(QString fields);                         // Формирование списка полей query_workfields для запросов БД, проверка наличия полей в currentTableFields

    void newRecord();                                           // Создает новую запись
    bool addRecord(int id);
    bool openRecord(int id);                                    // Загружает запись из рабочей таблицы

    bool setCurrentRecord(int i);                               // Устанавливает позицию i в стеке в качестве текущей
    bool findRecord(int id);

    int count();

    QMap<QString, QVariant> getCurrentRecord(int i=-1);

    QVariant getCRValue(QString key, int record_i=-1);
    void setCRValue(QString key, QVariant value);

    bool updateRecord(QMap<QString, QVariant> updateMap);       // Обновляет текущую запись
    bool deleteCurrent(int i=-1);                               // Очищает запись стека (-1 текущую) и переносит ее id в stack_todelete

    /*+*/

    void echoCR(int i=-1);
    void echoDict();

    // Работа со стеком
    /*+*/bool loadRecords(QString whereString="", QMap<QString, QVariant> whereMap = QMap<QString, QVariant>(), QString orderby=""); // Загружает записи из рабочей таблицы по условиям, заданым в where
    /*+*/bool saveChanges();                                    // Сохраняет изменения из stack в БД (если доступно id - обновляет)

};

#endif // ADICT_H
