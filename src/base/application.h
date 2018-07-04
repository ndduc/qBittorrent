/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2015, 2018  Vladimir Golovnev <glassez@yandex.ru>
 * Copyright (C) 2006  Christophe Dumez <chris@qbittorrent.org>
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

#include <QObject>
#include <QPointer>

#include "logger.h"
#include "preferences.h"
#include "profile.h"
#include "settingsstorage.h"
#include "types.h"

#ifndef DISABLE_WEBUI
class WebUI;
#endif

class FileLogger;

namespace BitTorrent
{
    class TorrentHandle;
}

namespace RSS
{
    class Session;
    class AutoDownloader;
}

class Application : public virtual QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Application)

public:
    explicit Application(Profile *profile);
    ~Application() override;

    static Application *instance();

    Logger *log() const;
    Preferences *preferences() const;
    Profile *profile() const;
    SettingsStorage *settings() const;

    // FileLogger properties
    bool isFileLoggerEnabled() const;
    void setFileLoggerEnabled(bool value);
    QString fileLoggerPath() const;
    void setFileLoggerPath(const QString &path);
    bool isFileLoggerBackup() const;
    void setFileLoggerBackup(bool value);
    bool isFileLoggerDeleteOld() const;
    void setFileLoggerDeleteOld(bool value);
    int fileLoggerMaxSize() const;
    void setFileLoggerMaxSize(int bytes);
    int fileLoggerAge() const;
    void setFileLoggerAge(int value);
    int fileLoggerAgeType() const;
    void setFileLoggerAgeType(int value);

protected:
    ShutdownAction shutdownAction() const;
    virtual void cleanup();
    virtual bool confirmShutdown() const = 0;
    virtual void shutdown() = 0;

private slots:
    void torrentFinished(BitTorrent::TorrentHandle *const torrent);
    void allTorrentsFinished();

private:
    void runExternalProgram(const BitTorrent::TorrentHandle *torrent) const;
    void sendNotificationEmail(const BitTorrent::TorrentHandle *torrent);

    Logger *m_log;
    Preferences *m_preferences;
    Profile *m_profile;
    SettingsStorage *m_settings;

#ifndef DISABLE_WEBUI
    WebUI *m_webui;
#endif

    // FileLog
    QPointer<FileLogger> m_fileLogger;

    ShutdownAction m_shutdownAction;

    static Application *m_instance;
};

// Helper function
void LogMsg(const QString &message, const Log::MsgType &type = Log::NORMAL);
