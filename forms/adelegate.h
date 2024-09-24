#ifndef ADELEGATE_H
#define ADELEGATE_H

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QSpinBox>

#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QDateEdit>
#include <QPushButton>
#include <QTableWidget>
#include "atablewidget.h"

class ADelegate : public QItemDelegate
{
    Q_OBJECT

private:
        QString typeeditor;
        QLineEdit *led;
        QComboBox *cbb;
        QCheckBox *chb;
        QDateEdit *de;
        QTimeEdit *te;
        QPushButton *pbt;
        QColor color;
        bool enabled;

private slots:
    void commitAndCloseEditor(QWidget *wgt=0);

protected:
    bool eventFilter(QObject *obj, QEvent *ev);

public:
    ADelegate(QObject *obj, QObject *parent = 0);
    void update();
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setHiglightColor(QString str);

    QTableWidget *tw;

public slots:
    static QStyleOptionViewItem setOptionAlign(QStyleOptionViewItem option, int align_intflag);
//    QString temp_classname;

public slots:
//    void pbtClickConnect();
};

#endif //ADELEGATE_H
