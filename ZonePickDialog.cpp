#include "ZonePickDialog.h"

#include <QTime>

#include <cityinfo.h>
#include <clockd/libtime.h>

ZonePickDialog::ZonePickDialog(QWidget *parent, const QString &zone) :
    RotatingDialog(parent),
    ui(new Ui::ZonePickDialog)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);

    // Use libcityinfo to easily obtain the list of time zones (hopefully all of
    // them, otherwise /usr/share/zoneinfo/ might be a better source). Watch out
    // for duplicates, as there can be more than one city per time zone.
    QSet<QString> zones;
    Cityinfo **cities = cityinfo_get_all();
    for (int i = 0; cities && cities[i]; i++)
        zones.insert(cityinfo_get_zone(cities[i]));
    cityinfo_free_all(cities);

    // Fill the list
    for (QSet<QString>::const_iterator z = zones.begin(); z != zones.end(); ++z) {
        // Convert to libc format
        const QString zone = ':' + *z;

        // Add item to the list
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(displayName(zone));
        item->setData(Qt::UserRole, zone);
        ui->zoneList->addItem(item);
    }
    ui->zoneList->sortItems();

    // Highlight the selected zone
    for (int i = 0; i < ui->zoneList->count(); i++) {
        if (ui->zoneList->item(i)->data(Qt::UserRole) == zone) {
            ui->zoneList->setCurrentRow(i);
            break;
        }
    }

    // Set up the default zone button
    ui->defaultButton->setText(displayName(QString()));

    connect(ui->zoneList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(onZoneActivated(QListWidgetItem*)));
    connect(ui->searchEdit, SIGNAL(textChanged(QString)), this, SLOT(onSearchTextChanged(QString)));
    connect(ui->defaultButton, SIGNAL(clicked()), this, SLOT(onDefaultClicked()));

    this->setFeatures(NULL, NULL);

    ui->searchEdit->setFocus();
}

ZonePickDialog::~ZonePickDialog()
{
    delete ui;
}

QString ZonePickDialog::displayName(QString zone)
{
    if (zone.isEmpty()) return tr("Current");

    const int offset = time_get_utc_offset(zone.toAscii());

    if (zone.startsWith(':'))
        zone = zone.mid(1).replace('_', ' ');

    return QString("%1 (GMT %2)")
           .arg(zone)
           .arg((offset > 0 ? '-' : '+') + QTime().addSecs(qAbs(offset)).toString("h:mm"));
}

void ZonePickDialog::resizeEvent(QResizeEvent *e)
{
    RotatingDialog::resizeEvent(e);

    ui->zoneList->scrollToItem(ui->zoneList->currentItem(), QAbstractItemView::PositionAtCenter);
}

void ZonePickDialog::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down) {
        ui->zoneList->setFocus();
        return;
    }

    if (ui->searchEdit->hasFocus() || e->text().isEmpty()) {
        return;
    }

    ui->searchEdit->setText(ui->searchEdit->text() + e->text());
    ui->searchEdit->setFocus();
}

void ZonePickDialog::onDefaultClicked()
{
    emit selected(QString());

    this->close();
}

void ZonePickDialog::onZoneActivated(QListWidgetItem *item)
{
    emit selected(item->data(Qt::UserRole).toString());

    this->close();
}

void ZonePickDialog::onSearchTextChanged(const QString &text)
{
    for (int i = 0; i < ui->zoneList->count(); i++) {
        QListWidgetItem *item = ui->zoneList->item(i);
        item->setHidden(!item->text().contains(text, Qt::CaseInsensitive));
    }
}
