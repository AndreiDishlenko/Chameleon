#ifndef ACOMBOCHECKBOX_H
#define ACOMBOCHECKBOX_H

#include <QObject>
#include <QWidget>
#include <QComboBox>
#include <QListView>

#include <QStandardItemModel>

class AComboCheckBox : public QComboBox {
    Q_OBJECT
public:
    explicit AComboCheckBox(QWidget *parent = 0);
    void showPopup();
    void hidePopup();
    bool addItem( QString value, QString key);

    QList<QVariant> getSelected();
    void selectAllItems();

    QStandardItemModel *model;
    QListView *m_listView;
    bool m_opening;

    bool eventFilter(QObject * watched, QEvent * event);

signals:
    void opened();
    void closed();

public slots:

};


#endif // ACOMBOCHECKBOX_H
