#include <QtGui>
#include "adelegate.h"
#include "agui.h"
//#include "fdataset.h"
#include "afunc.h"
#include "adataset.h"

#include "avariant.h"

ADelegate::ADelegate(QObject *obj, QObject *parent):QItemDelegate (parent) {
    //qDebug() << "ADelegate::ADelegate" << obj << parent << parent->objectName() << parent->metaObject()->className();
    QString className = obj->metaObject()->className();
    if (className=="QLineEdit") {led = static_cast<QLineEdit*>(obj); typeeditor="QLineEdit"; }
    if (className=="QComboBox") {cbb = static_cast<QComboBox*>(obj); typeeditor="QComboBox";}
    if (className=="QCheckBox") {chb = static_cast<QCheckBox*>(obj); typeeditor="QCheckBox";}
    if (className=="QDateEdit") {de = static_cast<QDateEdit*>(obj); typeeditor="QDateEdit";}
    if (className=="QTimeEdit") {te = static_cast<QTimeEdit*>(obj); typeeditor="QTimeEdit";}
    if (className=="QPushButton") {pbt = static_cast<QPushButton*>(obj); typeeditor="QPushButton";}
    color = QColor(0,0,0);
    QString parent_className = parent->metaObject()->className();
    enabled = obj->property("enabled").toBool();
    //qDebug() << "ADelegate::ADelegate" << obj << this << this->enabled;
    if (parent_className=="ATableWidget") {
        ATableWidget *atw = static_cast<ATableWidget*>(parent);
        tw = atw->tablewidget;
    } else {
        tw = 0;
    }
    obj->installEventFilter(this);
    //    qDebug() << tw;
    //connect(this, SIGNAL(closeEditor(QWidget)), this, SLOT(commitAndCloseEditor(QWidget)));
}

void ADelegate::update() {
    //qDebug() << "ADelegate::update";
    if (typeeditor=="QLineEdit") {ADataset::updateObject(led);}
    if (typeeditor=="QComboBox") {ADataset::updateObject(cbb); }
    if (typeeditor=="QCheckBox") {ADataset::updateObject(chb); }
    if (typeeditor=="QDateEdit") {ADataset::updateObject(de); }
    if (typeeditor=="QTimeEdit") {ADataset::updateObject(te); }
    if (typeeditor=="QPushButton") {ADataset::updateObject(pbt);}
}

void ADelegate::commitAndCloseEditor(QWidget *wgt) {
    //qDebug() << "ADelegate::commitAndCloseEditor" << typeeditor;
    if (typeeditor=="QLineEdit") {
        QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
        emit commitData(editor);
        emit closeEditor(editor, QAbstractItemDelegate::NoHint);

        //qDebug() << "commit" << editor;
    }
    if (typeeditor=="QComboBox") {
        QComboBox *editor = qobject_cast<QComboBox *>(sender());
        emit commitData(editor);
        emit closeEditor(editor, QAbstractItemDelegate::NoHint);
        //qDebug() << "commit" << editor;
    }
    if (typeeditor=="QDateEdit") {
        QDateEdit *editor = qobject_cast<QDateEdit *>(sender());
        emit commitData(editor);
        emit closeEditor(editor);
    }
    if (typeeditor=="QTimeEdit") {
        QTimeEdit *editor = qobject_cast<QTimeEdit *>(sender());
        emit commitData(editor);
        emit closeEditor(editor);
    }
}

QWidget *ADelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const {
    //qDebug() << "ADelegate::createEditor";
    QWidget *editor = 0;
    if (typeeditor=="QLineEdit") {
        if (!led->isEnabled()) return 0;
        QLineEdit *le = new QLineEdit(parent);
        le->setAlignment(led->alignment());
        le->setEchoMode(led->echoMode());
        //le->setProperty("enabled", led->property("enabled"));
        if (led->property("Precision").isValid()) {le->setProperty("Precision", led->property("Precision"));}
        editor = static_cast<QWidget *>(le);
        connect(le, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));
    }
    if (typeeditor=="QComboBox") {
        if (!cbb->isEnabled()) return 0;
        QComboBox *cmbox = new QComboBox(parent);
        cmbox->setStyleSheet("QComboBox::drop-down {border-width: 0px;} QComboBox::down-arrow {image: url(noimg); border-width: 0px;}");
        cmbox->setModel(cbb->model());
        cmbox->setEditable(true);
        //cmbox->setAlignment(cbb->alignment());
        cmbox->setProperty("modelColumn", cbb->property("modelColumn"));
        //cmbox->setProperty("enabled", cbb->property("enabled"));
        editor = static_cast<QWidget *>(cmbox);
        connect(cmbox, SIGNAL(currentIndexChanged(int)), this, SLOT(commitAndCloseEditor()));
    }
    if (typeeditor=="QCheckBox") {
        if (!chb->isEnabled()) return 0;
        QCheckBox *chbox = new QCheckBox(parent);
        chbox->setProperty("enabled", chb->property("enabled"));
        editor = static_cast<QWidget *>(chbox);
    }
    if (typeeditor=="QDateEdit") {
        if (!de->isEnabled()) return 0;
        QDateEdit *deb = new QDateEdit(parent);
        //deb->setProperty("enabled", de->property("enabled"));
        AGui::setObjectView(deb);
        editor = static_cast<QWidget *>(deb);
        connect(deb, SIGNAL(dateChanged(QDate)), this, SLOT(commitAndCloseEditor()));
    }
    if (typeeditor=="QTimeEdit") {
        if (!te->isEnabled()) return 0;
        QTimeEdit *teb = new QTimeEdit(parent);
        //teb->setProperty("enabled", te->property("enabled"));
        AGui::setObjectView(teb);
        editor = static_cast<QWidget *>(teb);
        connect(teb, SIGNAL(dateChanged(QTime)), this, SLOT(commitAndCloseEditor()));
    }
    if (typeeditor=="QPushButton") {
        if (!pbt->isEnabled()) return 0;
        QPushButton *pbt1 = new QPushButton("..", parent);
        //pbt1->setProperty("onClick", pbt->property("onClick"));
        //FEngine::createConnects(pbt1);
        //QObject::connect(pbt1,SIGNAL(clicked()),this,SLOT(pbtClickConnect()));
        //qDebug() << pbt;
        editor = pbt1;
    }
    //connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));
    if (tw) {tw->setProperty("selectmode", false);}
    return editor;
}

void ADelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const {
    //qDebug() << "ADelegate::updateEditorGeometry";
    editor->setStyleSheet("border:0px;");
    if (typeeditor=="QLineEdit") {
        //QLineEdit *le = static_cast<QLineEdit *>(editor);
        //le->setAlignment(led->alignment());
    }
    if (typeeditor=="QCheckBox") {
        QRect r(option.rect.x() + option.rect.width()/2 - 7, option.rect.y() + option.rect.height()/2 - 7, 13, 13);
        editor->setGeometry(r);
    } else {
        editor->setGeometry(option.rect);
    }
}

void ADelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QVariant data = index.model()->data(index, Qt::DisplayRole);
    //qDebug() << "ADelegate::setEditorData" << index.row() << index.column() << data;
    ATableWidget *tf = static_cast<ATableWidget *>(this->parent());
    editor->parent()->parent()->removeEventFilter(tf);
    if (typeeditor=="QLineEdit") {
        QLineEdit *le = static_cast<QLineEdit *>(editor);
        ADataset::setData(le, data);
        le->selectAll();
    }
    if (typeeditor=="QComboBox") {
        QComboBox *cmbox = static_cast<QComboBox *>(editor);
        ADataset::setData(cmbox, data);
    }
    if (typeeditor=="QCheckBox") {
        QCheckBox *chbox = static_cast<QCheckBox *>(editor);
        if (data.toBool()) {chbox->setCheckState(Qt::Checked);}
        else {chbox->setCheckState(Qt::Unchecked);}
    }
    if (typeeditor=="QDateEdit") {
        QDateEdit *de = static_cast<QDateEdit *>(editor);
        ADataset::setData(de, data);
    }
    if (typeeditor=="QTimeEdit") {
        QDateEdit *te = static_cast<QDateEdit *>(editor);
        ADataset::setData(te, data);
    }
    //qDebug() << "ADelegate::setEditorData";
}

void ADelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {

    QVariant value;
    if (typeeditor=="QLineEdit") {
        QLineEdit *le = static_cast<QLineEdit *>(editor);
        value = ADataset::getData(le);
    }
    if (typeeditor=="QComboBox") {
        QComboBox *cb = static_cast<QComboBox *>(editor);
        value = ADataset::getData(cb);
    }
    if (typeeditor=="QCheckBox") {
        QCheckBox *chbox = static_cast<QCheckBox *>(editor);
        value = chbox->checkState();
    }
    if (typeeditor=="QDateEdit") {
        QDateEdit *de = static_cast<QDateEdit *>(editor);
        value = AVariant::convertDateToString(de->date());
    }
    if (typeeditor=="QTimeEdit") {
        QTimeEdit *te = static_cast<QTimeEdit *>(editor);
        value = te->time().toString("hh::mm");
    }
    QVariant oldvalue=model->data(index).toString();
    model->setData(index, value, Qt::DisplayRole);

    //qDebug() << "ADelegate::setModelData" << value << oldvalue;

    ATableWidget *parent = static_cast<ATableWidget *>(this->parent());
    parent->cellUpdateAction(index.row(), index.column());
    if (oldvalue!=value) {
        parent->cellChangeAction(index.row(), index.column());
    }
    //if (oldvalue!=value && newRowIndex==index.row()) {
        //model->setProperty("newRowIndex",-1);
    //}
//    editor->parent()->parent()->installEventFilter(tf);
}

QStyleOptionViewItem ADelegate::setOptionAlign(QStyleOptionViewItem option, int align_intflag) {
    //qDebug() << "ADelegate::setOptionAlign";
    if (align_intflag==33 || align_intflag==65 || align_intflag==129 || align_intflag==257) {option.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;}
    if (align_intflag==34 || align_intflag==66 || align_intflag==130 || align_intflag==258) {option.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;}
    if (align_intflag==36 || align_intflag==68 || align_intflag==132 || align_intflag==260) {option.displayAlignment = Qt::AlignHCenter | Qt::AlignVCenter;}
    if (align_intflag==40 || align_intflag==72 || align_intflag==136 || align_intflag==264) {option.displayAlignment = Qt::AlignJustify | Qt::AlignVCenter;}
    return option;
}

//#include <QBrush>
void ADelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    //qDebug() << "ADelegate::paint" << this->parent() << painter << option << index;
    QVariant data = index.model()->data(index, Qt::DisplayRole);
    QStyleOptionViewItem myOption = option;

    QString temp_classname = this->parent()->metaObject()->className();
    if (temp_classname == "ATableWidget") {
        ATableWidget *atable = static_cast<ATableWidget *>(this->parent());
        myOption.palette.setColor(QPalette::Text, atable->highlightedRows.at(index.row()));
    }

    if (!this->enabled) {
        painter->fillRect(option.rect, QBrush(QColor(250,250,250)));
        myOption.palette.setColor(QPalette::Text, QColor(120,120,120));
    }

    if (typeeditor=="QLineEdit") {
        myOption = ADelegate::setOptionAlign(myOption, int(led->alignment()));
        if (led->echoMode()==QLineEdit::Password) {
            QRect rect = option.rect;
            QStyleOptionButton button;
            button.features = QStyleOptionButton::Flat;
            button.rect = rect;
            button.text = "***";
            //button.state = _state | QStyle::State_Enabled;
            QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
        } else {
            //if (led->isEnabled()) { painter->fillRect(option.rect, QBrush(QColor("#fafafa"))); painter->setPen(QColor("#cccccc"));}
            drawDisplay(painter, myOption, option.rect, data.toString());

        }
    }
    if (typeeditor=="QDateEdit") {
        myOption = ADelegate::setOptionAlign(myOption, int(de->alignment()));
        drawDisplay(painter, myOption, option.rect, AVariant::convertDateToString(data.toDate()));
    }
    if (typeeditor=="QTimeEdit") {
        myOption = ADelegate::setOptionAlign(myOption, int(te->alignment()));
        drawDisplay(painter, myOption, option.rect, data.toTime().toString("hh:mm"));
    }
    if (typeeditor=="QComboBox") {
        //qDebug() << "ADelegate::paint" << option.rect << myOption;
        myOption = ADelegate::setOptionAlign(myOption, int(cbb->lineEdit()->alignment()));
        int index = cbb->findData(data);
        QVariant text = cbb->itemText(index);
        //painter->drawText(option.rect.x()+4, option.rect.y(), option.rect.width()-8, option.rect.height(), cbb->lineEdit()->alignment(), text);
        //painter->drawText(option.rect, cbb->lineEdit()->alignment(), text);
        drawDisplay(painter, myOption, option.rect, text.toString());
    }
    if (typeeditor=="QCheckBox") {
        //qDebug() << "QCheckBox" << myOption.rect << option.rect << painter->window();
        int width = 13;
        int height = 13;
        int x = (option.rect.width()-width)/2+option.rect.x();
        int y = (option.rect.height()-height)/2+option.rect.y();
        if (data.toBool()) {
            QPixmap pixmap("C:/Chameleon/data/ui/posadmin/image/checkbox_checked.png");
            painter->drawPixmap(x,y,width,height, pixmap);
        } else {
            QPixmap pixmap("C:/Chameleon/data/ui/posadmin/image/checkbox_unchecked.png");
            painter->drawPixmap(x,y,width,height, pixmap);
        }

            //        QRect rect(x,y,width,height);
        //option.rect.setWidth(10);
//        drawCheck(painter, myOption, rect, data.toBool() ? Qt::Checked : Qt::Unchecked);
    }
    if (typeeditor=="QPushButton") {
        QRect rect = option.rect;

        QStyleOptionButton button;
            button.features = QStyleOptionButton::Flat;
            button.rect = rect;
            button.text = "..";
            //button.state = _state | QStyle::State_Enabled;
            QApplication::style()->drawControl
                (QStyle::CE_PushButton, &button, painter);
    }
    painter->setPen(Qt::SolidLine);
    painter->setPen(QColor(230,230,230));
    //painter->drawLine(QLine(option.rect.bottomLeft(), option.rect.bottomRight()));
    //painter->drawLine(QLine(option.rect.topRight(), option.rect.bottomRight()));
    painter->setPen(QColor(Qt::black));
}

void ADelegate::setHiglightColor(QString str){
    //qDebug() << "ADelegate::setHiglightColor";
    color.setNamedColor(str);
}

//void pbtClickConnect(){
    //QObject *obj = QObject::sender();
    //FEngine::uniClickSlot(obj);
//}

bool ADelegate::eventFilter(QObject *obj, QEvent *event) {
    ////    if (event->type()!= 12 && event->type()!=25) {qDebug() << "ADelegate::eventFilter" << obj << event->type();}
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        //qDebug() << "key1:" << keyEvent->key();
        //        if (keyEvent->key() == Qt::Key_Left) {
        //            qDebug() << "left catched";
        //            if (this->parent()->metaObject()->className()=="ATableWidget") {
        //                ATableWidget *atw = static_cast<ATableWidget*>(this->parent());
        //                bool selectmode = tw->property("selectmode").toBool();
        //                if (selectmode) {atw->prevCell(); return true;}
        //            }

        //        }
        //        if (keyEvent->key() == Qt::Key_Right) {
        //            qDebug() << "right catched";
        //            if (this->parent()->metaObject()->className()=="ATableWidget") {
        //                ATableWidget *atw = static_cast<ATableWidget*>(this->parent());
        //                bool selectmode = tw->property("selectmode").toBool();
        //                if (selectmode) {atw->nextCell(); return true;}
        //            }
        //        }
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return ) {
            //qDebug() << "Enter catched";
            //QKeyEvent event1 (QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
            //qApp->sendEvent(obj, &event1);
            //if (tw) {tw->closePersistentEditor(tw->currentItem());}
            //return true;
        }
        if (keyEvent->key() == Qt::Key_Escape) {
            //qDebug() << "Enter catched";
            if (tw) {tw->closePersistentEditor(tw->currentItem());}
            //if (tw) {tw->setProperty("selectmode", true);}
            //return true;
        }
    }
    return false;
}
