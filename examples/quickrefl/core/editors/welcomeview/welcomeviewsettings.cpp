// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#include "welcomeviewsettings.h"
#include <QDir>
#include <QSettings>
#include <mvvm/utils/fileutils.h>

namespace
{
const QString group_key = "welcomeview";
const QString current_workdir_key = "currentworkdir";
const QString workdir_setting_name()
{
    return group_key + "/" + current_workdir_key;
}
} // namespace

WelcomeViewSettings::WelcomeViewSettings()
{
    readSettings();
}

WelcomeViewSettings::~WelcomeViewSettings()
{
    writeSettings();
}

//! Returns current workdir.
QString WelcomeViewSettings::currentWorkdir() const
{
    return m_current_workdir;
}

//! Updates current workdir value from user selection.
//! Workdir will be set as parent director of selected `dirname`.
void WelcomeViewSettings::updateWorkdirFromSelection(const QString& dirname)
{
    if (!dirname.isEmpty()) {
        auto parent_path = ModelView::Utils::parent_path(dirname.toStdString());
        m_current_workdir = QString::fromStdString(parent_path);
    }
}

//! Write all settings to file.
void WelcomeViewSettings::writeSettings()
{
    QSettings settings;
    settings.setValue(workdir_setting_name(), m_current_workdir);
}

//! Reads all settings from file.
void WelcomeViewSettings::readSettings()
{
    QSettings settings;
    m_current_workdir = QDir::homePath();

    if (settings.contains(workdir_setting_name()))
        m_current_workdir = settings.value(workdir_setting_name()).toString();
}