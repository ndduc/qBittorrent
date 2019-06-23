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

#include "settings.h"

#include <QLocale>
#include "settingsstorage.h"

namespace
{
    QString mapKey(const QString &key)
    {
        static const QHash<QString, QString> keyMapping = {
            {Settings::APP_LOCALE, "Preferences/General/Locale"},
            {Settings::GUI_CUSTOMTHEME_ENABLED, "Preferences/General/UseCustomUITheme"},
            {Settings::GUI_CUSTOMTHEME_PATH, "Preferences/General/CustomUIThemePath"},
            {Settings::GUI_USESYSTEMICONTHEME, "Preferences/Advanced/useSystemIconTheme"},

            {"BitTorrent/Session/MaxRatioAction", "Preferences/Bittorrent/MaxRatioAction"},
            {"BitTorrent/Session/DefaultSavePath", "Preferences/Downloads/SavePath"},
            {"BitTorrent/Session/TempPath", "Preferences/Downloads/TempPath"},
            {"BitTorrent/Session/TempPathEnabled", "Preferences/Downloads/TempPathEnabled"},
            {"BitTorrent/Session/AddTorrentPaused", "Preferences/Downloads/StartInPause"},
            {"BitTorrent/Session/RefreshInterval", "Preferences/General/RefreshInterval"},
            {"BitTorrent/Session/Preallocation", "Preferences/Downloads/PreAllocation"},
            {"BitTorrent/Session/AddExtensionToIncompleteFiles", "Preferences/Downloads/UseIncompleteExtension"},
            {"BitTorrent/Session/TorrentExportDirectory", "Preferences/Downloads/TorrentExportDir"},
            {"BitTorrent/Session/FinishedTorrentExportDirectory", "Preferences/Downloads/FinishedTorrentExportDir"},
            {"BitTorrent/Session/GlobalUPSpeedLimit", "Preferences/Connection/GlobalUPLimit"},
            {"BitTorrent/Session/GlobalDLSpeedLimit", "Preferences/Connection/GlobalDLLimit"},
            {"BitTorrent/Session/AlternativeGlobalUPSpeedLimit", "Preferences/Connection/GlobalUPLimitAlt"},
            {"BitTorrent/Session/AlternativeGlobalDLSpeedLimit", "Preferences/Connection/GlobalDLLimitAlt"},
            {"BitTorrent/Session/UseAlternativeGlobalSpeedLimit", "Preferences/Connection/alt_speeds_on"},
            {"BitTorrent/Session/BandwidthSchedulerEnabled", "Preferences/Scheduler/Enabled"},
            {"BitTorrent/Session/Port", "Preferences/Connection/PortRangeMin"},
            {"BitTorrent/Session/UseRandomPort", "Preferences/General/UseRandomPort"},
            {"BitTorrent/Session/IPv6Enabled", "Preferences/Connection/InterfaceListenIPv6"},
            {"BitTorrent/Session/Interface", "Preferences/Connection/Interface"},
            {"BitTorrent/Session/InterfaceName", "Preferences/Connection/InterfaceName"},
            {"BitTorrent/Session/InterfaceAddress", "Preferences/Connection/InterfaceAddress"},
            {"BitTorrent/Session/SaveResumeDataInterval", "Preferences/Downloads/SaveResumeDataInterval"},
            {"BitTorrent/Session/Encryption", "Preferences/Bittorrent/Encryption"},
            {"BitTorrent/Session/ForceProxy", "Preferences/Connection/ProxyForce"},
            {"BitTorrent/Session/ProxyPeerConnections", "Preferences/Connection/ProxyPeerConnections"},
            {"BitTorrent/Session/MaxConnections", "Preferences/Bittorrent/MaxConnecs"},
            {"BitTorrent/Session/MaxUploads", "Preferences/Bittorrent/MaxUploads"},
            {"BitTorrent/Session/MaxConnectionsPerTorrent", "Preferences/Bittorrent/MaxConnecsPerTorrent"},
            {"BitTorrent/Session/MaxUploadsPerTorrent", "Preferences/Bittorrent/MaxUploadsPerTorrent"},
            {"BitTorrent/Session/DHTEnabled", "Preferences/Bittorrent/DHT"},
            {"BitTorrent/Session/LSDEnabled", "Preferences/Bittorrent/LSD"},
            {"BitTorrent/Session/PeXEnabled", "Preferences/Bittorrent/PeX"},
            {"BitTorrent/Session/AddTrackersEnabled", "Preferences/Bittorrent/AddTrackers"},
            {"BitTorrent/Session/AdditionalTrackers", "Preferences/Bittorrent/TrackersList"},
            {"BitTorrent/Session/IPFilteringEnabled", "Preferences/IPFilter/Enabled"},
            {"BitTorrent/Session/TrackerFilteringEnabled", "Preferences/IPFilter/FilterTracker"},
            {"BitTorrent/Session/IPFilter", "Preferences/IPFilter/File"},
            {"BitTorrent/Session/GlobalMaxRatio", "Preferences/Bittorrent/MaxRatio"},
            {"BitTorrent/Session/AnnounceToAllTrackers", "Preferences/Advanced/AnnounceToAllTrackers"},
            {"BitTorrent/Session/DiskCacheSize", "Preferences/Downloads/DiskWriteCacheSize"},
            {"BitTorrent/Session/DiskCacheTTL", "Preferences/Downloads/DiskWriteCacheTTL"},
            {"BitTorrent/Session/UseOSCache", "Preferences/Advanced/osCache"},
            {"BitTorrent/Session/AnonymousModeEnabled", "Preferences/Advanced/AnonymousMode"},
            {"BitTorrent/Session/QueueingSystemEnabled", "Preferences/Queueing/QueueingEnabled"},
            {"BitTorrent/Session/MaxActiveDownloads", "Preferences/Queueing/MaxActiveDownloads"},
            {"BitTorrent/Session/MaxActiveUploads", "Preferences/Queueing/MaxActiveUploads"},
            {"BitTorrent/Session/MaxActiveTorrents", "Preferences/Queueing/MaxActiveTorrents"},
            {"BitTorrent/Session/IgnoreSlowTorrentsForQueueing", "Preferences/Queueing/IgnoreSlowTorrents"},
            {"BitTorrent/Session/OutgoingPortsMin", "Preferences/Advanced/OutgoingPortsMin"},
            {"BitTorrent/Session/OutgoingPortsMax", "Preferences/Advanced/OutgoingPortsMax"},
            {"BitTorrent/Session/IgnoreLimitsOnLAN", "Preferences/Advanced/IgnoreLimitsLAN"},
            {"BitTorrent/Session/IncludeOverheadInLimits", "Preferences/Advanced/IncludeOverhead"},
            {"BitTorrent/Session/AnnounceIP", "Preferences/Connection/InetAddress"},
            {"BitTorrent/Session/SuperSeedingEnabled", "Preferences/Advanced/SuperSeeding"},
            {"BitTorrent/Session/MaxHalfOpenConnections", "Preferences/Connection/MaxHalfOpenConnec"},
            {"BitTorrent/Session/uTPEnabled", "Preferences/Bittorrent/uTP"},
            {"BitTorrent/Session/uTPRateLimited", "Preferences/Bittorrent/uTP_rate_limited"},
            {"BitTorrent/TrackerEnabled", "Preferences/Advanced/trackerEnabled"},
            {"Network/Proxy/OnlyForTorrents", "Preferences/Connection/ProxyOnlyForTorrents"},
            {"Network/Proxy/Type", "Preferences/Connection/ProxyType"},
            {"Network/Proxy/Authentication", "Preferences/Connection/Proxy/Authentication"},
            {"Network/Proxy/Username", "Preferences/Connection/Proxy/Username"},
            {"Network/Proxy/Password", "Preferences/Connection/Proxy/Password"},
            {"Network/Proxy/IP", "Preferences/Connection/Proxy/IP"},
            {"Network/Proxy/Port", "Preferences/Connection/Proxy/Port"},
            {"Network/PortForwardingEnabled", "Preferences/Connection/UPnP"},
            {"AddNewTorrentDialog/TreeHeaderState", "AddNewTorrentDialog/qt5/treeHeaderState"},
            {"AddNewTorrentDialog/Width", "AddNewTorrentDialog/width"},
            {"AddNewTorrentDialog/Position", "AddNewTorrentDialog/y"},
            {"AddNewTorrentDialog/Expanded", "AddNewTorrentDialog/expanded"},
            {"AddNewTorrentDialog/SavePathHistory", "TorrentAdditionDlg/save_path_history"},
            {"AddNewTorrentDialog/Enabled", "Preferences/Downloads/NewAdditionDialog"},
            {"AddNewTorrentDialog/TopLevel", "Preferences/Downloads/NewAdditionDialogFront"},

            {"State/BannedIPs", "Preferences/IPFilter/BannedIPs"}
        };

        return keyMapping.value(key, key);
    }
}

Settings *Settings::m_instance = nullptr;

#define DEFINE_SETTING_ID(ID) constexpr QLatin1String Settings::ID;
DEFINE_SETTING_ID(APP_LOCALE)
DEFINE_SETTING_ID(GUI_CUSTOMTHEME_ENABLED)
DEFINE_SETTING_ID(GUI_CUSTOMTHEME_PATH)
DEFINE_SETTING_ID(GUI_USESYSTEMICONTHEME)

void Settings::initInstance()
{
    if (!m_instance)
        m_instance = new Settings;
}

void Settings::freeInstance()
{
    if (m_instance) {
        delete m_instance;
        m_instance = nullptr;
    }
}

Settings *Settings::instance()
{
    return m_instance;
}

Settings::Settings()
    : m_settingHandlers {
          {Settings::APP_LOCALE, SettingHandler {QLocale::system().name()}},
          {Settings::GUI_CUSTOMTHEME_ENABLED, SettingHandler {false}},
          {Settings::GUI_CUSTOMTHEME_PATH, SettingHandler {}},
          {Settings::GUI_USESYSTEMICONTHEME, SettingHandler {false}},
      }
{
    for (auto it = m_settingHandlers.begin(); it != m_settingHandlers.end(); ++it) {
        (*it).setValue(SettingsStorage::instance()->loadValue(mapKey(it.key()), (*it).getValue()));
    }
}

QVariant Settings::get(const QString &name) const
{
    return getSettingHandler(name).getValue();
}

void Settings::set(const QString &name, const QVariant &value)
{
    getSettingHandler(name).setValue(value);
}

void Settings::begin()
{
    Q_ASSERT(!m_isTransacted);
    m_isTransacted = true;
}

void Settings::end()
{
    Q_ASSERT(m_isTransacted);

    SettingsStorage::instance()->save();
    m_isTransacted = false;
    emit changed();
}

SettingHandler &Settings::getSettingHandler(const QString &name)
{
    auto it = m_settingHandlers.find(name);
    if (it == m_settingHandlers.end())
       throw SettingNotFoundError {name};

    return it.value();
}

const SettingHandler &Settings::getSettingHandler(const QString &name) const
{
    auto it = m_settingHandlers.find(name);
    if (it == m_settingHandlers.end())
        throw SettingNotFoundError {name};

    return it.value();
}

SettingHandler::SettingHandler(const QVariant &defaultValue)
    : m_value {defaultValue}
{
}

QVariant SettingHandler::getValue() const
{
    return m_value;
}

void SettingHandler::setValue(const QVariant &value)
{
    m_value = value;
}
