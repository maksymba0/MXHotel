#ifndef CALENDARDIALOG_H
#define CALENDARDIALOG_H

#include <QDialog>
#include <QCalendarWidget>
#include <QPushButton>
#include <QVBoxLayout>

class CalendarDialog : public QDialog {
    Q_OBJECT

public:
    explicit CalendarDialog(QWidget *parent = nullptr);
    QDate getSelectedDate() const;

private slots:
    void onDateSelected(const QDate &date);

private:
    QCalendarWidget *calendar;
    QDate selectedDate;
};

#endif // CALENDARDIALOG_H
