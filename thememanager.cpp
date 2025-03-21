#include "thememanager.h"
#include <QString>
#include <QDebug>
#include <QWidget>
#include <QList>
#include <QLabel>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>

ThemeManager::ThemeManager(QWidget* _parent)
{
    parent = _parent;
    current = Themes::DEFAULT;
}
ThemeManager::ThemeManager()
{
    current = Themes::DEFAULT;
}

void ThemeManager::SelectNextTheme()
{
    if(current == Themes::DEFAULT)
    {
        current = Themes::DARK;
    }else if(current == Themes::DARK)
    {
        current =  Themes::DEFAULT;
    }
    OnThemeUpdated(parent);
}

void ThemeManager::SelectTheme(int Index)
{
    Themes theme = Themes::INVALID;
    // Verify that the index is valid (valid theme index in Themes enum)
    // Remember that argument passed here cannot be 0, or it is Themes::INVALID
    if(Index > 3 || Index <= 0)
    {
        theme = Themes::DEFAULT;
    }
    theme = static_cast<Themes>(Index);

    current = theme;
    OnThemeUpdated(parent);
    return;

}

QString ThemeManager::GetThemeName()
{
    if(this->IsStandard())
    {
        return "Standard";
    }
    if(this->IsDark())
    {
        return "Dark";
    }
    if(this->IsCustom())
    {
        return "Custom";
    }
    return "Standard";
}

void ThemeManager::OnThemeUpdated(QWidget* parent)
{
// Here is the logic for changing every element color
    if(!parent)
    {
    qDebug() << "Parent is nullptr";
    return;
    }
    auto Theme = GetTheme();
    QList<QLabel*> labels = parent->findChildren<QLabel*>();
    QTabWidget* tabWidget = parent->findChild<QTabWidget*>();
    QList<QTableWidget*> tableWidgets = parent->findChildren<QTableWidget*>();
    QList<QTextEdit*> textEdits = parent->findChildren<QTextEdit*>();
    QList<QGroupBox*> groupBoxes = parent->findChildren<QGroupBox*>();
    QList<QComboBox*> comboBoxes = parent->findChildren<QComboBox*>();
    QList<QPushButton*> buttons = parent->findChildren<QPushButton*>();

    switch(Theme)
    {
    default:
    case Themes::INVALID:
    case Themes::DEFAULT:
        // TO WHITE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        parent->setStyleSheet("color:black;background:white;");
        for(const auto& Object : labels)
        {
            Object->setStyleSheet("color:black;");
        }
        for(const auto& Object : tableWidgets)
        {
            Object->setStyleSheet("border: 1px ridge black;background-color:white;");
        }
        for(const auto& Object : textEdits)
        {
            Object->setStyleSheet("border: 1px ridge black;color:black;");
        }
        for(const auto& Object : groupBoxes)
        {
            Object->setStyleSheet(
                                  "QGroupBox {"
                                  "   border: 1px ridge black;"   // Border style
                                  "   color: black;"              // Text color inside the QGroupBox
                                  "}"
                                  "QGroupBox::title {"
                                  "   color: red;"                // Title text color
                                  "   padding: 0 5px;"            // Space around the title
                                  "   background-color: white;"  // Background color of the title
                                  "}");
        }
        for(const auto& Object : comboBoxes)
        {
            Object->setStyleSheet("QComboBox {"
                                  "   border: 1px ridge black;"   // Border style
                                  "   color: black;"              // Text color inside the QGroupBox
                                  "}"
                                  );
        }
        for (const auto& Object : buttons) {
            Object->setStyleSheet(
                "QPushButton {"
                "    border: 2px solid black;"
                "    background-color: #F9F9F9;"  // Softer white for less strain
                "    color: black;"
                "    padding: 4px;"
                "    border-radius: 6px;"
                "    font-size: 14px;"
                "    font-weight: bold;"
                "}"
                "QPushButton:hover {"
                "    border: 2px solid red;"
                "    background-color: #EAEAEA;"  // Light gray hover effect
                "}"
                "QPushButton:pressed {"
                "    border: 2px solid blue;"
                "    background-color: lightblue;"
                "}"
                "QPushButton:disabled {"
                "    border: 2px solid gray;"
                "    background-color: #DDDDDD;"
                "    color: #888888;"
                "}");
        }
        qDebug() << "Changed " << groupBoxes.size() << " group boxes";
        break;
    case Themes::DARK:
        // TO BLACK
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        parent->setStyleSheet("color:white;background:black;background-color:black;");

        for(const auto& Object : labels)
        {
            Object->setStyleSheet("color:white;");
        }

        for(const auto& Object : tableWidgets)
        {
             Object->setStyleSheet("border: 1px ridge white;color:white;background-color:black;");
        }
        for(const auto& Object : textEdits)
        {
            Object->setStyleSheet("border: 1px ridge white;color:white;");
        }
        for(const auto& Object : groupBoxes)
        {
            Object->setStyleSheet(
                "QGroupBox {"
                "   border: 1px ridge white;"   // Border style
                "   color: black;"              // Text color inside the QGroupBox
                "}"
                "QGroupBox::title {"
                "   color: red;"                // Title text color
                "   padding: 0 5px;"            // Space around the title
                "   background-color: black;"  // Background color of the title
                "}");
        }
        for(const auto& Object : comboBoxes)
        {
            Object->setStyleSheet("QComboBox {"
                                  "   border: 1px ridge white;"   // Border style
                                  "   color: white;"              // Text color inside the QGroupBox
                                  "}");
        }
        for (const auto& Object : buttons) {
            Object->setStyleSheet(
                "QPushButton {"
                "    border: 2px solid white;"
                "    background-color: #222222;"  // Slightly lighter black for depth
                "    color: white;"
                "    padding: 4px;"
                "    border-radius: 6px;"
                "    font-size: 14px;"
                "    font-weight: bold;"
                "}"
                "QPushButton:hover {"
                "    border: 2px solid red;"
                "    background-color: #333333;"  // Slightly lighter for hover effect
                "}"
                "QPushButton:pressed {"
                "    border: 2px solid blue;"
                "    background-color: #318A5E;"
                "}"
                "QPushButton:disabled {"
                "    border: 2px solid gray;"
                "    background-color: #111111;"
                "    color: #666666;"
                "}");
        }
        break; 
    case Themes::CUSTOM:
        // TO CUSTOM
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        break;
    }

}
