#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <thememanager.h>
#include "booking.h"
#include "employee.h"
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
    void LoadBooking();


    //Hotel Map Page
    void SetHotelMapPage();

    //Employees Page
    void SetEmployeesPage();
    void LoadEmployees();
    int GetNewEmployeeID();
    void AddEmployee(Employee* employee);
    Employee* GetEmployeeByName(QString Name);


    bool EmployeeModified = false;


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
    Booking* getBooking();
    void setBooking(const Booking &newBooking);

private slots:

    void OnEmployeeCreated();
    void OnEmployeeUpdated();
    void OnEmployeeRemoved();
    void OnEmployeeCleared();

    void OnSearchEmployee();


    void UpdateTheme(int Index);
    void OnRoomInfoRequested(QString RoomName);
    void OnTableItemEditable(QTableWidgetItem* item);
    void OnTableItemChanged(QTableWidgetItem* item);

    void OnCustomerBanned();
    void OnCustomerCreated();
    void OnCustomerRemoved();
    void OnCustomerCheckedOut();
    void OnCustomerCheckedIn();
    void OnCustomerInfoRequested();


    void OnSavedChanges();
    void OnNewBooking();
    void OnChangingRoom();
    void OnCheckOutDateChanged();
    void OnCheckInDateChanged();

    void OnPaymentAdded();
    void OnPaymentRemoved();

private:
    Booking booking;

    QList<Employee> employees;

    QList<Customer> customers;

    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
