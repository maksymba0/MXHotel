#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H
#include <stdint.h>
#include <QString>
#include <QWidget>
enum class Themes : int
{
    INVALID = -1,
    DEFAULT,
    DARK,
    CUSTOM

};

class ThemeManager
{
public:
    ThemeManager();
    ThemeManager(QWidget* _parent);

    QWidget* parent;

    void SetTheme(Themes theme);
    void SelectNextTheme();
    Themes GetTheme() { return current; }
    void SelectTheme(int Index);

    bool IsStandard() { return current == Themes::DEFAULT;}
    bool IsDark() { return current == Themes::DARK;}
    bool IsWhite() { return IsStandard();}
    bool IsCustom() { return current == Themes::CUSTOM;}


    QString GetThemeName();
private:
    void OnThemeUpdated(QWidget* parent);
protected:
Themes current;
};

#endif // THEMEMANAGER_H
