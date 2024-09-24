#include "acombocheckbox.h"
#include <QListView>
#include <QMouseEvent>
#include <QDebug>

////////////////////////////////////////////////////////////////////////////////////
// конструктор
////////////////////////////////////////////////////////////////////////////////////
AComboCheckBox::AComboCheckBox(QWidget *parent) : QComboBox(parent)
{
    this->installEventFilter(this);
    model = new QStandardItemModel();
    this->setModel(model);
}


bool AComboCheckBox::addItem( QString value, QString key ) {
    QStandardItem *item = new QStandardItem();
    item->setText(value);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked, Qt::CheckStateRole);
//    item->setData( QSize(200, 25), Qt::SizeHintRole);
//    item->setData( Qt::AlignVCenter, Qt::TextAlignmentRole);

//    item->setTextAlignment(Qt::AlignCenter);
    model->appendRow(item);
    this->setItemData(this->count()-1, key, Qt::UserRole);
//    this->
    return true;
}

void AComboCheckBox::selectAllItems() {
    for (int i=0;i<this->count();i++) {
        this->model->item(i,0)->setData(Qt::Checked, Qt::CheckStateRole);
    }

}

QList<QVariant> AComboCheckBox::getSelected() {
    QList<QVariant> result;
    for (int i=0;i<this->count();i++) {
        if (this->model->data(this->model->item(i,0)->index(), Qt::CheckStateRole).toBool()) {result << this->itemData(i, Qt::UserRole).toInt();}
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//  Перехватчик событий для работы чекбоксов
////////////////////////////////////////////////////////////////////////////////
bool AComboCheckBox::eventFilter(QObject * watched, QEvent * event)
{
    // проверка тика отловленного события
    if (event->type() == QEvent::MouseButtonRelease)
    {
        // блокируем смену галочки при открытии
        if (m_opening)
        {
            m_opening = false;
//            qDebug() << "opened";
            return QObject::eventFilter(watched, event);
        }
        // проверяем тип
        if (watched->parent()->inherits("QListView"))
        {
            // приводим к нужным типам
            QListView *tmp = (QListView *)(watched->parent());
            QMouseEvent *mEvent = (QMouseEvent *)event;
            QModelIndex ind = tmp->indexAt(mEvent->pos());
            // меняем состояние cheched
            bool checked = tmp->model()->data(ind,Qt::CheckStateRole).toBool();
            tmp->model()->setData(ind,!checked,Qt::CheckStateRole);
            // блокируем закрытие комбобокса
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}

////////////////////////////////////////////////////////////////////////////////
// функция раскрытия комбобокса
////////////////////////////////////////////////////////////////////////////////
void AComboCheckBox::showPopup()
{
    // флаг открытия комбобокса
    m_opening = true;
    // вызываем функцию класса - предка
    QComboBox::showPopup();
    emit opened();
}
void AComboCheckBox::hidePopup()
{
    // флаг открытия комбобокса
    m_opening = true;
    // вызываем функцию класса - предка
    QComboBox::hidePopup();
    emit closed();
}

