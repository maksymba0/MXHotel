#include "calendardialog.h"

CalendarDialog::CalendarDialog(QWidget *parent)
    : QDialog(parent), selectedDate(QDate::currentDate()) {

    setWindowTitle("Select a Date");
    setFixedSize(300, 250);

    calendar = new QCalendarWidget(this);
    calendar->setGridVisible(true);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(calendar);

    connect(calendar, &QCalendarWidget::clicked, this, &CalendarDialog::onDateSelected);
}

void CalendarDialog::onDateSelected(const QDate &date) {
    selectedDate = date;
    accept(); // Closes the dialog
}

QDate CalendarDialog::getSelectedDate() const {
    return selectedDate;
}
