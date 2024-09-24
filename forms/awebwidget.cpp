#include "forms/awebwidget.h"
#include "afunc.h"
#include "adataset.h"
#include "agui.h"


#include <QWebEnginePage>
#include <QWebEngineProfile>


AWebWidget :: AWebWidget(QWidget *parent) {
    //qDebug() << "AWebWidget::AWebWidget" << parent << parent->size() << parent->children();
    if (parent==0) {qDebug() << "[ERROR] Can't create AWebWidget. Parent is null."; return;}
    container = parent;
    webwidget = new QWebEngineView(parent);
    webwidget->setContextMenuPolicy(Qt::NoContextMenu);
    webwidget->move(QPoint(0,0));
    webwidget->resize(parent->size());
    container->installEventFilter(this);
    //webwidget->page()->settings()->setAttribute(QWebEngineSettings::ShowScrollBars, 0);
    //webwidget->setProperty("FingerScroll", container->property("FingerScroll"));
    AGui::setObjectView(webwidget);
    //QObject::connect(parent, SIGNAL());
}

AWebWidget::~AWebWidget() {
}

// WebWidget update
void AWebWidget :: update() {
    //qDebug() << "AWebWidget :: update" << container->property("date").toString();
    // Read HTML
    QString html_source;
    QString filename = container->property("SourceHTMLFile").toString();
    if (filename !="") {
        html_source = AFunc::readFile(filename);
    } else {
        html_source = container->property("HTML").toString();
    }

    // Read SQL query
    QString sql_string = ADataset::readDataSource(container);
    if (container->property("ifDebug").toBool()) {qDebug() << "[Debug SQL : ATableWidget::static_update] " << sql_string;}

    // Update HTML from database
    QMap <QString, QVariant> params_map;
    QString html = ADataset::parseHtml(html_source, params_map, sql_string);

    // Update all %% widget parameters including local variables
    html = ADataset::updateStringVariablesFromWidget(html, container);

    webwidget->page()->setHtml(html);
}

bool AWebWidget::eventFilter(QObject *obj, QEvent *event) {
    if (event->type()==QEvent::Resize) {
        webwidget->resize(container->size());
    }
    return false;
}
