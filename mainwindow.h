#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <thememanager.h>
#include "booking.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //Booking Page
    void SetBookingPage();
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

    ThemeManager Themes;
    QString RandomBookingNumber();
private slots:
    void UpdateTheme(int Index);
    void OnRoomInfoRequested(QString RoomName);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
