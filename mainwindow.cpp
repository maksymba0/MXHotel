#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSpacerItem>
#include <QTabBar>
#include <QDebug>
#include <QVBoxLayout>
#include <QRandomGenerator>
#include <QMessageBox>
#include "calendardialog.h"
#include <QInputDialog>
#include "employee.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Themes = ThemeManager(this);


    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    // Set up the pages
    SetBookingPage();
    SetHotelMapPage();
    SetEmployeesPage();
    SetCustomersPage();
    setPartnersPage();
    SetNotificationsPage();
    SetRequestsPage();
    SetSettingsPage();

    UpdateTheme((int)Themes::DEFAULT);




}

MainWindow::~MainWindow()
{
    delete ui;
}

// CALLBACKS
void MainWindow::UpdateTheme(int Index)
{
    Themes.SelectTheme(Index);
    qDebug() << "Changed to " << Themes.GetThemeName();

}

void MainWindow::OnRoomInfoRequested(QString RoomName)
{
    ui->tabWidget->setCurrentIndex(0);

    if(getBooking()->getIsChangingRoom())
    {
        RoomName.remove("BookingRoom_");
        int RoomNumber = RoomName.toInt();
        qDebug() << "Selected Room #" << RoomNumber;
        this->getBooking()->setRoomNumber(RoomNumber);
        getBooking()->setIsChangingRoom(false);
        LoadBooking();
        return;
    }

    // Requesting data

    Booking newbkg;
    Booking* booking = &newbkg;
    booking->setBookerEmail("admin.admin@gmail.com");
    booking->setBookerName("Maksy Creator");
    booking->setBookerPhonenumber("+44 123 123 123");
    booking->setBookingNumber(RandomBookingNumber());
    const auto date = QDateTime::currentDateTime();
    booking->setCheckedinDate(date.addDays(1));
    booking->setCheckoutDate(date.addDays(2));
    booking->setCreatedDate(date.addDays(0));
    booking->setRoomNumber(0);
    Customer customer;
    customer.setPhonenumber("+44 123 123 123");

    customer.setDob(QDate(2002,01,15));
    customer.setAge();
    customer.setName("Daniel Ricardo");
    customer.setDocumentNumber("KHTAIR9291");
    customer.setDocumentType("ID Card");
    customer.setCheckedIn(false);
    booking->addCustomer(customer);
    customer.setPhonenumber("+380 95 128 190");

    customer.setDob(QDate(1998,11,26));
    customer.setAge();
    customer.setName("Vlad Kroker");
    customer.setDocumentNumber("MI299DAS0");
    customer.setDocumentType("Passport");
    customer.setCheckedIn(false);
    booking->addCustomer(customer);
    customer.setPhonenumber("+380 68 330 110");

    customer.setDob(QDate(2003,05,14));
    customer.setAge();
    customer.setName("Zombuz Stepankov");
    customer.setDocumentNumber("PIV920OO");
    customer.setDocumentType("Refugee passport");
    customer.setCheckedIn(false);
    booking->addCustomer(customer);

    booking->setIsBeingCreated(false);

    for(int i = 0; i < 3; i++)
    {
        Payment payment;
        payment.PopulateWithRandomValues();
       // payment.Print();
        booking->addPayment(payment);
    }
    // Changing data on Hotel Map page

    this->setBooking(*booking);
    ui->tableWidget_2->blockSignals(true);
        ui->tableWidget->blockSignals(true);
    LoadBooking();

        ui->tableWidget_2->blockSignals(false);
        ui->tableWidget->blockSignals(false );
}

void MainWindow::OnTableItemEditable(QTableWidgetItem *item)
{
    qDebug() << "Double clicked";

    if(!item)
    {
        return;
    }
    QTableWidget* TableWidget = nullptr;
    if(ui->tableWidget == item->tableWidget() || // customers
        (ui->tableWidget_2 == item->tableWidget()) || // payment
        (ui->tableWidget_3 == item->tableWidget())) // employees
    {
        TableWidget = item->tableWidget();
    }
    if(!TableWidget)
    {
        return;
    }
    TableWidget->blockSignals(true);
    if(item->flags() & Qt::ItemIsEditable)
    {
        // Make this row non editable
        auto row = item->row();
        auto columnCount = TableWidget->columnCount();
        for(int i = 0; i < columnCount; ++i)
        {
            auto item = TableWidget->item(row,i);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            if(Themes.IsStandard())
            {
                item->setForeground(Qt::black);
            }else
            {
                item->setForeground(Qt::white);
            }
        }
    }else
    {
        // Make editable

        auto row = item->row();
        auto columnCount = TableWidget->columnCount();
        for(int i = 0; i < columnCount; ++i)
        {
            auto item = TableWidget->item(row,i);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            item->setForeground(QColor(130, 200, 200));
        }
    }
    TableWidget->blockSignals(false);
}

void MainWindow::OnTableItemChanged(QTableWidgetItem* item)
{
    qDebug() << "Item changed";
    if(!item)
    {
        return;
    }
    // Update Customer Data with these changes
    auto row = item->row();

    if(item->tableWidget() == ui->tableWidget) // CUSTOMER TABLE
    {
        if(getBooking()->getCustomers().empty() || row > getBooking()->getCustomers().size())
        {
            qDebug() << "Check customersArr size";
            return;
        }
        Customer* customer = &getBooking()->getCustomers()[row];
        QDate DOB;
        switch(item->column())
        {
        case 0://name name
            customer->setName(item->text());
            break;
        case 1://dob
            DOB = QDate::fromString(item->text(),"dd.MM.yyyy");
            if(DOB.isValid())
            {
                qDebug() << "DOB SET TO: " << DOB.toString();
                customer->setDob(DOB);
            }else
            {
                DOB = QDate::currentDate().addYears(-20);
                customer->setDob(DOB);
                item->setText(DOB.toString("dd.MM.yyyy"));
            }
            break;
        case 2://phone

            customer->setPhonenumber(item->text());
            break;
        case 3://docs

            customer->setDocumentNumber(item->text());
            break;
        case 4://docs type

            customer->setDocumentType(item->text());
            break;
        }
        getBooking()->setIsModified(true);
        qDebug() << "Successfully changed data for " << customer->getName();

        booking.Print();

    }else if(item->tableWidget() == ui->tableWidget_2) // PAYMENT TABLE
    {
        qDebug() << "Payment table";
        Payment* payment = &getBooking()->getPayments()[row];
        if(payment)
        {
            if(getBooking()->getPayments().empty() || row > getBooking()->getPayments().size())
            {
                qDebug() << "Check paymentsArr size";
                return;
            }
            switch(item->column())
            {
            case 0:
                payment->setAmount(ui->tableWidget_2->item(row,0)->text().toFloat()); // value;
                break;
            case 1:

                payment->setCurrency(ui->tableWidget_2->item(row,1)->text()); // currency;
                break;
            case 2:
                payment->setMethod(ui->tableWidget_2->item(row,2)->text()); // method;
                break;
            }
            getBooking()->setIsModified(true);
        }
    }else if(item->tableWidget() == ui->tableWidget_3) // EMPLOYEE TABLE
    {
        auto EmployeeTable = ui->tableWidget_3;
        if(employees.empty() || row > employees.size())
        {
            qDebug() << "Outside of index boundary";
            return;
        }
        Employee* employee = &employees[row];
        if(employee)
        {
            switch(item->column())
            {
            case 0:
                employee->setName(EmployeeTable->item(row,0)->text());
                break; // name
            case 1:

                employee->setRole(EmployeeTable->item(row,1)->text());
                break; // role
            case 2:

                employee->setEmail(EmployeeTable->item(row,2)->text());
                break; // email
            case 3:

                employee->setPhoneNumber(EmployeeTable->item(row,3)->text());
                break; // phone number
            case 4:

                employee->setSalary(EmployeeTable->item(row,4)->text().toFloat());
                break; //salary
            case 5:

                employee->setLogin(EmployeeTable->item(row,5)->text());
                break;// login
            case 6:
                employee->setPassword(EmployeeTable->item(row,6)->text());
                break; // password
            }
            EmployeeModified = true;
            employee->setIsModified(true);
            employee->Print();

        }

    }


}

void MainWindow::OnSavedChanges()
{
    qDebug() << "OnSavedChanges";
    if(!getBooking()->IsValid())
    {
        QMessageBox::warning(this,"Save changes- failed","Trying to save an empty booking. \nPlease use 'NEW' to create a new booking.");
        return;
    }
    if(getBooking()->getIsBeingCreated())
    {
        auto reply = QMessageBox::question(this, "Save changes", QString("New booking %d% for %y% customers will be created. Are you sure you want to do this?").replace("%y%",QString::number(getBooking()->getCustomers().size())).
                                                    replace("%d%",getBooking()->getBookingNumber()));
        switch(reply)
        {
        case QMessageBox::Yes:
            QMessageBox::information(this,"Save change - success","Booking has been created.");
            this->getBooking()->setBookingNumber(this->RandomBookingNumber());
            this->getBooking()->setIsBeingCreated(false);
            LoadBooking();
            break;
        case QMessageBox::No:
        default:
            break;

        }

        // This is a new booking, we will send a request to create a new booking on the database
    }else
    {
        if(getBooking()->getIsModified())
        {
            auto reply = QMessageBox::question(this, "Save changes", QString("You are trying to save the booking %d% for %y% customers with some changes. Are you sure you want to do it?").replace("%y%",QString::number(getBooking()->getCustomers().size())).
                                                                     replace("%d%",getBooking()->getBookingNumber()));
            switch(reply)
            {
            case QMessageBox::Yes:
                QMessageBox::information(this,"Save change - success","Booking has been updated.");
                getBooking()->setIsModified(false);
                break;
            case QMessageBox::No:
            default:
                break;

            }
        }else
        {
            QMessageBox::warning(this,"Save changes - failed", "Cannot save booking with no changes done.");
        }
        // This is a booking that we retrieved from the db, that we are editing.
    }

}
void MainWindow::OnBookingCustomerBanned()
{

    qDebug() << "OnCustomerBanned";
}
void MainWindow::OnBookingCustomerCreated(){

    qDebug() << "OnBookingCustomerCreated";
    int count = ui->tableWidget->rowCount();
    if(count >= 5)
    {
        QMessageBox::warning(this,"Create Customer","A room cannot contain more than 5 customers");
        return;
    }

    ui->tableWidget->insertRow(count);
    qDebug() << "rows: "<<ui->tableWidget->rowCount();
    int columnNum = ui->tableWidget->columnCount();
    Customer customer;
    booking.addCustomer(customer);
    for(int i = 0 ; i < columnNum; ++i)
    {
        QTableWidgetItem* item = new QTableWidgetItem("Text");
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,i,item);
        auto Widget = ui->tableWidget->item(ui->tableWidget->rowCount()-1,i);
        if(Widget)
        {
            Widget->setBackground(QColor(0x545454));
        }
        else
        {
            qDebug() << "Reading nullptr;";
        }
    }
    QMessageBox::about(this,"Customer Created","Please make sure to add data to the customer.");
    getBooking()->setIsModified(true);
    LoadBooking();
}
void MainWindow::OnBookingCustomerRemoved(){

    qDebug() << "OnCustomerRemoved";
    QSet<int> rows;

    // Change information on booking

    QList<QTableWidgetItem*> selectedItems = ui->tableWidget->selectedItems();

    if(selectedItems.empty())
    {
        auto RowMax = ui->tableWidget->rowCount();
        if(RowMax <= 0)
        {
            return;
        }
        auto NameWidget = ui->tableWidget->item(RowMax-1,0);
        if(!NameWidget)
        {
            return;
        }
        auto Name = (NameWidget->text() != "" ? NameWidget->text() : "Empty cell");
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,"Remove customer",""
                                            "NO SPECIFIC CUSTOMER SELECTED. Are you sure you want to remove the last customer ( "+Name+ " ) from the booking?",
                                            QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::Yes)
        {
         auto customer = booking.getCustomerByName(ui->tableWidget->item(RowMax-1,0)->text());
         if(customer)
         {
             bool IsSuccess  = false;

             // for each selected row
             getBooking()->getCustomers().removeOne(*customer);
             if(IsSuccess)
                 QMessageBox::information(this,"REMOVE SUCCESS",customer->getName() + " has been removed!");
            getBooking()->setIsModified(true);
             ui->tableWidget->removeRow(RowMax-1);
         }else
         {
             QMessageBox::warning(this,"Remove Customer","Error while removing the last customer from the booking object");
         }
        }
        return;
    }
    // Update UI
    for(auto& obj : selectedItems)
    {
        rows.insert(obj->row());
    }

    for(auto& Row : rows)
    {
        auto NameWidget = ui->tableWidget->item(Row,0);
        if(!NameWidget)
        {
            continue;
        }
        auto customer = booking.getCustomerByName(NameWidget->text());
        bool IsSuccess = false;
        QString name = customer->getName();
        if(!customer)
        {
            continue;
        }
        IsSuccess  = true;
        // for each selected row
        ui->tableWidget->removeRow(Row);
        getBooking()->getCustomers().removeOne(*customer);
        if(IsSuccess)
        {

            QMessageBox::information(this,"REMOVE SUCCESS",name + " has been removed!");
            getBooking()->setIsModified(true);
        }
    }
}
void MainWindow::OnBookingCustomerCheckedOut(){

    qDebug() << "OnCustomerCheckedOut";
    QSet<int> rows;

    // Change information on booking

    QList<QTableWidgetItem*> selectedItems = ui->tableWidget->selectedItems();
    if(selectedItems.empty())
    {
        QMessageBox::warning(this,"Action failed","Please select a customer first!");
        return;
    }
    // Update UI
    for(auto& obj : selectedItems)
    {
        rows.insert(obj->row());
    }
    int columnCount = ui->tableWidget->columnCount();
    for(auto& Row : rows)
    {
        auto NameWidget = ui->tableWidget->item(Row,0);
        auto customer = booking.getCustomerByName(NameWidget->text());
        bool IsSuccess = false;
        if(!customer)
        {
            continue;
        }
        if(!customer->getCheckedIn())
        {
            QMessageBox::warning(this,"CHECK-OUT FAILED",customer->getName() + " is not checked in! Why checking out?");
            continue;
        }
        customer->setCheckedIn(false);
        IsSuccess  = true;
        // for each selected row
        for(int Column = 0; Column < columnCount;++Column)
        {
            auto item = ui->tableWidget->item(Row,Column);

            item->setBackground(QColor(0x545454));

        }
        if(IsSuccess)
        {
            QMessageBox::information(this,"CHECK-OUT SUCCESS",customer->getName() + " has been checked out!");
            getBooking()->setIsModified(true);
        }

    }
}
void MainWindow::OnBookingCustomerCheckedIn(){

    qDebug() << "OnCustomerCheckedIn";
    QSet<int> rows;

    // Change information on booking

    QList<QTableWidgetItem*> selectedItems = ui->tableWidget->selectedItems();
    if(selectedItems.empty())
    {
        QMessageBox::warning(this,"Action failed","Please select a customer first!");
        return;
    }
    // Update UI
    for(auto& obj : selectedItems)
    {
        rows.insert(obj->row());
    }
    int columnCount = ui->tableWidget->columnCount();
    for(auto& Row : rows)
    {
        auto NameWidget = ui->tableWidget->item(Row,0);
        bool IsSuccess = false;
        auto customer = booking.getCustomerByName(NameWidget->text());
        if(!customer)
        {
            continue;
        }
        if(customer->getCheckedIn())
        {
            QMessageBox::warning(this,"CHECK-IN FAILED",customer->getName() + " is already checked in!");
            continue;
        }
        customer->setCheckedIn(true);
        IsSuccess = true;
        // for each selected row
        for(int Column = 0; Column < columnCount;++Column)
        {
            auto item = ui->tableWidget->item(Row,Column);
            item->setBackground(QColor(0x1C7D17));
        }
        if(IsSuccess)
        {

            QMessageBox::information(this,"CHECK-IN SUCCESS",customer->getName() + " has been checked in!");
            getBooking()->setIsModified(true);
        }
    }
}

void MainWindow::OnCustomerInfoRequested()
{
    bool hasName = false;
    bool hasSurname = false;
    bool hasPhone = false;
    bool hasEmail = false;
    bool hasBookingNum = false;


    QRegularExpression regex("^[A-Za-z]+$");
    if(ui->lineEdit->text() != "" && regex.match(ui->lineEdit->text()).hasMatch()) // first name
    {
        hasName = true;
    }
    if(ui->lineEdit_2->text() != "" && regex.match(ui->lineEdit_2->text()).hasMatch()) // surname
    {
        hasSurname = true;
    }
     regex = QRegularExpression("^[+0-9 ]+$");
    if(ui->lineEdit_3->text() != "" && regex.match(ui->lineEdit_3->text()).hasMatch()) // surname
    {
        hasPhone= true;
    }

    regex = QRegularExpression(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
    if(ui->lineEdit_4->text() != "" && regex.match(ui->lineEdit_4->text()).hasMatch()) // surname
    {
        hasEmail = true;
    }
    regex = QRegularExpression("^[A-Za-z]+$");
    if(ui->lineEdit_5->text() != "" && regex.match(ui->lineEdit_5->text()).hasMatch()) // first name
    {
        hasBookingNum = true;
    }

    qDebug() << ui->lineEdit_3->text() << " " << ui->lineEdit_4->text() << " " << ui->lineEdit_5->text();
    QString name = hasName && hasSurname ? ui->lineEdit->text() + " " + ui->lineEdit_2->text()
                   : hasName ?  ui->lineEdit->text()
                   : hasSurname ? ui->lineEdit_2->text()
                   : "";
    QString phone = hasPhone ? ui->lineEdit_3->text() : "";
    QString email = hasEmail ? ui->lineEdit_4->text() : "";
    QString bookingNum = hasBookingNum ? ui->lineEdit_5->text() : "";


    QString Query = QString("Name: %1 Phone: %2 Email: %3 Booking: %4")
                        .arg(name)
                        .arg(phone)
                        .arg(email)
                        .arg(bookingNum);

    qDebug() << " Sending request" << Query;

}

void MainWindow::OnNewBooking()
{
    getBooking()->Clear();
    getBooking()->Print();
    getBooking()->setIsBeingCreated(true);
    getBooking()->RequestBookerInfo();
    LoadBooking();
}

void MainWindow::OnChangingRoom()
{
    ui->tabWidget->setCurrentIndex(1);// HotelMap
    getBooking()->setIsChangingRoom(true);
}

void MainWindow::OnCheckOutDateChanged()
{
    CalendarDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QDate selectedDate = dialog.getSelectedDate();
        QDateTime dateTime = QDateTime::fromString(selectedDate.toString("yyyy-MM-dd"), "yyyy-MM-dd");
        if(dateTime < getBooking()->getCreatedDate())
        {
            QMessageBox::warning(this,"Change Check-out date","Cannot choose check-out date in the past");
            return;
        }
        if(getBooking()->getCheckedinDate().isValid())
        {
            if(dateTime < getBooking()->getCheckedinDate())
            {

                QMessageBox::warning(this,"Change Check-out date","Wrong check-out date. It cannot be before checking in.");
                return;
            }
        }
        qDebug() << "Selected date: " << selectedDate.toString("dd.MM.yyyy");
        getBooking()->setCheckoutDate(dateTime);
        LoadBooking();
    }
}

void MainWindow::OnCheckInDateChanged()
{
    CalendarDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QDate selectedDate = dialog.getSelectedDate();
        QDateTime dateTime(selectedDate,QTime(0,0,0)); // Properly converts QDate to QDateTime

        qDebug() << "Selected date: " << dateTime.toString("dd.MM.yyyy hh:mm:ss");
        qDebug() << "Created date: " << getBooking()->getCreatedDate().toString("dd.MM.yyyy hh:mm:ss");

        if(dateTime >= getBooking()->getCreatedDate()) {
            getBooking()->setCheckedinDate(dateTime);
            LoadBooking();
        } else {
            QMessageBox::warning(this, "Change Check-in date", "Cannot choose check-in date in the past");
        }
    }
}


void MainWindow::OnPaymentAdded()
{
    auto paymentTable = ui->tableWidget_2;


    paymentTable->insertRow(paymentTable->rowCount());
    qDebug() << "Added payment (currently has : "<<paymentTable->rowCount()<<" )";

    Payment payment;
    getBooking()->Print();
    getBooking()->addPayment(payment);
    getBooking()->Print();


    QTableWidgetItem* paymentAmount = new QTableWidgetItem(" ");
    paymentTable->setItem(paymentTable->rowCount()-1,0,paymentAmount);

    QTableWidgetItem* paymentCurrency = new QTableWidgetItem(" ");
    paymentTable->setItem(paymentTable->rowCount()-1,1,paymentCurrency);

    QTableWidgetItem* paymentMethod= new QTableWidgetItem(" ");
    paymentTable->setItem(paymentTable->rowCount()-1,2,paymentMethod);

    getBooking()->setIsModified(true);


}

void MainWindow::OnPaymentRemoved()
{
    if(getBooking()->getPayments().empty())
    {
        QMessageBox::warning(this,"Remove payment","Cannot remove payments because none exists");
        return;
    }
    auto paymentTable = ui->tableWidget_2;
    //int columnNum = paymentTable->columnCount();
    QSet<int> Rows;
    QList<QTableWidgetItem*> selectedItems = paymentTable->selectedItems();
    if(selectedItems.empty())
    {
        auto& payment = getBooking()->getPayments()[paymentTable->rowCount()-1];

        auto result = QMessageBox::question(this,"Remove payment",
                                            "No payment selected. Do you want to remove the last payment of"+QString(std::to_string(payment.getAmount()).c_str())+" ?",
                                            QMessageBox::Yes | QMessageBox::No);
        if(result == QMessageBox::No)
        {
            return;
        }else
        {
            getBooking()->getPayments().removeOne(payment);
            paymentTable->removeRow(paymentTable->rowCount()-1);
            getBooking()->setIsModified(true);
        }
    }
    else
    {
        for(auto& item : selectedItems)
        {
            Rows.insert(item->row());
        }
        for(int i = Rows.size() -1; i >= 0; --i)
        {
            Payment* payment = &getBooking()->getPayments()[i];
            if(payment)
            {
                getBooking()->getPayments().removeOne(*payment);
                paymentTable->removeRow(i);
                getBooking()->setIsModified(true);
            }
        }
    }

}

Booking* MainWindow::getBooking()
{
    return &booking;
}

void MainWindow::setBooking(const Booking &newBooking)
{
    booking = newBooking;
}

void MainWindow::OnEmployeeCreated()
{
    qDebug() << "OnEmployeeCreated";
    auto EmployeeTable = ui->tableWidget_3;
    int count = EmployeeTable->rowCount();


    Employee employee;

    auto InputValue = QInputDialog::getText(this,"Create Employee","Name of the new employee");

    if(InputValue == "")
    {
        QMessageBox::warning(this, "Invalid Name", "Empty name was provided. Try again");
        return;
    }

    QRegularExpression regex("^[A-Za-z ]+$");

    if (!regex.match(InputValue).hasMatch()) {
        QMessageBox::warning(this, "Invalid Name", "Name must contain only letters (A-Z, a-z)");
        return;
    }

    employee.setName(InputValue);

    auto employeeEmail = InputValue.trimmed().replace(" ",".").toLower() + "@mxhotel.com";
    InputValue = QInputDialog::getText(this,"Create Employee","Email of the new employee");
    if(InputValue == "" || InputValue == " " || InputValue.contains(" ") || !InputValue.contains("@"))
    {
        employee.setEmail(employeeEmail);
    }else
    {

        employee.setEmail(InputValue);
    }


    employee.setLogin(InputValue);
    InputValue.clear();

    int ID = GetNewEmployeeID();
    InputValue = "mx_"+QString::number(ID);
    employee.setLogin(InputValue);
    InputValue.clear();

    InputValue = QInputDialog::getText(this,"Create Employee","Password of the new employee");
    if(InputValue == "" || InputValue == " ")
    {
        QMessageBox::warning(this, "Invalid password", "Invalid password. It will be randomized");
        const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
        QString randomPassword;
        for (int i = 0; i < 10; ++i) {
            int index = rand() % possibleCharacters.length();
            randomPassword.append(possibleCharacters[index]);
        }
        InputValue = randomPassword;
    }
    employee.setPassword(InputValue);
    InputValue.clear();

    InputValue = QInputDialog::getText(this,"Create Employee","Phone number of the new employee");
    regex = QRegularExpression("^\\+?[\\d\\s-]+$");
    if(!regex.match(InputValue).hasMatch())
    {
        InputValue = "0";
    }
    employee.setPhoneNumber(InputValue);
    InputValue.clear();

    InputValue = QInputDialog::getText(this,"Create Employee","Role of the new employee");
    regex = QRegularExpression("^[A-Za-z]+$");
    if(!regex.match(InputValue).hasMatch())
    {

        InputValue = "None";
    }
    employee.setRole(InputValue);
    InputValue.clear();

    InputValue = QInputDialog::getText(this,"Create Employee","Salary of the new employee");
    regex = QRegularExpression("^[0-9.,]+$");
    if(!regex.match(InputValue).hasMatch())
    {
        InputValue = "0.0";
    }
    employee.setSalary(InputValue.remove(',').toFloat());
    InputValue.clear();


    if(employees.contains(employee))
    {
        QMessageBox::warning(this,"Create Employee","Such employee already exists");
        return;
    }
    employees.push_back(employee);

    EmployeeTable->insertRow(count);
    qDebug() << "rows: "<<EmployeeTable->rowCount() << " columns: " << EmployeeTable->columnCount();
    int columnNum = EmployeeTable->columnCount();
    EmployeeTable->blockSignals(true);
    for(int i = 0 ; i < columnNum; ++i)
    {
        QTableWidgetItem* item = new QTableWidgetItem("Text");

        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
       EmployeeTable->setItem(EmployeeTable->rowCount()-1,i,item);
        auto Widget = EmployeeTable->item(EmployeeTable->rowCount()-1,i);
        if(Widget)
        {
            Widget->setBackground(QColor(0x545454));
        }
        else
        {
            qDebug() << "Reading nullptr;";
        }
    }
    EmployeeTable->blockSignals(false);
    QMessageBox::information(this,"Employee Created","Congratulations. Created an employee.");
    LoadEmployees();
    qDebug() << "Loaded employees";
   // EmployeeTable->resizeColumnsToContents();
}
void MainWindow::OnEmployeeUpdated()
{
    if(EmployeeModified)
    {
        auto reply = QMessageBox::question(this,"Save changes - EMPLOYEE","Are you sure you want to save the changes? Please check once again before saving the changes!!!");
        if(reply == QMessageBox::No)
        {
            return;
        }else
        {
            EmployeeModified = false;
            QMessageBox::information(this,"Save changes - EMPLOYEE","Saved changes.");
        }
    }
    else
    {
        QMessageBox::warning(this,"Save changes - EMPLOYEE", "You are trying to save empty changes. Request denied.");
    }
};
void MainWindow::OnEmployeeRemoved()
{
    QSet<int> rows;

    // Change information on booking

    auto EmployeeTable = ui->tableWidget_3;

    QList<QTableWidgetItem*> selectedItems = EmployeeTable->selectedItems();

    if(selectedItems.empty())
    {
        auto RowMax = EmployeeTable->rowCount();
        if(RowMax <= 0)
        {
            return;
        }
        auto NameWidget = EmployeeTable->item(RowMax-1,0);
        if(!NameWidget)
        {
            return;
        }
        auto Name = (NameWidget->text() != "" ? NameWidget->text() : "Empty cell");
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,"Remove employee",""
                                                               "NO SPECIFIC EMPLOYEE SELECTED. Are you sure you want to remove the last employee from list ( "+Name+ " ) from the system?",
                                      QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::Yes)
        {
            auto employee = GetEmployeeByName(Name);
            if(employee)
            {
                bool IsSuccess  = false;

                // for each selected row
                employees.removeOne(*employee);
                if(IsSuccess)
                    QMessageBox::information(this,"REMOVE SUCCESS",employee->getName() + " has been removed!");
                EmployeeModified = true;
                EmployeeTable->removeRow(RowMax-1);
            }else
            {
                QMessageBox::warning(this,"Remove Customer","Error while removing the last employee from the employee table");
            }
        }
        return;
    }
    // Update UI
    for(auto& obj : selectedItems)
    {
        rows.insert(obj->row());
    }

    for(auto& Row : rows)
    {
        auto NameWidget = EmployeeTable->item(Row,0);
        if(!NameWidget)
        {
            continue;
        }
        auto employee = GetEmployeeByName(NameWidget->text());
        bool IsSuccess = false;
        QString name = employee->getName();
        if(!employee)
        {
            continue;
        }
        auto reply = QMessageBox::question(this,"Remove employee","You are trying to remove " + name + " . Are you sure?");
        if(reply != QMessageBox::Yes)
        {
            continue;
        }
        IsSuccess  = true;
        // for each selected row
        EmployeeTable->removeRow(Row);
        employees.removeOne(*employee);
        if(IsSuccess)
        {

            QMessageBox::information(this,"REMOVE SUCCESS",name + " has been removed!");
            EmployeeModified = true;
        }
    }
    if(EmployeeTable->rowCount() == 0)
    {
        EmployeeModified = false;
    }
}

void MainWindow::OnEmployeeCleared()
{
    qDebug() << "Cleared search";

    ui->lineEdit_6->clear();
    ui->lineEdit_7->clear();
    ui->lineEdit_8->clear();

    employees.clear();
    ui->tableWidget_3->clearContents();
    ui->tableWidget_3->setRowCount(0);
}

void MainWindow::OnSearchEmployee()
{
    QRegularExpression regex("^[A-Za-z]+$");
    QString dataQuery;
    bool ValidName = true;
    bool ValidSurname = true;
    bool ValidPhone = true;
    if(!regex.match(ui->lineEdit_6->text()).hasMatch())// first name
    {
        ValidName = false;
    }

    if(!regex.match(ui->lineEdit_7->text()).hasMatch())// surname
    {
        //QMessageBox::warning(this,"Search Employee","Invalid surname");
        ValidSurname = false;
    }

    regex = QRegularExpression("^[0-9 ]+$");
    if(!regex.match(ui->lineEdit_8->text()).hasMatch())// first name
    {
        ValidPhone = false;
       // QMessageBox::warning(this,"Search Employee","Invalid phone number");
    }

    QString employeeName = "";
    if(ValidName && ValidSurname)
    {
        employeeName =  ui->lineEdit_6->text() + " " + ui->lineEdit_7->text();
        //QMessageBox::warning(this,"Search Employee","Invalid name and surname");
    }else
    {
        if(ValidName)
        {
            employeeName = ui->lineEdit_6->text();
        }else if(ValidSurname)
        {
            employeeName = ui->lineEdit_7->text();
        }
    }
    dataQuery = employeeName + " " + ui->lineEdit_8->text();
    qDebug() << "Sent data:  " << dataQuery;

    bool foundEmployee = false;
    Employee employee;
    if(employeeName != "") // name
    {
        auto employeePtr = GetEmployeeByName(employeeName);
        if(employeePtr)
        {
            qDebug() << "GetEmployeeByName found";
            employeePtr->Print();
            employee = *employeePtr;
            foundEmployee = true;
        }else
        {

        }
    }else
    {
        if(ui->lineEdit_8->text() != "") // phone number
        {

            for (auto& object : employees)
            {
                if(object.getPhoneNumber() == employeeName)
                {
                    object.Print();
                    employee = object;
                    foundEmployee = true;
                    break;
                }
            }
        }
    }

    if(!foundEmployee)
    {
        QMessageBox::warning(this,"Search Employee", "Could not find "+employeeName);
        return;
    }
    qDebug() << "=====================";

    qDebug() << "=====================";
    employees.clear();
    ui->tableWidget_3->clearContents();
    ui->tableWidget_3->setRowCount(0);
    ui->tableWidget_3->insertRow(ui->tableWidget_3->rowCount());

    ui->tableWidget_3->blockSignals(true);
    for(int i = 0 ; i < ui->tableWidget_3->columnCount(); ++i)
    {
        QTableWidgetItem* item = new QTableWidgetItem(" ");

        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget_3->setItem(ui->tableWidget_3->rowCount()-1,i,item);
        auto Widget = ui->tableWidget_3->item(ui->tableWidget_3->rowCount()-1,i);
        if(Widget)
        {
            Widget->setBackground(QColor(0x545454));
        }
        else
        {
            qDebug() << "Reading nullptr;";
        }
    }
    ui->tableWidget_3->blockSignals(false);
    employees.push_back(employee);
    LoadEmployees();

};
/////////////////////////////////////////
/////////////////////////////////////////
/////////////////////////////////////////
void MainWindow::SetBookingPage()
{
// here we will connect buttons and signals events
    connect(ui->tableWidget, &QTableWidget::itemDoubleClicked,
            this, &MainWindow::OnTableItemEditable);
    connect(ui->tableWidget_2, &QTableWidget::itemDoubleClicked,
        this, &MainWindow::OnTableItemEditable);

    ui->tableWidget->setToolTip("Double click a cell to make the row editable/uneditable");
    connect(ui->tableWidget,&QTableWidget::itemChanged,this,&MainWindow::OnTableItemChanged);
    connect(ui->tableWidget_2,&QTableWidget::itemChanged,this,&MainWindow::OnTableItemChanged);

    // savechanges
    connect(ui->pushButton,&QPushButton::clicked,this,&MainWindow::OnSavedChanges);
    // check in
    connect(ui->pushButton_2,&QPushButton::clicked,this,&MainWindow::OnBookingCustomerCheckedIn);
    // check out
    connect(ui->pushButton_3,&QPushButton::clicked,this,&MainWindow::OnBookingCustomerCheckedOut);
    // remove
    connect(ui->pushButton_4,&QPushButton::clicked,this,&MainWindow::OnBookingCustomerRemoved);
    // create
    connect(ui->pushButton_5,&QPushButton::clicked,this,&MainWindow::OnBookingCustomerCreated);
    // ban
    connect(ui->pushButton_6,&QPushButton::clicked,this,&MainWindow::OnBookingCustomerBanned);
    // new booking, reset
    connect(ui->pushButton_7,&QPushButton::clicked,this,&MainWindow::OnNewBooking);
    // change room
    connect(ui->pushButton_10, &QPushButton::clicked,this,&MainWindow::OnChangingRoom);
    // change checkout date
    connect(ui->pushButton_11, &QPushButton::clicked,this,&MainWindow::OnCheckInDateChanged);
    // change checkin date
    connect(ui->pushButton_12, &QPushButton::clicked,this,&MainWindow::OnCheckOutDateChanged);
    // payment, create remove
    connect(ui->pushButton_8,&QPushButton::clicked,this,&MainWindow::OnPaymentAdded);
    connect(ui->pushButton_9,&QPushButton::clicked,this,&MainWindow::OnPaymentRemoved);

    // label booker data
}

void MainWindow::LoadBooking()
{
    this->blockSignals(true);
    Booking* booking = getBooking();
    int RoomNumber = booking->getRoomNumber();
    int Floor = 0;
    if(RoomNumber >= 13)
    {
        Floor = 2;
    }else if(RoomNumber >= 3)
    {
        Floor = 1;
    }
    const auto date = QDateTime::currentDateTime();
    //
    ui->label->setText(booking->getBookingNumber());
    // Created: %Date%
    ui->label_4->setText("Created: "+booking->getCreatedDate().toString("dd MMM yyyy"));
        //Check-in date: %Date%
    ui->label_7->setText("Check-in date: "+booking->getCheckedinDate().date().toString("dd MMM yyyy"));
        //Check-out date: %Date%
    ui->label_5->setText("Check-out date: "+booking->getCheckoutDate().date().toString("dd MMM yyyy"));
        //Room %Number% ( Floor: %Floor%)
    if(booking->getCheckoutDate() >= date)
    {
        if(date > booking->getCheckoutDate())
        {
            ui->label_5->setStyleSheet("color:red;");
        }else
        {
            ui->label_5->setStyleSheet("color:green;");
        }
    }else
    {
        if(this->Themes.IsWhite())
        {
            ui->label_5->setStyleSheet("color:black;");
        }else
        {
            ui->label_5->setStyleSheet("color:white;");
        }
    }
    ui->label_10->setText("Room "+QString::fromStdString(std::to_string(RoomNumber))+
                          " ( Floor "+QString::fromStdString(std::to_string(Floor))+" )");

    ui->label_2->setText(booking->getBookerName()+ " ( "
                         + booking->getBookerEmail()+" ) "
                         + "( " + booking->getBookerPhonenumber() +" )");

    ui->textEdit->setText(booking->getNotes());


    if(booking->getCustomers().empty())
    {
        ui->tableWidget->clearContents();
        ui->tableWidget->setRowCount(0);
    }
    else
    {
        for(int i = 0 ; i < booking->getCustomers().size(); ++i)
        {
            Customer customer = booking->getCustomers()[i];
            int RowCount = ui->tableWidget->rowCount();
            if(i > RowCount-1)
            {
                ui->tableWidget->insertRow(RowCount); // Insert at the end
            }
            ui->tableWidget->setItem(i,0,new QTableWidgetItem(customer.getName()));
            ui->tableWidget->item(i,0)->setFlags(ui->tableWidget->item(i,0)->flags() & ~Qt::ItemIsEditable);

            ui->tableWidget->setItem(i,1,new QTableWidgetItem(customer.getDob().toString("dd.MM.yyyy")));
            ui->tableWidget->item(i,1)->setFlags(ui->tableWidget->item(i,1)->flags() & ~Qt::ItemIsEditable);

            ui->tableWidget->setItem(i,2,new QTableWidgetItem(customer.getPhonenumber()));
            ui->tableWidget->item(i,2)->setFlags(ui->tableWidget->item(i,2)->flags() & ~Qt::ItemIsEditable);

            ui->tableWidget->setItem(i,3,new QTableWidgetItem(customer.getDocumentNumber()));
            ui->tableWidget->item(i,3)->setFlags(ui->tableWidget->item(i,3)->flags() & ~Qt::ItemIsEditable);

            ui->tableWidget->setItem(i,4,new QTableWidgetItem(customer.getDocumentType()));
            ui->tableWidget->item(i,4)->setFlags(ui->tableWidget->item(i,4)->flags() & ~Qt::ItemIsEditable);

            for(int ColumnIndex = 0; ColumnIndex < ui->tableWidget->columnCount();++ColumnIndex)
            {
                ui->tableWidget->item(i,ColumnIndex)->setBackground(QColor(customer.getCheckedIn() ? 0x1C7D17 : 0x545454));
                //0x1C7D17)) : item->setBackground(QColor(0x545454));
            }

        }
    }

    if(booking->getPayments().empty())
    {
        ui->tableWidget_2->clearContents();
        ui->tableWidget_2->setRowCount(0);
    }else
    {
        for(int i = 0 ; i < booking->getPayments().size(); ++i)
        {
            Payment payment = booking->getPayments()[i];
            int RowCount = ui->tableWidget_2->rowCount();
            if(i > RowCount-1)
            {
                ui->tableWidget_2->insertRow(RowCount); // Insert at the end
            }
            ui->tableWidget_2->setItem(i,0,new QTableWidgetItem(QString::fromStdString(QString::number(payment.getAmount(),'f',2).toStdString())));

            ui->tableWidget_2->setItem(i,1,new QTableWidgetItem(payment.getCurrency()));

            ui->tableWidget_2->setItem(i,2,new QTableWidgetItem(payment.getMethod()));
        }
    }

    this->blockSignals(false);
}
void MainWindow::SetHotelMapPage(){

    QList<QPushButton*> buttons = this->findChildren<QPushButton*>();
    for(const auto& obj: buttons)
    {
        if(obj->objectName().contains("Room"))
        {
            //qDebug() << "Found room: "<<obj->objectName();
            connect(obj,&QPushButton::clicked,this,[this,obj](){
                OnRoomInfoRequested(obj->objectName());
            });
        }
    }

}

void MainWindow::SetEmployeesPage(){

    Employee employee("Maksy Creator","admin");
    employee.Print();

    connect(ui->pushButton_27,&QPushButton::clicked,this,&MainWindow::OnEmployeeCreated);
        //Create
    connect(ui->pushButton_29,&QPushButton::clicked,this,&MainWindow::OnEmployeeUpdated);
        //Update
    connect(ui->pushButton_28,&QPushButton::clicked,this,&MainWindow::OnEmployeeRemoved);

    connect(ui->tableWidget_3,&QTableWidget::itemDoubleClicked,this,&MainWindow::OnTableItemEditable);

    connect(ui->tableWidget_3,&QTableWidget::itemChanged,this,&MainWindow::OnTableItemChanged);

    connect(ui->pushButton_34,&QPushButton::clicked,this,&MainWindow::OnEmployeeCleared);

    ui->tableWidget_3->setColumnWidth(0,130);
    ui->tableWidget_3->setColumnWidth(1,100);
    ui->tableWidget_3->setColumnWidth(2,200);
    ui->tableWidget_3->setColumnWidth(3,180);
    ui->tableWidget_3->setColumnWidth(4,80);
    ui->tableWidget_3->setColumnWidth(5,140);
    ui->tableWidget_3->setColumnWidth(6,230);

    connect(ui->pushButton_33,&QPushButton::clicked,this,&MainWindow::OnSearchEmployee);

    Employee max;
    max.setName("Max Test");
    max.setEmail("max.Test@mxhotel.com");
    max.setLogin("adminmax");
    max.setPassword("adminpassword");
    max.setPhoneNumber("+48 929 858 999");
    max.setSalary(2500);

    this->AddEmployee(&max);
    Employee daniel;

    daniel.setName("Daniel Ricardo");
    daniel.setEmail("ricardo.daniel@mxhotel.com");
    daniel.setLogin("tairdanieldanya");
    daniel.setPassword("testgabesoftware");
    daniel.setPhoneNumber("+48 888 582 020");
    daniel.setSalary(505);
    this->AddEmployee(&daniel);





    //Remove

}

void MainWindow::LoadEmployees()
{
    auto EmployeeTable = ui->tableWidget_3;

    if(employees.empty())
    {
        QMessageBox::warning(this,"LoadEmployees","Failed to load empty employees");
    }
    if(EmployeeTable->rowCount() < employees.size())
    {
        qDebug() << "Employee table must have enough rows: "<<EmployeeTable->rowCount() << " " << employees.size();
    }
    int i = 0;
    qDebug() << "Populating table with values";
    EmployeeTable->blockSignals(true);
    for(auto& employee : employees)
    {
        employee.Print();
        auto cellItem = EmployeeTable->item(i,0);
        if(!cellItem)
        {
            qDebug() << "Invalid item at row "<<i<< " column: " << 0;
            continue;
        }else
        {

        }

        cellItem->setText(employee.getName());

        cellItem = EmployeeTable->item(i,1);
        cellItem->setText(employee.getRole());

        cellItem = EmployeeTable->item(i,2);
        cellItem->setText(employee.getEmail());

        cellItem = EmployeeTable->item(i,3);
        cellItem->setText(employee.getPhoneNumber());

        cellItem = EmployeeTable->item(i,4);
        cellItem->setText(QString::number(employee.getSalary()));

        cellItem = EmployeeTable->item(i,5);
        cellItem->setText(employee.getLogin());

        cellItem = EmployeeTable->item(i,6);
        cellItem->setText(employee.getPassword());

        ++i;
    }
    EmployeeTable->blockSignals(false);
}

int MainWindow::GetNewEmployeeID()
{
    // Make request to the database to check the employees count
    return QRandomGenerator::global()->bounded(0,100);
}

void MainWindow::AddEmployee(Employee* employee)
{
    qDebug() << "Creating employee internally";
    if(!employee)
    {
        return;
    }
    auto EmployeeTable = ui->tableWidget_3;
    int count = EmployeeTable->rowCount();




    if(employee->getName() == "")
    {
        QMessageBox::warning(this, "Invalid Name", "Empty name was provided. Try again");
        return;
    }

    QRegularExpression regex("^[A-Za-z0-9 ]+$");

    if (!regex.match(employee->getName()).hasMatch()) {
        QMessageBox::warning(this, "Invalid Name", "Name must contain only letters (A-Z, a-z)\n Name is Test Name.");
        employee->setName("Test Name");
    }

    if(employee->getEmail() == "")
    {
        QString stringName = employee->getName();
        stringName.trimmed().replace(" ",".").toLower() + "@mxhotel.com";
    }


    if(employee->getPassword() == "" || employee->getPassword() == " ")
    {
        QMessageBox::warning(this, "Invalid password", "Invalid password. It will be randomized");
        const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
        QString randomPassword;
        for (int i = 0; i < 10; ++i) {
            int index = rand() % possibleCharacters.length();
            randomPassword.append(possibleCharacters[index]);
        }
        employee->setPassword(randomPassword);
    }

    if(employees.contains(*employee))
    {
        QMessageBox::warning(this,"Create Employee","Such employee already exists");
        return;
    }
    employees.push_back(*employee);

    EmployeeTable->insertRow(count);
    qDebug() << "rows: "<<EmployeeTable->rowCount() << " columns: " << EmployeeTable->columnCount();
    int columnNum = EmployeeTable->columnCount();
    EmployeeTable->blockSignals(true);
    for(int i = 0 ; i < columnNum; ++i)
    {
        QTableWidgetItem* item = new QTableWidgetItem("Text");

        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        EmployeeTable->setItem(EmployeeTable->rowCount()-1,i,item);
        auto Widget = EmployeeTable->item(EmployeeTable->rowCount()-1,i);
        if(Widget)
        {
            Widget->setBackground(QColor(0x545454));
        }
        else
        {
            qDebug() << "Reading nullptr;";
        }
    }
    EmployeeTable->blockSignals(false);
    QMessageBox::information(this,"Employee Created","Congratulations. Created an employee.");
    LoadEmployees();
    qDebug() << "Loaded employee";
}

// This will return the first employee in the system with the given name
Employee *MainWindow::GetEmployeeByName(QString Name)
{
    if(employees.empty() || Name.size() < 3)
    {
        return nullptr;
    }
    for(auto& employee : employees)
    {
        qDebug() << "Comparing " << employee.getName() << "   to   " << Name;
        if(employee.getName() == Name || employee.getName().contains(Name,Qt::CaseInsensitive))
        {
            return &employee;
        }
    }
    return nullptr;
}
void MainWindow::SetCustomersPage(){

    connect(ui->pushButton_30,&QPushButton::clicked,this,&MainWindow::OnCustomerInfoRequested);

}
void MainWindow::setPartnersPage(){

}
void MainWindow::SetNotificationsPage(){

}
void MainWindow::SetRequestsPage(){

}
void MainWindow::SetSettingsPage(){

    connect(ui->comboBox,&QComboBox::currentIndexChanged,this,&MainWindow::UpdateTheme);

}

QString MainWindow::RandomBookingNumber()
{

    QString code;

    for (int i = 0; i < 6; ++i) {
        // Generate a random number between 0 and 25 for uppercase letters (A-Z)
        int randIndex = QRandomGenerator::global()->bounded(26); // 26 uppercase letters

        // Add an uppercase letter (A-Z)
        code.append(QChar('A' + randIndex));
    }
    return code;
}

Payment MainWindow::RandomPayment()
{
    Payment payment;
    payment.PopulateWithRandomValues();
    return payment;
}

