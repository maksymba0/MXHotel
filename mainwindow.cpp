#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSpacerItem>
#include <QTabBar>
#include <QDebug>
#include <QVBoxLayout>
#include <QRandomGenerator>
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
    int RoomNumber = RoomName.section('_',1).toInt();

    // Requesting data

    Booking* booking =  new Booking();
    booking->setBookerEmail("admin.admin@gmail.com");
    booking->setBookerName("Maksy Creator");
    booking->setBookerPhonenumber("+44 123 123 123");
    booking->setBookingNumber(RandomBookingNumber());
    const auto date = QDateTime::currentDateTime();
    booking->setCheckedinDate(date.addDays(-2));
    booking->setCheckoutDate(date.addDays(-1));
    booking->setCreatedDate(date.addDays(-3));
    booking->setRoomNumber(0);
    Customer customer;
    customer.setPhonenumber("+44 123 123 123");
    customer.setAge(40);
    customer.setDob(QDate(1996,01,15));
    customer.setName("Daniel Ricardo");
    customer.setDocumentNumber("KHTAIR9291");
    customer.setDocumentType("ID Card");
    booking->addCustomer(customer);
    customer.setPhonenumber("+380 95 128 190");
    customer.setAge(21);
    customer.setDob(QDate(1998,11,26));
    customer.setName("Vlad Kroker");
    customer.setDocumentNumber("MI299DAS0");
    customer.setDocumentType("Passport");
    booking->addCustomer(customer);
    customer.setPhonenumber("+380 68 330 110");
    customer.setAge(19);
    customer.setDob(QDate(2003,05,14));
    customer.setName("Zombuz Stepankov");
    customer.setDocumentNumber("PIV920OO");
    customer.setDocumentType("Refugee passport");
    booking->addCustomer(customer);


    for(int i = 0; i < 3; i++)
    {
        Payment payment;
        payment.PopulateWithRandomValues();
        payment.Print();
        booking->addPayment(payment);
    }
    // Changing data on Hotel Map page

    LoadBooking(booking,RoomNumber);
}
/////////////////////////////////////////
/////////////////////////////////////////
/////////////////////////////////////////
void MainWindow::SetBookingPage()
{

}

void MainWindow::LoadBooking(Booking *booking, int RoomNumber)
{
    if(!booking)
    {
        booking = new Booking();
    }
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
    if(date > booking->getCheckoutDate())
    {
        ui->label_5->setStyleSheet("color:red;");
    }else
    {
        ui->label_5->setStyleSheet("color:green;");
    }
    ui->label_10->setText("Room "+QString::fromStdString(std::to_string(RoomNumber))+
                          " ( Floor "+QString::fromStdString(std::to_string(Floor))+" )");
    // %Name% (%Email%) (%Phone number%)
    ui->label_2->setText(booking->getBookerName()+ " ( "
                         + booking->getBookerEmail()+" ) "
                         + "( " + booking->getBookerPhonenumber() +" )");

    for(int i = 0 ; i < booking->getCustomers().size(); ++i)
    {
        Customer customer = booking->getCustomers()[i];
        int RowCount = ui->tableWidget->rowCount();
        if(i > RowCount-1)
        {
            ui->tableWidget->insertRow(RowCount); // Insert at the end
        }
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(customer.getName()));

        ui->tableWidget->setItem(i,1,new QTableWidgetItem(customer.getDob().toString("dd.MM.yyyy")));

        ui->tableWidget->setItem(i,2,new QTableWidgetItem(customer.getPhonenumber()));

        ui->tableWidget->setItem(i,3,new QTableWidgetItem(customer.getDocumentNumber()));

        ui->tableWidget->setItem(i,4,new QTableWidgetItem(customer.getDocumentType()));
    }

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
void MainWindow::SetHotelMapPage(){

    QList<QPushButton*> buttons = this->findChildren<QPushButton*>();
    for(const auto& obj: buttons)
    {
        if(obj->objectName().contains("Room"))
        {
            qDebug() << "Found room: "<<obj->objectName();
            connect(obj,&QPushButton::clicked,this,[this,obj](){
                OnRoomInfoRequested(obj->objectName());
            });
        }
    }

}
void MainWindow::SetEmployeesPage(){

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

