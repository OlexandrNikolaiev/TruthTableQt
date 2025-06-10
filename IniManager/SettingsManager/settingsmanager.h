#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include "../inimanager.h"

class SettingsManager : public IniManager {
public:
    SettingsManager(const QString& fileName = "settings.ini");

    void saveWindowGeometry(const QByteArray& geometry);
    QByteArray loadWindowGeometry();

    void saveShowAuxButtons(bool show);
    bool loadShowAuxButtons();

    void saveCellHoverColor(const QColor& color);
    QString loadCellHoverColor();

    void saveLanguage(QString language);
    QString loadLanguage();
};

#endif // SETTINGSMANAGER_H
