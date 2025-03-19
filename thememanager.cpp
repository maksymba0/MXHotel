#include "thememanager.h"
#include <QString>
#include <QDebug>
#include <QWidget>
#include <QList>
#include <QLabel>
#include <QTabWidget>
#include <QTableWidget>

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
    switch(Theme)
    {
    default:
    case Themes::INVALID:
    case Themes::DEFAULT:
        // TO WHITE

        parent->setStyleSheet("background:white;");
        for(const auto& Object : labels)
        {
            Object->setStyleSheet("color:black;");
        }
        for(const auto& Object : tableWidgets)
        {
            Object->setStyleSheet("color:black;");
        }
        //tabWidget->setStyleSheet()
        break;
    case Themes::DARK:
        // TO BLACK
        parent->setStyleSheet("color:#525252;background:#525252;");
        for(const auto& Object : labels)
        {
            Object->setStyleSheet("color:white;");
        }

        for(const auto& Object : tableWidgets)
        {
            Object->setStyleSheet("color:white;");
        }

        break;
    case Themes::CUSTOM:
        // TO CUSTOM
        break;
    }

}
