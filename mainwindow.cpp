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
    int Floor = 0;
    if(RoomNumber >= 13)
    {
        Floor = 2;
    }else if(RoomNumber >= 3)
    {
        Floor = 1;
    }
    // Requesting data

    Booking* booking =  new Booking();
    booking->setBookerEmail("admin.admin@gmail.com");
    booking->setBookerName("Maksy Creator");
    booking->setBookerPhonenumber("+44 123 123 123");
    booking->setBookingNumber(RandomBookingNumber());
    const auto date = QDateTime::currentDateTime();
    booking->setCheckedinDate(date);
    booking->setCheckoutDate(date.addDays(30));
    booking->setCreatedDate(date.addDays(-2));
    booking->setRoomNumber(0);
    Customer customer;
    customer.setPhonenumber("+44 123 123 123");
    customer.setAge(40);
    customer.setDob(QDate(1996,01,15));
    customer.setName("Daniel Ricardo");
    booking->addCustomer(customer);
    // Changing data on Hotel Map page

    // pnr
    ui->label->setText(booking->getBookingNumber());
    // Created: %Date%
    ui->label_4->setText("Created: "+booking->getCreatedDate().toString());
            //Check-in date: %Date%
    ui->label_7->setText("Check-in date: "+booking->getCheckedinDate().date().toString());
                //Check-out date: %Date%
    ui->label_5->setText("Check-out date: "+booking->getCheckoutDate().date().toString());
                    //Room %Number% (Floor: %Floor%)
    if(date > booking->getCheckoutDate())
    {
        ui->label_5->setStyleSheet("color:red;");
    }else
    {
        ui->label_5->setStyleSheet("color:green;");
    }
    ui->label_10->setText("Room "+QString::fromStdString(std::to_string(RoomNumber))+
                          " (Floor "+QString::fromStdString(std::to_string(Floor))+" )");
        // %Name% (%Email%) (%Phone number%)
    ui->label_2->setText(booking->getBookerName()+ " ( "
                         + booking->getBookerEmail()+" ) "
                         + "( " + booking->getBookerPhonenumber() +" )");
}
/////////////////////////////////////////
/////////////////////////////////////////
/////////////////////////////////////////
void MainWindow::SetBookingPage()
{

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

