#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSpacerItem>
#include <QTabBar>
#include <QDebug>
#include <QVBoxLayout>
#include <QRandomGenerator>
#include <QMessageBox>
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


    for(int i = 0; i < 3; i++)
    {
        Payment payment;
        payment.PopulateWithRandomValues();
       // payment.Print();
        booking->addPayment(payment);
    }
    // Changing data on Hotel Map page

    this->setBooking(*booking);
    LoadBooking();
}

void MainWindow::OnTableItemEditable(QTableWidgetItem *item)
{
    qDebug() << "Double clicked";

    if(!item)
    {
        return;
    }
    QTableWidget* TableWidget = nullptr;
    if(ui->tableWidget == item->tableWidget() || (ui->tableWidget_2 == item->tableWidget()))
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
            item->setForeground(Qt::black);
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
        }
    }


}

void MainWindow::OnSavedChanges()
{
    qDebug() << "OnSavedChanges";
}
void MainWindow::OnCustomerBanned()
{

    qDebug() << "OnCustomerBanned";
}
void MainWindow::OnCustomerCreated(){

    qDebug() << "OnCustomerCreated";
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

    LoadBooking();
}
void MainWindow::OnCustomerRemoved(){

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
        qDebug() << "RowMax is "<<RowMax << " but NameWidget is "<< NameWidget;
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
            QMessageBox::information(this,"REMOVE SUCCESS",name + " has been removed!");

    }
}
void MainWindow::OnCustomerCheckedOut(){

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
            QMessageBox::information(this,"CHECK-OUT SUCCESS",customer->getName() + " has been checked out!");

    }
}
void MainWindow::OnCustomerCheckedIn(){

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
            QMessageBox::information(this,"CHECK-IN SUCCESS",customer->getName() + " has been checked in!");

    }
}

void MainWindow::OnNewBooking()
{
    getBooking()->Clear();
    getBooking()->Print();
    LoadBooking();
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


}

void MainWindow::OnPaymentRemoved()
{
    if(getBooking()->getPayments().empty())
    {
        QMessageBox::warning(this,"Remove payment","Cannot remove payments because none exists");
        return;
    }
    auto paymentTable = ui->tableWidget_2;
    int columnNum = paymentTable->columnCount();
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
        }
    }
    else
    {
        for(auto& item : selectedItems)
        {
            Rows.insert(item->row());
        }
        qDebug() << "Selected rows: " << Rows.size();
        for(int i = Rows.size() -1; i >= 0; --i)
        {
            qDebug() << "Trying to access Payments["<<i<<" ] while Payments contains only "<<getBooking()->getPayments().size() << " elements";
            Payment* payment = &getBooking()->getPayments()[i];
            if(payment)
            {
                getBooking()->getPayments().removeOne(*payment);
                paymentTable->removeRow(i);
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
    connect(ui->pushButton_2,&QPushButton::clicked,this,&MainWindow::OnCustomerCheckedIn);
    // check out
    connect(ui->pushButton_3,&QPushButton::clicked,this,&MainWindow::OnCustomerCheckedOut);
    // remove
    connect(ui->pushButton_4,&QPushButton::clicked,this,&MainWindow::OnCustomerRemoved);
    // create
    connect(ui->pushButton_5,&QPushButton::clicked,this,&MainWindow::OnCustomerCreated);
    // ban
    connect(ui->pushButton_6,&QPushButton::clicked,this,&MainWindow::OnCustomerBanned);
    // new booking, reset
    connect(ui->pushButton_7,&QPushButton::clicked,this,&MainWindow::OnNewBooking);

    // payment, create remove
    connect(ui->pushButton_8,&QPushButton::clicked,this,&MainWindow::OnPaymentAdded);
    connect(ui->pushButton_9,&QPushButton::clicked,this,&MainWindow::OnPaymentRemoved);

}

void MainWindow::LoadBooking()
{
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


}
void MainWindow::SetCustomersPage(){

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

