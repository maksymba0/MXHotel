#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <thememanager.h>
#include "booking.h"
#include "employee.h"
#include "payment.h"
#include "ucustomer.h"
#include "partner.h"
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

    void ResetAndGetEmployees();
    void LoadEmployees();

    int GetNewEmployeeID();
    void AddEmployee(Employee* employee, bool showMessage = false);
    Employee* GetEmployeeByName(QString Name);


    bool EmployeeModified = false;


    //Customers Page
    void SetCustomersPage();
    void LoadCustomers();
    UCustomer* GetCustomerByID(int ID);
    UCustomer* GetCustomerByName(QString Name);
    void AddCustomer(UCustomer* customer);

    //Partners Page
    void setPartnersPage();
    Partner* GetPartner();
    void AddPartner(Partner* partner);
    void LoadPartners();

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

    // Database DB RELATED funcs

    bool DBExistsBooking();
    bool DBCreateNewBooking();
    bool DBUpdateBooking();
    bool DBClearBooking();
    bool DBRemoveBooking();

    bool DBCreateEmployee();
    bool DBRemoveEmployee();

    bool DBUpdateCustomer();


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

    void OnBookingCustomerBanned();
    void OnBookingCustomerCreated();
    void OnBookingCustomerRemoved();
    void OnBookingCustomerCheckedOut();
    void OnBookingCustomerCheckedIn();
    void OnCustomerInfoRequested();
    void OnNewCustomerCreated();
    void OnSelectedCustomerRemoved();

    void OnNewPartnerCreated();
    void OnPartnerRemoved();

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

    QList<UCustomer> uCustomers;

    QList<Partner> partners;

    Ui::MainWindow *ui;
public:
    bool CustomerPageBeenModified = false;



};
#endif // MAINWINDOW_H
