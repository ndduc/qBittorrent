/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2019  Vladimir Golovnev <glassez@yandex.ru>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * In addition, as a special exception, the copyright holders give permission to
 * link this program with the OpenSSL project's "OpenSSL" library (or with
 * modified versions of it that use the same license as the "OpenSSL" library),
 * and distribute the linked executables. You must obey the GNU General Public
 * License in all respects for all of the code used other than "OpenSSL".  If you
 * modify file(s), you may extend this exception to your version of the file(s),
 * but you are not obligated to do so. If you do not wish to do so, delete this
 * exception statement from your version.
 */

#pragma once

#include <QHash>
#include <QObject>
#include <QVariant>

#include "exceptions.h"

class SettingError : public RuntimeError
{
public:
    explicit SettingError(const QString &settingName, const QString &message)
        : RuntimeError {(QString {"'%1': %2"}).arg(settingName, message)}
        , m_settingName {settingName}
    {
    }

    QString settingName() const
    {
        return m_settingName;
    }

private:
    const QString m_settingName;
};

class SettingNotFoundError : public SettingError
{
public:
    explicit SettingNotFoundError(const QString &settingName)
        : SettingError {settingName, "setting doesn't exist"}
    {
    }
};

class SettingHandler
{
public:
    explicit SettingHandler (const QVariant &defaultValue = {});

    QVariant getValue() const;
    void setValue(const QVariant &value);

private:
    QVariant m_value;
};

#define DECLARE_SETTING_ID(ID, Name) constexpr static QLatin1String ID {Name};

class Settings : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Settings)

public:
    DECLARE_SETTING_ID(APP_LOCALE, "app.locale")
    DECLARE_SETTING_ID(GUI_CUSTOMTHEME_ENABLED, "gui.custom_theme.enabled")
    DECLARE_SETTING_ID(GUI_CUSTOMTHEME_PATH, "gui.custom_theme.path")
    DECLARE_SETTING_ID(GUI_USESYSTEMICONTHEME, "gui.use_system_icon_theme")

    DECLARE_SETTING_ID(ExitConfirm, "Preferences/General/ExitConfirm")
    DECLARE_SETTING_ID(PreventFromSuspendWhenDownloading, "Preferences/General/PreventFromSuspendWhenDownloading")
    DECLARE_SETTING_ID(PreventFromSuspendWhenSeeding, "Preferences/General/PreventFromSuspendWhenSeeding")
    DECLARE_SETTING_ID(ScanDirs, "Preferences/Downloads/ScanDirsV2")
    DECLARE_SETTING_ID(MailNotification_enabled, "Preferences/MailNotification/enabled")
    DECLARE_SETTING_ID(MailNotification_sender, "Preferences/MailNotification/sender")
    DECLARE_SETTING_ID(MailNotification_email, "Preferences/MailNotification/email")
    DECLARE_SETTING_ID(MailNotification_smtp_server, "Preferences/MailNotification/smtp_server")
    DECLARE_SETTING_ID(MailNotification_req_ssl, "Preferences/MailNotification/req_ssl")
    DECLARE_SETTING_ID(MailNotification_req_auth, "Preferences/MailNotification/req_auth")
    DECLARE_SETTING_ID(MailNotification_username, "Preferences/MailNotification/username")
    DECLARE_SETTING_ID(MailNotification_password, "Preferences/MailNotification/password")
    DECLARE_SETTING_ID(Scheduler_Starttime, "Preferences/Scheduler/start_time")
    DECLARE_SETTING_ID(Scheduler_Endtime, "Preferences/Scheduler/end_time")
    DECLARE_SETTING_ID(Search_Enabled, "Preferences/Search/SearchEnabled")
    DECLARE_SETTING_ID(WebUI_Enabled, "Preferences/WebUI/Enabled")
    DECLARE_SETTING_ID(WebUI_LocalHostAuth, "Preferences/WebUI/LocalHostAuth")
    DECLARE_SETTING_ID(WebUI_AuthSubnetWhitelistEnabled, "Preferences/WebUI/AuthSubnetWhitelistEnabled")
    DECLARE_SETTING_ID(WebUI_ServerDomains, "Preferences/WebUI/ServerDomains")
    DECLARE_SETTING_ID(WebUI_Address, "Preferences/WebUI/Address")
    DECLARE_SETTING_ID(WebUI_Port, "Preferences/WebUI/Port")
    DECLARE_SETTING_ID(WebUI_UseUPnP, "Preferences/WebUI/UseUPnP")
    DECLARE_SETTING_ID(WebUI_UseUPnP, "Preferences/WebUI/UseUPnP")
    DECLARE_SETTING_ID(WebUI_Username, "Preferences/WebUI/Username")
    DECLARE_SETTING_ID(WebUI_Password_PBKDF2, "Preferences/WebUI/Password_PBKDF2")
    DECLARE_SETTING_ID(WebUI_SessionTimeout, "Preferences/WebUI/SessionTimeout")
    DECLARE_SETTING_ID(WebUI_ClickjackingProtection, "Preferences/WebUI/ClickjackingProtection")
    DECLARE_SETTING_ID(WebUI_CSRFProtection, "Preferences/WebUI/CSRFProtection")
    DECLARE_SETTING_ID(WebUI_HostHeaderValidation, "Preferences/WebUI/HostHeaderValidation")
    DECLARE_SETTING_ID(WebUI_HTTPS_Enabled, "Preferences/WebUI/HTTPS/Enabled")
    DECLARE_SETTING_ID(WebUI_HTTPS_CertificatePath, "Preferences/WebUI/HTTPS/CertificatePath")
    DECLARE_SETTING_ID(WebUI_HTTPS_KeyPath, "Preferences/WebUI/HTTPS/KeyPath")
    DECLARE_SETTING_ID(WebUI_AlternativeUIEnabled, "Preferences/WebUI/AlternativeUIEnabled")
    DECLARE_SETTING_ID(WebUI_RootFolder, "Preferences/WebUI/RootFolder")
    DECLARE_SETTING_ID(DynDNS_Enabled, "Preferences/DynDNS/Enabled")
    DECLARE_SETTING_ID(DynDNS_DomainName, "Preferences/DynDNS/DomainName")
    DECLARE_SETTING_ID(DynDNS_Username, "Preferences/DynDNS/Username")
    DECLARE_SETTING_ID(DynDNS_Password, "Preferences/DynDNS/Password")
    DECLARE_SETTING_ID(AutoRun_Enabled, "AutoRun/enabled")
    DECLARE_SETTING_ID(AutoRun_Program, "AutoRun/program")
    DECLARE_SETTING_ID(AutoShutDownOnCompletion, "Preferences/Downloads/AutoShutDownOnCompletion")
    DECLARE_SETTING_ID(AutoSuspendOnCompletion, "Preferences/Downloads/AutoSuspendOnCompletion")
    DECLARE_SETTING_ID(AutoHibernateOnCompletion, "Preferences/Downloads/AutoHibernateOnCompletion")
    DECLARE_SETTING_ID(AutoShutDownqBTOnCompletion, "Preferences/Downloads/AutoShutDownqBTOnCompletion")
    DECLARE_SETTING_ID(DontConfirmAutoExit, "ShutdownConfirmDlg/DontConfirmAutoExit")
    DECLARE_SETTING_ID(RecheckOnCompletion, "Preferences/Advanced/RecheckOnCompletion")
    DECLARE_SETTING_ID(ResolvePeerCountries, "Preferences/Connection/ResolvePeerCountries")
    DECLARE_SETTING_ID(ResolvePeerHostNames, "Preferences/Connection/ResolvePeerHostNames")
    DECLARE_SETTING_ID(DisableRecursiveDownload, "Preferences/Advanced/DisableRecursiveDownload")
    DECLARE_SETTING_ID(NeverCheckFileAssocation, "Preferences/Win32/NeverCheckFileAssocation")
    DECLARE_SETTING_ID(trackerPort, "Preferences/Advanced/trackerPort")
    DECLARE_SETTING_ID(updateCheck, "Preferences/Advanced/updateCheck")
    DECLARE_SETTING_ID(confirmTorrentDeletion, "Preferences/Advanced/confirmTorrentDeletion")
    DECLARE_SETTING_ID(confirmTorrentRecheck, "Preferences/Advanced/confirmTorrentRecheck")
    DECLARE_SETTING_ID(confirmRemoveAllTags, "Preferences/Advanced/confirmRemoveAllTags")
    DECLARE_SETTING_ID(Search_disabledEngines, "SearchEngines/disabledEngines")
    DECLARE_SETTING_ID(DeleteTorrentsFilesAsDefault, "Preferences/General/DeleteTorrentsFilesAsDefault")
    DECLARE_SETTING_ID(ExitConfirm, "Preferences/General/ExitConfirm")
    DECLARE_SETTING_ID(PreventFromSuspendWhenDownloading, "Preferences/General/PreventFromSuspendWhenDownloading")
    DECLARE_SETTING_ID(PreventFromSuspendWhenSeeding, "Preferences/General/PreventFromSuspendWhenSeeding")
    DECLARE_SETTING_ID(RecheckOnCompletion, "Preferences/Advanced/RecheckOnCompletion")

    QVariant get(const QString &name) const;
    void set(const QString &name, const QVariant &value);
    void begin();
    void end();

    static void initInstance();
    static void freeInstance();
    static Settings *instance();

signals:
    void changed();

private:
    Settings();

    SettingHandler &getSettingHandler(const QString &name);
    const SettingHandler &getSettingHandler(const QString &name) const;

    static Settings *m_instance;
    bool m_isTransacted = false;
    QHash<QString, SettingHandler> m_settingHandlers;
};
