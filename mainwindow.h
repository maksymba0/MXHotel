#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <thememanager.h>
#include "booking.h"
#include "payment.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    ThemeManager Themes;

    //Booking Page
    void SetBookingPage();
    void LoadBooking(Booking* booking, int RoomNumber);

    //Hotel Map Page
    void SetHotelMapPage();
    //Employees Page
    void SetEmployeesPage();
    //Customers Page
    void SetCustomersPage();
    //Partners Page
    void setPartnersPage();
    //Notifications Page
    void SetNotificationsPage();
    //Requests Page
    void SetRequestsPage();
    //Settings Page
    void SetSettingsPage();


    QString RandomBookingNumber();
    Payment RandomPayment();
private slots:
    void UpdateTheme(int Index);
    void OnRoomInfoRequested(QString RoomName);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
