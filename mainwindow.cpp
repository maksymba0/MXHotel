#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSpacerItem>
#include <QTabBar>
#include <QDebug>
#include <QVBoxLayout>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QSqlQuery>
#include "calendardialog.h"
#include <QInputDialog>
#include "employee.h"
#include <QSqlDatabase>
#include <QSqlError>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Themes = ThemeManager(this);


    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);


    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");  // or your server IP
    db.setPort(5432);             // default PostgreSQL port
    db.setDatabaseName("mxhoteldb");
    db.setUserName("postgres");
    db.setPassword("postgresqlpassword1337");

    if(db.open())
    {
        qDebug() << "Connected to the database";
    }else
    {
        QMessageBox::warning(this,"Database connection","Failed to connect to the database");
    }
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

    RoomName.remove("BookingRoom_");
    int RoomNumber = RoomName.toInt();

    if(getBooking()->getIsChangingRoom())
    {
        qDebug() << "Selected Room #" << RoomNumber;
        this->getBooking()->setRoomNumber(RoomNumber);
        getBooking()->setIsChangingRoom(false);
        LoadBooking();
        return;
    }

    // Requesting data

    Booking newbkg;
    Booking* booking = &newbkg;


    //  BOOKING DETAILS
    QSqlQuery query;
    query.prepare(R"(
    SELECT * from Booking
    WHERE room_number = :roomNumber
                  )");
    query.bindValue(":roomNumber",RoomNumber);
    int bookingId = -1;
    if (query.exec()) {
        if (query.next()) {
            booking->setBookingNumber(query.value("booking_number").toString());
            booking->setCreatedDate(query.value("created_date").toDateTime());
            booking->setCheckedinDate(query.value("checkin_date").toDateTime());
            booking->setCheckoutDate(query.value("checkout_date").toDateTime());
            booking->setRoomNumber(query.value("room_number").toInt());
            booking->setBookerName(query.value("booker_name").toString());
            booking->setBookerEmail(query.value("booker_email").toString());
            booking->setBookerPhonenumber(query.value("booker_phonenumber").toString());
            booking->setNotes(query.value("notes").toString());
            bookingId = query.value("booking_id").toInt();
        } else {
            qDebug() << "No booking found for room" << RoomNumber;
            auto response = QMessageBox::question(this,"Map - Retrieve booking","No booking find for this room. Do you Want to book it?");
            if(response == QMessageBox::Yes)
            {
                booking->Clear();

                ui->tableWidget->clearContents();
                ui->tableWidget->setRowCount(0);
                ui->tableWidget_2->clearContents();
                ui->tableWidget_2->setRowCount(0);
                OnNewBooking();
            }else
            {
                //No
            }
            return;
        }
    } else {
        qDebug() << "Booking query failed:" << query.lastError() << " .Please try create a new booking";
        return;
    }


    if(bookingId)
    {
        ui->tableWidget->clearContents();
        ui->tableWidget->setRowCount(0);
        ui->tableWidget_2->clearContents();
        ui->tableWidget_2->setRowCount(0);
        // CUSTOMER DETAILS
        QSqlQuery customerQuery;
        customerQuery.prepare(R"(
    SELECT * from Booking_Customer WHERE booking_id = :bookingId
                  )");
        customerQuery.bindValue(":bookingId",bookingId);
        if(customerQuery.exec())
        {
            while(customerQuery.next())
            {
                Customer customer;
                customer.setName(customerQuery.value("name").toString());
                customer.setEmail(customerQuery.value("email").toString());
                customer.setPhonenumber(customerQuery.value("phonenumber").toString());
                customer.setDocumentType(customerQuery.value("document_type").toString());
                customer.setDocumentNumber(customerQuery.value("document_number").toString());
                customer.setAge();
                customer.setDob(customerQuery.value("dob").toDate());
                customer.setCheckedIn(customerQuery.value("checked_in").toBool());

                booking->addCustomer(customer);
            }
        }else {
            qDebug() << "Failed to fetch customers:" << customerQuery.lastError();
        }
        // PAYMENT DETAILS
        QSqlQuery paymentQuery;
        paymentQuery.prepare(R"(
    SELECT * from Payment WHERE booking_id = :bookingId
                  )");
        paymentQuery.bindValue(":bookingId",bookingId);
        if(paymentQuery.exec())
        {
            while(paymentQuery.next())
            {
                Payment payment;
                payment.setAmount(paymentQuery.value("amount").toFloat());
                payment.setCurrency(paymentQuery.value("currency").toString());
                payment.setMethod(paymentQuery.value("method").toString());
                booking->addPayment(payment);
            }
        }else {
            qDebug() << "Failed to fetch payments:" << paymentQuery.lastError();
        }
    }

    booking->setIsBeingCreated(false);
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
        (ui->tableWidget_3 == item->tableWidget()) || // employees
        (ui->tableWidget_4 == item->tableWidget())) // unique customer
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

    }else if(item->tableWidget() == ui->tableWidget_4) // CUSTOMER TABLE
    {
        auto CustomerTable = ui->tableWidget_4;
        if(uCustomers.empty() || row > uCustomers.size())
        {
            qDebug() << "Outside of index boundary";
            return;
        }
        UCustomer* customer = &uCustomers[row];
        if(customer)
        {
            switch(item->column())
            {
            case 0:
                customer->setID(CustomerTable->item(row,0)->text().toInt());
                break; // name
            case 1:

                customer->setBookingNumber(CustomerTable->item(row,1)->text());

                break; // role
            case 2:

                customer->setName(CustomerTable->item(row,2)->text());
                break; // email
            case 3:

                customer->setLastRoom(CustomerTable->item(row,3)->text().toInt());
                break; // phone number
            case 4:

                customer->setPhonenumber(CustomerTable->item(row,4)->text());
                break; //salary
            case 5:

                customer->setEmail(CustomerTable->item(row,5)->text());
                break;// login
            case 6:
                customer->setInformation(CustomerTable->item(row,6)->text());
                break; // password
            default:
                return;
            }
            customer->setIsModified(true);
            this->DBUpdateCustomer();

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

            getBooking()->setBookingNumber(this->RandomBookingNumber());
            if(!DBExistsBooking())
            {

                if(DBCreateNewBooking())
                {
                    // Create - Success

                    QMessageBox::information(this,"Create booking - success","Booking has been created.");
                    this->getBooking()->setIsBeingCreated(false);
                    this->getBooking()->setIsModified(false);
                    LoadBooking();
                }
                else
                {
                    QMessageBox::warning(this,"Create booking- failure","Failed to create booking");
                    // Create - Failure
                }
                //Exists
            }
            else
            {
                QMessageBox::warning(this,"Create booking- failure","Failed to create booking --- SUCH BOOKING ALREADY EXISTS");
                //Non Exists
            }
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
                if(!DBUpdateBooking())
                {
                    QMessageBox::warning(this,"Save change - failure","Failed to update current booking.");
                }else
                {
                    QMessageBox::information(this,"Save change - success","Booking has been updated.");
                    getBooking()->setIsModified(false);
                }
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
        if(ui->lineEdit_5->text().size() == 6)
        {
            hasBookingNum = true;
        }
    }

    //qDebug() << ui->lineEdit_3->text() << " " << ui->lineEdit_4->text() << " " << ui->lineEdit_5->text();
    QString name = hasName && hasSurname ? ui->lineEdit->text() + " " + ui->lineEdit_2->text()
                   : hasName ?  ui->lineEdit->text()
                   : hasSurname ? ui->lineEdit_2->text()
                   : "";
    QString phone = hasPhone ? ui->lineEdit_3->text() : "";
    QString email = hasEmail ? ui->lineEdit_4->text() : "";
    QString bookingNum = hasBookingNum ? ui->lineEdit_5->text() : "";


    QSqlQuery SearchCustomerQuery;


    QString Query = QString("SELECT * from Unique_Customer WHERE 1=1");

    if(name != "")
    {
        Query += " AND name ILIKE :name";
    }
    if(phone!= "")
    {
        Query += " AND phone_number= :phone_number";
    }
    if(email != "")
    {
        Query += " AND email = :email";
    }
    if(bookingNum != "")
    {
        Query += " AND booking_number = :booking_number";
    }



    SearchCustomerQuery.prepare(Query);


    if(name != "")
    {
        SearchCustomerQuery.bindValue(":name",name);
    }
    if(phone!= "")
    {
        SearchCustomerQuery.bindValue(":phone_number",phone);
    }
    if(email != "")
    {
        SearchCustomerQuery.bindValue(":email",email);
    }
    if(bookingNum != "")
    {
        SearchCustomerQuery.bindValue(":booking_number",bookingNum);
    }

    if(SearchCustomerQuery.exec())
    {
        while(SearchCustomerQuery.next())
        {
            UCustomer ucustomer;
            ucustomer.setBookingNumber(SearchCustomerQuery.value("booking_number").toString());
            ucustomer.setPhonenumber(SearchCustomerQuery.value("phone_number").toString());
            ucustomer.setEmail(SearchCustomerQuery.value("email").toString());
            ucustomer.setID(SearchCustomerQuery.value("id").toInt());
            ucustomer.setInformation(SearchCustomerQuery.value("information").toString());
            ucustomer.setLastRoom(SearchCustomerQuery.value("last_room").toInt());
            ucustomer.setName(SearchCustomerQuery.value("name").toString());
            AddCustomer(&ucustomer);
            LoadCustomers();
        }
    }else
    {
        QMessageBox::warning(this,"Search Unique Customer - Failed","Internal failure to search customer:"+SearchCustomerQuery.lastError().text());
    }


}

void MainWindow::OnNewCustomerCreated()
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
        if(ui->lineEdit_5->text().size() == 6)
        {
            hasBookingNum = true;
        }
    }

    //qDebug() << ui->lineEdit_3->text() << " " << ui->lineEdit_4->text() << " " << ui->lineEdit_5->text();
    QString name = hasName && hasSurname ? ui->lineEdit->text() + " " + ui->lineEdit_2->text()
                   : hasName ?  ui->lineEdit->text()
                   : hasSurname ? ui->lineEdit_2->text()
                                         : "";
    QString phone = hasPhone ? ui->lineEdit_3->text() : "";
    QString email = hasEmail ? ui->lineEdit_4->text() : "";
    QString bookingNum = hasBookingNum ? ui->lineEdit_5->text() : "";

    if(!hasName)
    {
        QMessageBox::warning(this,"Create customer","Failed to create a new customer: Missing name.");
        return;
    }

    QString Query = "SELECT COUNT(*) FROM Unique_Customer WHERE 1=1";

    if(name!= "")
    {
        Query += " AND name=:name";
    }
    if(phone!= "")
    {
        Query += " AND phone_number = :phone_number";
    }
    if(email!= "")
    {
        Query += " AND email=:email";
    }
    if(bookingNum != "")
    {
        Query += " AND booking_number=:booking_number";
    }

    QSqlQuery CustomerExistsQuery;
    CustomerExistsQuery.prepare(Query);
    if(name!= "")
    {
        CustomerExistsQuery.bindValue(":name",name);
    }
    if(phone!= "")
    {
        CustomerExistsQuery.bindValue(":phone_number",phone);
    }
    if(email!= "")
    {
      CustomerExistsQuery.bindValue(":email",email);
    }
    if(bookingNum != "")
    {
        CustomerExistsQuery.bindValue(":booking_number",bookingNum);
    }


    if(CustomerExistsQuery.exec())
    {
        if (CustomerExistsQuery.next()) {
            if (CustomerExistsQuery.value(0).toInt() > 0) {
                QMessageBox::warning(this,"Create Unique Customer","Failed to create. Already exists");
                return;
            }
        } else {
            QMessageBox::warning(this,"Create Unique Customer","Internal db error: " + CustomerExistsQuery.lastError().text());
            return;
        }

    } else {
        QMessageBox::warning(this,"Create Unique Customer","Internal db error: " + CustomerExistsQuery.lastError().text());
        return;
    }

    QSqlQuery CreateUniqueCustomer;
    CreateUniqueCustomer.prepare(R"(INSERT INTO Unique_Customer (booking_number,name,phone_number,email)
    VALUES (:booking_number,:name,:phone_number,:email)
    RETURNING id
)");


    CreateUniqueCustomer.bindValue(":name", name);
    CreateUniqueCustomer.bindValue(":booking_number", bookingNum);
    CreateUniqueCustomer.bindValue(":phone_number", phone);
    CreateUniqueCustomer.bindValue(":email", email);
    // Here we will get from the database the ID and status if creating new one, or we already have the existing user with this name and data.
    // from Booking number we will get the last room
    int newID = -1;
    if (CreateUniqueCustomer.exec() && CreateUniqueCustomer.next()) {
        newID = CreateUniqueCustomer.value(0).toInt();
    }

    auto CustomerTable = ui->tableWidget_4;
    auto count = CustomerTable->rowCount();
    CustomerTable->insertRow(count);
    qDebug() << "rows: "<<CustomerTable->rowCount();
    int columnNum = CustomerTable->columnCount();

    UCustomer customer;
    customer.setName(name);
    customer.setEmail(email);
    customer.setPhonenumber(phone);
    customer.setBookingNumber(bookingNum);
    customer.setID(newID);

    uCustomers.push_back(customer);
    CustomerTable->blockSignals(true);
    for(int i = 0 ; i < columnNum; ++i)
    {
        QTableWidgetItem* item = new QTableWidgetItem(" ");
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        CustomerTable->setItem(CustomerTable->rowCount()-1,i,item);
        auto Widget = CustomerTable->item(CustomerTable->rowCount()-1,i);
        if(Widget)
        {
            Widget->setBackground(QColor(0x545454));
        }
        else
        {
            qDebug() << "Reading nullptr;";
        }
    }

    CustomerTable->blockSignals(false);
    QMessageBox::information(this,"Create Customer","Successfully created a customer.");

    LoadCustomers();

}

void MainWindow::OnSelectedCustomerRemoved()
{
    qDebug() << "OnCustomerRemoved";
    QSet<int> rows;

    // Change information on booking
    auto CustomerTable = ui->tableWidget_4;

    QList<QTableWidgetItem*> selectedItems = CustomerTable->selectedItems();

    if(selectedItems.empty())
    {
        return;
    }



    if (QMessageBox::question(this, "Confirm Deletion", "Are you sure you want to delete this customer?") != QMessageBox::Yes) {
        return;
    }

    // Update UI
    for(auto& obj : selectedItems)
    {
        rows.insert(obj->row());
    }

    for(auto& Row : rows)
    {
        auto NameWidget = CustomerTable->item(Row,0);
        if(!NameWidget)
        {
            continue;
        }

        QString name = CustomerTable->item(Row,2)->text();//ui->lineEdit->text() + " " + ui->lineEdit_2->text();
        QString phone = CustomerTable->item(Row,4)->text();;
        QString ID = CustomerTable->item(Row,0)->text();
        QString email = CustomerTable->item(Row,5   )->text();
        QString bookingNum = CustomerTable->item(Row,1)->text();
        bool hasName = name != "";
        bool hasPhone = phone != "";
        bool hasID = ID != "";
        bool hasEmail = email != "";
        bool hasBookingNum = bookingNum != "";

        if(!hasName && !hasPhone && !hasID && !hasEmail && !hasBookingNum)
        {
            QMessageBox::warning(this,"Remove customer - failure","Failed to remove empty customer.");
            return;
        }
        QString Query = QString("SELECT COUNT(*) FROM Unique_Customer WHERE 1=1");

        if(hasName)
        {
            Query += " AND name = :name";
        }

        if(hasPhone)
        {
            Query += " AND phone_number = :phone_number";
        }

        if(hasID)
        {
            Query += " AND id = :id";
        }

        if(hasEmail)
        {
            Query += " AND email = :email";
        }

        if(hasBookingNum)
        {
            Query += " AND booking_number= :booking_number";
        }

        QSqlQuery ExistsCustomer;// just an extra check if it exists
        ExistsCustomer.prepare(Query);

        if(hasName)
        {
            ExistsCustomer.bindValue(":name",name);
        }

        if(hasPhone)
        {
            ExistsCustomer.bindValue(":phone_number",phone);
        }

        if(hasID)
        {
            ExistsCustomer.bindValue(":id",ID);
        }

        if(hasEmail)
        {
            ExistsCustomer.bindValue(":email",email);
        }

        if(hasBookingNum)
        {
            ExistsCustomer.bindValue(":booking_number",bookingNum);
        }


        if(ExistsCustomer.exec())
        {
            if(ExistsCustomer.next())
            {
                if(ExistsCustomer.value(0).toInt() <= 0)
                {
                    QMessageBox::warning(this,"Remove Customer","This customer doesn't exist on database.");
                    return;
                }
            }
        }
        else
        {
            QMessageBox::warning(this,"Remove Customer","Internal error to find existing one:"+ExistsCustomer.lastError().text());
            return;
        }

        QSqlQuery RemoveCustomerQuery;
        RemoveCustomerQuery.prepare("DELETE FROM Unique_Customer WHERE id = :id");
        RemoveCustomerQuery.bindValue(":id",ID);
        if (!RemoveCustomerQuery.exec()) {
            QMessageBox::warning(this, "Delete failed", "Could not delete customer: " + RemoveCustomerQuery.lastError().text());
            return;
        }

        bool IsSuccess = false;


        IsSuccess  = true;
        // for each selected row
        CustomerTable->removeRow(Row);
        auto customer = GetCustomerByID(ID.toInt());
        if(customer)
        {
            uCustomers.removeOne(*customer);
        }
        if(IsSuccess)
        {
            QMessageBox::information(this,"REMOVE SUCCESS",name + " has been removed!");
        }else
        {
            QMessageBox::warning(this,"REMOVE FAILURE",name + " has not been removed due to an error!");
        }
    }

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

// Database DB Related

bool MainWindow::DBExistsBooking()
{
    auto booking = getBooking();

    // Step 1: Check for duplicate booking number
    QSqlQuery checkNumber;
    checkNumber.prepare("SELECT COUNT(*) FROM Booking WHERE booking_number = :bookingNumber");
    checkNumber.bindValue(":bookingNumber", booking->getBookingNumber());

    if (!checkNumber.exec() || !checkNumber.next()) {
        qDebug() << "Booking number check failed:" << checkNumber.lastError();
        return true;
    }

    if (checkNumber.value(0).toInt() > 0) {
        QMessageBox::warning(this, "Booking Error", "A booking with this number already exists.");
        return true;
    }

    // Step 2: Check for booking conflict by room/date
    QSqlQuery checkConflict;
    checkConflict.prepare(R"(
        SELECT COUNT(*) FROM Booking
        WHERE room_number = :roomNumber
        AND NOT (
            checkout_date <= :checkin OR
            checkin_date >= :checkout
        )
    )");
    checkConflict.bindValue(":roomNumber", booking->getRoomNumber());
    checkConflict.bindValue(":checkin", booking->getCheckedinDate());
    checkConflict.bindValue(":checkout", booking->getCheckoutDate());

    if (!checkConflict.exec() || !checkConflict.next()) {
        qDebug() << "Room conflict check failed:" << checkConflict.lastError();
        return true;
    }

    if (checkConflict.value(0).toInt() > 0) {
        QMessageBox::warning(this, "Room Conflict", "This room is already booked for the selected dates.");
        return true;
    }

    return false;


}

bool MainWindow::DBCreateNewBooking()
{
    auto booking = getBooking();
    int bookingId = -1;

// BOOKING


    QSqlQuery insert;
    insert.prepare(R"(
        INSERT INTO Booking (booking_number, created_date, checkin_date, checkout_date, room_number,
                             booker_name, booker_email, booker_phonenumber, notes)
        VALUES (:bookingNumber, :created, :checkin, :checkout, :room,
                :name, :email, :phone, :notes)
        RETURNING booking_id
    )");
    insert.bindValue(":bookingNumber", booking->getBookingNumber());
    insert.bindValue(":created", booking->getCreatedDate());
    insert.bindValue(":checkin", booking->getCheckedinDate());
    insert.bindValue(":checkout", booking->getCheckoutDate());
    insert.bindValue(":room", booking->getRoomNumber());
    insert.bindValue(":name", booking->getBookerName());
    insert.bindValue(":email", booking->getBookerEmail());
    insert.bindValue(":phone", booking->getBookerPhonenumber());
    insert.bindValue(":notes", booking->getNotes());

    if (!insert.exec()) {
        qDebug() << "Insert failed:" << insert.lastError();
        return false;
    }
    if (insert.next()) {
        bookingId = insert.value(0).toInt();
    } else {
        qDebug() << "Failed to retrieve booking ID.";
        return false;
    }

// BOOKING CUSTOMER

    for (auto& customer : booking->getCustomers()) {
        QSqlQuery customerInsert;
        customerInsert.prepare(R"(
        INSERT INTO Booking_Customer (booking_id, name, email, phonenumber, document_type,
                              document_number, age, dob, checked_in)
        VALUES (:bookingId, :name, :email, :phone, :docType,
                :docNum, :age, :dob, :checkedIn)
        )");
        customerInsert.bindValue(":bookingId", bookingId);
        customerInsert.bindValue(":name", customer.getName());
        customerInsert.bindValue(":email", customer.getEmail());
        customerInsert.bindValue(":phone", customer.getPhonenumber());
        customerInsert.bindValue(":docType", customer.getDocumentType());
        customerInsert.bindValue(":docNum", customer.getDocumentNumber());
        customerInsert.bindValue(":age", customer.getAge());
        customerInsert.bindValue(":dob", customer.getDob());
        customerInsert.bindValue(":checkedIn", customer.getCheckedIn());

        if (!customerInsert.exec()) {
            qDebug() << "Failed to insert customer:" << customerInsert.lastError();
            return false;
        }
    }

// PAYMENT
    for (auto& payment : booking->getPayments()) {
        QSqlQuery paymentInsert;
        paymentInsert.prepare(R"(
        INSERT INTO Payment (booking_id, amount, currency, method)
        VALUES (:bookingId, :amount, :currency, :method)
    )");
        paymentInsert.bindValue(":bookingId", bookingId);
        paymentInsert.bindValue(":amount", payment.getAmount());
        paymentInsert.bindValue(":currency", payment.getCurrency());
        paymentInsert.bindValue(":method", payment.getMethod());

        if (!paymentInsert.exec()) {
            qDebug() << "Failed to insert payment:" << paymentInsert.lastError();
            return false;
        }
    }

    return true;
}
bool MainWindow::DBUpdateBooking()
{
    auto booking = getBooking();

    // First, get the booking ID from booking_number
    QSqlQuery getIdQuery;
    getIdQuery.prepare("SELECT booking_id FROM Booking WHERE booking_number = :bookingNumber");
    getIdQuery.bindValue(":bookingNumber", booking->getBookingNumber());

    int bookingId = -1;
    if (getIdQuery.exec() && getIdQuery.next()) {
        bookingId = getIdQuery.value(0).toInt();
    } else {
        qDebug() << "Failed to find booking ID for update:" << getIdQuery.lastError();
        return false;
    }

    // 1. Update Booking table
    QSqlQuery update;
    update.prepare(R"(
        UPDATE Booking SET
            created_date = :created,
            checkin_date = :checkin,
            checkout_date = :checkout,
            room_number = :room,
            booker_name = :name,
            booker_email = :email,
            booker_phonenumber = :phone,
            notes = :notes
        WHERE booking_id = :bookingId
    )");

    update.bindValue(":created", booking->getCreatedDate());
    update.bindValue(":checkin", booking->getCheckedinDate());
    update.bindValue(":checkout", booking->getCheckoutDate());
    update.bindValue(":room", booking->getRoomNumber());
    update.bindValue(":name", booking->getBookerName());
    update.bindValue(":email", booking->getBookerEmail());
    update.bindValue(":phone", booking->getBookerPhonenumber());
    update.bindValue(":notes", booking->getNotes());
    update.bindValue(":bookingId", bookingId);

    if (!update.exec()) {
        qDebug() << "Failed to update booking:" << update.lastError();
        return false;
    }

    // 2. Delete previous customers
    QSqlQuery deleteCustomers;
    deleteCustomers.prepare("DELETE FROM Booking_Customer WHERE booking_id = :bookingId");
    deleteCustomers.bindValue(":bookingId", bookingId);
    if (!deleteCustomers.exec()) {
        qDebug() << "Failed to delete old customers:" << deleteCustomers.lastError();
        return false;
    }

    // 3. Re-insert updated customers
    for (auto& customer : booking->getCustomers()) {
        QSqlQuery customerInsert;
        customerInsert.prepare(R"(
            INSERT INTO Booking_Customer (booking_id, name, email, phonenumber, document_type,
                                          document_number, age, dob, checked_in)
            VALUES (:bookingId, :name, :email, :phone, :docType, :docNum, :age, :dob, :checkedIn)
        )");

        customerInsert.bindValue(":bookingId", bookingId);
        customerInsert.bindValue(":name", customer.getName());
        customerInsert.bindValue(":email", customer.getEmail());
        customerInsert.bindValue(":phone", customer.getPhonenumber());
        customerInsert.bindValue(":docType", customer.getDocumentType());
        customerInsert.bindValue(":docNum", customer.getDocumentNumber());
        customerInsert.bindValue(":age", customer.getAge());
        customerInsert.bindValue(":dob", customer.getDob());
        customerInsert.bindValue(":checkedIn", customer.getCheckedIn());

        if (!customerInsert.exec()) {
            qDebug() << "Failed to insert updated customer:" << customerInsert.lastError();
            return false;
        }
    }

    // 4. Delete previous payments
    QSqlQuery deletePayments;
    deletePayments.prepare("DELETE FROM Payment WHERE booking_id = :bookingId");
    deletePayments.bindValue(":bookingId", bookingId);
    if (!deletePayments.exec()) {
        qDebug() << "Failed to delete old payments:" << deletePayments.lastError();
        return false;
    }

    // 5. Re-insert updated payments
    for (auto& payment : booking->getPayments()) {
        QSqlQuery paymentInsert;
        paymentInsert.prepare(R"(
            INSERT INTO Payment (booking_id, amount, currency, method)
            VALUES (:bookingId, :amount, :currency, :method)
        )");

        paymentInsert.bindValue(":bookingId", bookingId);
        paymentInsert.bindValue(":amount", payment.getAmount());
        paymentInsert.bindValue(":currency", payment.getCurrency());
        paymentInsert.bindValue(":method", payment.getMethod());

        if (!paymentInsert.exec()) {
            qDebug() << "Failed to insert updated payment:" << paymentInsert.lastError();
            return false;
        }
    }

    return true;
}

bool MainWindow::DBClearBooking()
{

    return true;
}
bool MainWindow::DBRemoveBooking()
{

    return true;
}

bool MainWindow::DBCreateEmployee()
{

}

bool MainWindow::DBRemoveEmployee()
{

}

bool MainWindow::DBUpdateCustomer()
{
    int count = 0;
    for(auto& customer : uCustomers)
    {
        if(customer.getIsModified() == false)
        {
            continue;
        }

        QSqlQuery update;
        update.prepare(R"(
        UPDATE Unique_Customer SET
            booking_number = :booking_number,
            name = :name,
            last_room= :last_room,
            phone_number= :phone_number,
            email= :email,
            information= :information
        WHERE id = :id
    )");

        update.bindValue(":booking_number", customer.getBookingNumber());
        update.bindValue(":name", customer.getName());
        update.bindValue(":last_room", customer.getLastRoom());
        update.bindValue(":phone_number", customer.getPhonenumber());
        update.bindValue(":email", customer.getEmail());
        update.bindValue(":information", customer.getInformation());
        update.bindValue(":id", customer.getID());

        if (!update.exec()) {
            QMessageBox::warning(this,"Update UCustomer - Failed","Failed to update ucustomer:" + update.lastError().text());
            continue;
        }else
        {
            customer.setIsModified(false);
            count++;
        }

    }
    QMessageBox::information(this,"Update Customer  - Success"," Successfully updated " + QString::number(count) + " customers");
    return true;
}

void MainWindow::OnEmployeeCreated()
{
    qDebug() << "OnEmployeeCreated";
    auto EmployeeTable = ui->tableWidget_3;
    int count = EmployeeTable->rowCount();


    QString name;
    QString role;
    QString email;
    QString phone_number;
    float salary;
    QString login = "";
    QString password;


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

    name = InputValue;

    auto employeeEmail = InputValue.trimmed().replace(" ",".").toLower() + "@mxhotel.com";
    InputValue = QInputDialog::getText(this,"Create Employee","Email of the new employee");
    if(InputValue.isEmpty() || InputValue == "" || InputValue == " " || InputValue.contains(" ") || !InputValue.contains("@"))
    {
        email = employeeEmail;
    }else
    {

        email = InputValue;
    }


    QSqlQuery IDQuery;
    IDQuery.prepare(R"(
    SELECT COUNT(*) from Employee;
)");
    if(IDQuery.exec())
    {
        if(IDQuery.next())
        {
         login = IDQuery.value(0).toString();
        }
    }
    if(login == "")
    {
        login = QString::number(QRandomGenerator::global()->bounded(0,9999));
    }
    qDebug() << "LOGIN IS "<<login;
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
    password = (InputValue);
    InputValue.clear();

    InputValue = QInputDialog::getText(this,"Create Employee","Phone number of the new employee");
    regex = QRegularExpression("^\\+?[\\d\\s-]+$");
    if(!regex.match(InputValue).hasMatch())
    {
        InputValue = "0";
    }
    phone_number = (InputValue);
    InputValue.clear();

    InputValue = QInputDialog::getText(this,"Create Employee","Role of the new employee");
    regex = QRegularExpression("^[A-Za-z]+$");
    if(!regex.match(InputValue).hasMatch())
    {

        InputValue = "None";
    }
    role = (InputValue);
    InputValue.clear();

    InputValue = QInputDialog::getText(this,"Create Employee","Salary of the new employee");
    regex = QRegularExpression("^[0-9.,]+$");
    if(!regex.match(InputValue).hasMatch())
    {
        InputValue = "0.0";
    }
    salary = (InputValue.remove(',').toFloat());
    InputValue.clear();

    QSqlQuery EmployeeExistsQuery;
    EmployeeExistsQuery.prepare(R"(
    SELECT COUNT(*) FROM Employee
    WHERE name = :name
    AND role = :role
    AND email = :email
    AND phone_number = :phone_number
    )");
    EmployeeExistsQuery.bindValue(":name",name);
    EmployeeExistsQuery.bindValue(":role",role);
    EmployeeExistsQuery.bindValue(":email",email);
    EmployeeExistsQuery.bindValue(":phone_number",phone_number);
    EmployeeExistsQuery.bindValue(":salary",salary);
    EmployeeExistsQuery.bindValue(":login",login);
    EmployeeExistsQuery.bindValue(":password",password);

    if(EmployeeExistsQuery.exec())
    {
        if(EmployeeExistsQuery.next() && EmployeeExistsQuery.value(0).toInt() > 0)
        {
            QMessageBox::warning(this,"Create Employee", name + " employee already exists");
            return;
        }

            QSqlQuery CreateEmployeeQuery;
            CreateEmployeeQuery.prepare(R"(
            INSERT INTO Employee (name,role,email,phone_number,salary,login,password)
            VALUES (:name,:role,:email,:phone_number,:salary,:login,:password)
)");
            CreateEmployeeQuery.bindValue(":name",name);
            CreateEmployeeQuery.bindValue(":role",role);
            CreateEmployeeQuery.bindValue(":email",email);
            CreateEmployeeQuery.bindValue(":phone_number",phone_number);
            CreateEmployeeQuery.bindValue(":salary",salary);
            CreateEmployeeQuery.bindValue(":login",login);
            qDebug()<< "Login:" + login;
            CreateEmployeeQuery.bindValue(":password",password);

            if(CreateEmployeeQuery.exec())
            {
                QMessageBox::information(this,"CREATE EMPLOYEE","Successfully created an employee: "+name);
            }else
            {
                QMessageBox::warning(this,"CREATE EMPLOYEE","Failed to create the employee: "+CreateEmployeeQuery.lastError().text());
            }


        Employee employee;
        employee.setName(name);
        employee.setEmail(email);
        employee.setLogin(login);
        employee.setPassword(password);
        employee.setPhoneNumber(phone_number);
        employee.setRole(role);
        employee.setSalary(salary);

        EmployeeTable->setRowCount(0);
        EmployeeTable->clearContents();

        employees.push_back(employee);


    }

    EmployeeTable->blockSignals(true);
    int columnNum = EmployeeTable->columnCount();

    for(int j = 0; j < employees.size(); ++j)
    {
        EmployeeTable->insertRow(EmployeeTable->rowCount());
        for(int i = 0 ; i < columnNum; ++i)
        {
            QTableWidgetItem* item = new QTableWidgetItem("Text");

            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            EmployeeTable->setItem(j,i,item);
            auto Widget = EmployeeTable->item(j,i);
            if(Widget)
            {
                Widget->setBackground(QColor(0x545454));
            }
            else
            {
                qDebug() << "Reading nullptr;";
            }
        }
    }
    EmployeeTable->blockSignals(false);
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
            for(auto& employee : employees)
            {
                if(!employee.getIsModified())
                {
                    continue;
                }

                QSqlQuery EmployeeID;
                EmployeeID.prepare(R"(
                SELECT id FROM Employee
                WHERE email = :email
                )");
                EmployeeID.bindValue(":email",employee.getOldEmail());
                int employeeID = -1;
                if(EmployeeID.exec())
                {
                    if(EmployeeID.next())
                    {
                        employeeID = EmployeeID.value("id").toInt();
                    }
                }
                if(employeeID == -1)
                {
                    QMessageBox::warning(this,"Update Employee","Failed to find the employee: "+EmployeeID.lastError().text());
                    return;
                }
                QSqlQuery EmployeeUpdate;
                EmployeeUpdate.prepare(R"(
                UPDATE Employee SET
                name = :name,
                role = :role,
                email = :email,
                phone_number = :phone_number,
                salary = :salary,
                login = :login,
                password = :password
                WHERE
                    id = :id
                )");
                EmployeeUpdate.bindValue(":name", employee.getName());
                EmployeeUpdate.bindValue(":role", employee.getRole());
                EmployeeUpdate.bindValue(":email", employee.getEmail());
                EmployeeUpdate.bindValue(":phone_number", employee.getPhoneNumber());
                EmployeeUpdate.bindValue(":salary", employee.getSalary());
                EmployeeUpdate.bindValue(":login", employee.getLogin());
                EmployeeUpdate.bindValue(":password", employee.getPassword());
                EmployeeUpdate.bindValue(":id", employeeID);
                if (!EmployeeUpdate.exec())
                {
                    QMessageBox::warning(this, "Employee Update Failed",
                                         "Failed to update employee: " + EmployeeUpdate.lastError().text());
                }
                else
                {
                    // Optional: you can reset IsModified flag here
                    employee.setIsModified(false);
                    QMessageBox::information(this,"Save changes - EMPLOYEE","Saved changes.");
                }
            }
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

                int ID = -1;
                QSqlQuery IDQuery;
                IDQuery.prepare(R"(
                SELECT id from Employee
                WHERE email = :email
        )");
                IDQuery.bindValue(":email",employee->getEmail());
                if(IDQuery.exec())
                {
                    if(IDQuery.next())
                    {
                        ID = IDQuery.value("id").toInt();
                    }
                }
                if(ID == -1)
                {
                    QMessageBox::warning(this,"Remove Employee","Failed to find the employee");
                    return;
                }
                QSqlQuery DeleteEmployee;
                DeleteEmployee.prepare(R"(
                DELETE FROM Employee
                WHERE id = :id)");
                DeleteEmployee.bindValue(":id",ID);
                if (DeleteEmployee.exec()) {
                    qDebug() << "Employee with ID" << ID << "deleted successfully.";
                    employees.removeOne(*employee);
                    QMessageBox::information(this,"REMOVE SUCCESS",employee->getName() + " has been removed!");
                    delete employee;
                    EmployeeModified = true;
                    EmployeeTable->removeRow(RowMax-1);
                } else {
                    qDebug() << "Failed to delete employee:" << DeleteEmployee.lastError().text();
                }



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

        int ID = -1;
        QSqlQuery IDQuery;
        IDQuery.prepare(R"(
                SELECT id from Employee
                WHERE email = :email
        )");
        IDQuery.bindValue(":email",employee->getEmail());
        if(IDQuery.exec())
        {
            if(IDQuery.next())
            {
                ID = IDQuery.value("id").toInt();
            }
        }
        if(ID == -1)
        {
            QMessageBox::warning(this,"Remove Employee","Failed to find the employee");
            return;
        }
        QSqlQuery DeleteEmployee;
        DeleteEmployee.prepare(R"(
                DELETE FROM Employee
                WHERE id = :id)");
        DeleteEmployee.bindValue(":id",ID);
        if (DeleteEmployee.exec()) {
            qDebug() << "Employee with ID" << ID << "deleted successfully.";

            QMessageBox::information(this,"REMOVE SUCCESS",employee->getName() + " has been removed!");
             employees.removeOne(*employee);
            EmployeeModified = true;
            EmployeeTable->removeRow(Row);
        } else {
            qDebug() << "Failed to delete employee:" << DeleteEmployee.lastError().text();
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

    ResetAndGetEmployees();
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

    QString phone;
    regex = QRegularExpression("^\\+?[0-9 ]+$");
    if(!regex.match(ui->lineEdit_8->text()).hasMatch())// first name
    {
        ValidPhone = false;
       // QMessageBox::warning(this,"Search Employee","Invalid phone number");
    }

    if(ValidPhone)
    {
        phone = ui->lineEdit_8->text();
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

    QSqlQuery query;
    if (!employeeName.isEmpty() && !phone.isEmpty())
    {
        query.prepare(R"(
        SELECT * FROM Employee
        WHERE name ILIKE :name AND phone_number = :phone
    )");
        query.bindValue(":name", employeeName + "%");
        query.bindValue(":phone", phone);
    }
    else if (!employeeName.isEmpty())
    {
        query.prepare(R"(
        SELECT * FROM Employee
        WHERE name ILIKE :name
    )");
        query.bindValue(":name", employeeName + "%");
    }
    else if (!phone.isEmpty())
    {
        query.prepare(R"(
        SELECT * FROM Employee
        WHERE phone_number = :phone
    )");
        query.bindValue(":phone", phone);
    }
    else
    {
        qDebug() << phone;
        qDebug() << ui->lineEdit_8->text();
        QMessageBox::warning(this, "Search Employee", "Please enter at least name or phone number to search.");
        return;
    }

    bool foundEmployee = false;
    if (query.exec())
    {
        employees.clear();
        ui->tableWidget_3->clearContents();
        ui->tableWidget_3->setRowCount(0);

        while(query.next())
        {

            foundEmployee = true;
            Employee employee;
            // Employee found
            QString foundName = query.value("name").toString();
            QString foundRole = query.value("role").toString();
            QString foundEmail = query.value("email").toString();
            QString foundPhone = query.value("phone_number").toString();
            float foundSalary = query.value("salary").toFloat();
            QString login = query.value("login").toString();
            QString password = query.value("password").toString();
            employee.setName(foundName);
            employee.setRole(foundRole);
            employee.setEmail(foundEmail);
            employee.setPhoneNumber(foundPhone);
            employee.setSalary(foundSalary);
            employee.setLogin(login);
            employee.setPassword(password);

            employees.push_back(employee);
            ui->tableWidget_3->insertRow(ui->tableWidget_3->rowCount());

        }
    } else {
        QMessageBox::warning(this, "Search Employee", "Failed to search: " + query.lastError().text());
    }

    qDebug() << "=====================";

    qDebug() << "=====================";

    if(!foundEmployee)
    {
        QMessageBox::warning(this,"Find employee","Found no employee.");
        return;
    }
    ui->tableWidget_3->blockSignals(true);


    for(int j = 0; j < employees.size(); ++j)
    {

        for(int i = 0 ; i < ui->tableWidget_3->columnCount(); ++i)
        {
            QTableWidgetItem* item = new QTableWidgetItem(" ");

            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->tableWidget_3->setItem(j,i,item);
            auto Widget = ui->tableWidget_3->item(j,i);
            if(Widget)
            {
                Widget->setBackground(QColor(0x545454));
            }
            else
            {
                qDebug() << "Reading nullptr;";
            }
        }
    }

    ui->tableWidget_3->blockSignals(false);
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

    connect(ui->textEdit,&QTextEdit::textChanged,this,[this] {
        this->getBooking()->setNotes(ui->textEdit->toPlainText());
    });

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

    ResetAndGetEmployees();





    //Remove

}

void MainWindow::ResetAndGetEmployees()
{

    // Check if we have same amount of employees as in db

    QSqlQuery EmployeeCount;
    EmployeeCount.prepare(R"(
    SELECT COUNT(*) from Employee;)");
    if(EmployeeCount.exec())
    {
        if(EmployeeCount.next())
        {
            qDebug() << "Comparing " << EmployeeCount.value(0).toInt() << " to employees size " << employees.size();
            if(EmployeeCount.value(0).toInt() == employees.size())
            {
                QMessageBox::warning(this,"ResetAndGetEmployees","No need to reset the Employee Table");
                return;
            }
        }
    }
    employees.clear();
    ui->tableWidget_3->clearContents();
    ui->tableWidget_3->setRowCount(0);
    QSqlQuery EmployeesQuery;
    EmployeesQuery.prepare(R"(
    SELECT * from Employee;
)");
    if(EmployeesQuery.exec())
    {
        while(EmployeesQuery.next())
        {
            QString name = EmployeesQuery.value("name").toString();
            QString role = EmployeesQuery.value("role").toString();
            QString email = EmployeesQuery.value("email").toString();
            QString phone_number = EmployeesQuery.value("phone_number").toString();
            float salary = EmployeesQuery.value("salary").toFloat();
            QString login = EmployeesQuery.value("login").toString();
            QString password = EmployeesQuery.value("password").toString();

            Employee employee(name,role,email,phone_number,salary,login,password);

            AddEmployee(&employee);

        }
    }else
    {
        QMessageBox::warning(this,"GetAllEmployees - failed","Failed to get all employees:"+EmployeesQuery.lastError().text());
    }
    LoadEmployees();
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

void MainWindow::AddEmployee(Employee* employee, bool showMessage)
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
    if(showMessage)
    {
        QMessageBox::information(this,"Employee Created","Congratulations. Created an employee.");
    }
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
    connect(ui->pushButton_31,&QPushButton::clicked,this,&MainWindow::OnNewCustomerCreated);
    connect(ui->pushButton_32,&QPushButton::clicked,this,&MainWindow::OnSelectedCustomerRemoved);

    connect(ui->tableWidget_4,&QTableWidget::itemDoubleClicked,this,&MainWindow::OnTableItemEditable);

    connect(ui->tableWidget_4,&QTableWidget::itemChanged,this,&MainWindow::OnTableItemChanged);


}

void MainWindow::LoadCustomers()
{
    this->blockSignals(true);
    auto CustomerTable = ui->tableWidget_4;
    CustomerTable->blockSignals(true);

    if(uCustomers.empty())
    {
        CustomerTable->clearContents();
        CustomerTable->setRowCount(0);
    }
    else
    {
        for(int i = 0 ; i < uCustomers.size(); ++i)
        {
            auto customer = uCustomers[i];


            int RowCount = CustomerTable->rowCount();
            if(i > RowCount-1)
            {
               CustomerTable->insertRow(RowCount); // Insert at the end
            }
            CustomerTable->setItem(i,0,new QTableWidgetItem(QString::number(customer.getID())));
            CustomerTable->item(i,0)->setFlags(CustomerTable->item(i,0)->flags() & ~Qt::ItemIsEditable);

            CustomerTable->setItem(i,1,new QTableWidgetItem(customer.getBookingNumber()));
            CustomerTable->item(i,1)->setFlags(CustomerTable->item(i,1)->flags() & ~Qt::ItemIsEditable);

            CustomerTable->setItem(i,2,new QTableWidgetItem(customer.getName()));
            CustomerTable->item(i,2)->setFlags(CustomerTable->item(i,2)->flags() & ~Qt::ItemIsEditable);

            CustomerTable->setItem(i,3,new QTableWidgetItem(customer.getLastRoom()));
            CustomerTable->item(i,3)->setFlags(CustomerTable->item(i,3)->flags() & ~Qt::ItemIsEditable);

            CustomerTable->setItem(i,4,new QTableWidgetItem(customer.getPhonenumber()));
            CustomerTable->item(i,4)->setFlags(CustomerTable->item(i,4)->flags() & ~Qt::ItemIsEditable);

            CustomerTable->setItem(i,5,new QTableWidgetItem(customer.getEmail()));
            CustomerTable->item(i,5)->setFlags(CustomerTable->item(i,5)->flags() & ~Qt::ItemIsEditable);

            CustomerTable->setItem(i,6,new QTableWidgetItem(customer.getInformation()));
            CustomerTable->item(i,6)->setFlags(CustomerTable->item(i,6)->flags() & ~Qt::ItemIsEditable);

        }
    }

    this->blockSignals(false);
    CustomerTable->blockSignals(false);
}

UCustomer *MainWindow::GetCustomerByID(int ID)
{
    for(auto& customer : uCustomers)
    {
        if(customer.getID() == ID)
        {
            return &customer;
        }
    }
}

UCustomer *MainWindow::GetCustomerByName(QString Name)
{

    for(auto& customer : uCustomers)
    {
        if(customer.getName() == Name)
        {
            return &customer;
        }
    }
}

void MainWindow::AddCustomer(UCustomer *customer)
{
    if(!uCustomers.contains(*customer))
    {
        uCustomers.push_back(*customer);
    }
    return;

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

