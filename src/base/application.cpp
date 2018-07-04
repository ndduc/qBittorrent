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

#include "application.h"

#include <QDebug>
#include <QProcess>

#include "base/bittorrent/session.h"
#include "base/bittorrent/torrenthandle.h"
#include "base/iconprovider.h"
#include "base/net/smtp.h"
#include "base/utils/fs.h"
#include "base/utils/misc.h"
#include "base/utils/string.h"
#include "filelogger.h"

#ifndef DISABLE_WEBUI
#include "webui/webui.h"
#endif

namespace
{
#define SETTINGS_KEY(name) "Application/" name

    // FileLogger properties keys
#define FILELOGGER_SETTINGS_KEY(name) QStringLiteral(SETTINGS_KEY("FileLogger/") name)
    const QString KEY_FILELOGGER_ENABLED = FILELOGGER_SETTINGS_KEY("Enabled");
    const QString KEY_FILELOGGER_PATH = FILELOGGER_SETTINGS_KEY("Path");
    const QString KEY_FILELOGGER_BACKUP = FILELOGGER_SETTINGS_KEY("Backup");
    const QString KEY_FILELOGGER_DELETEOLD = FILELOGGER_SETTINGS_KEY("DeleteOld");
    const QString KEY_FILELOGGER_MAXSIZEBYTES = FILELOGGER_SETTINGS_KEY("MaxSizeBytes");
    const QString KEY_FILELOGGER_AGE = FILELOGGER_SETTINGS_KEY("Age");
    const QString KEY_FILELOGGER_AGETYPE = FILELOGGER_SETTINGS_KEY("AgeType");

    const QString LOG_FOLDER("logs");

    const int MIN_FILELOG_SIZE = 1024; // 1KiB
    const int MAX_FILELOG_SIZE = 1000 * 1024 * 1024; // 1000MiB
    const int DEFAULT_FILELOG_SIZE = 65 * 1024; // 65KiB
}

Application *Application::m_instance = nullptr;

Application::Application(Profile *profile)
    : m_profile {profile}
#ifndef DISABLE_WEBUI
    , m_webui {}
#endif
    , m_shutdownAction {ShutdownAction::Exit}
{
    Q_ASSERT(m_instance == nullptr);
    m_instance = this;

    qRegisterMetaType<Log::Msg>("Log::Msg");

    m_log = new Logger;
    m_settings = new SettingsStorage;
    m_preferences = new Preferences;

    if (isFileLoggerEnabled()) {
        m_fileLogger = new FileLogger {fileLoggerPath(), isFileLoggerBackup(), fileLoggerMaxSize()
                       , isFileLoggerDeleteOld(), fileLoggerAge(), static_cast<FileLogger::FileLogAgeType>(fileLoggerAgeType())};
    }

    log()->addMessage(tr("qBittorrent %1 started.", "qBittorrent v3.2.0alpha started").arg(QBT_VERSION));
}

Application::~Application()
{
    cleanup();
}

Application *Application::instance()
{
    return m_instance;
}

Logger *Application::log() const
{
    return m_log;
}

Preferences *Application::preferences() const
{
    return m_preferences;
}

Profile *Application::profile() const
{
    return m_profile;
}

SettingsStorage *Application::settings() const
{
    return m_settings;
}

bool Application::isFileLoggerEnabled() const
{
    return settings()->loadValue(KEY_FILELOGGER_ENABLED, true).toBool();
}

void Application::setFileLoggerEnabled(bool value)
{
    if (value && !m_fileLogger)
        m_fileLogger = new FileLogger(fileLoggerPath(), isFileLoggerBackup(), fileLoggerMaxSize(), isFileLoggerDeleteOld(), fileLoggerAge(), static_cast<FileLogger::FileLogAgeType>(fileLoggerAgeType()));
    else if (!value)
        delete m_fileLogger;
    settings()->storeValue(KEY_FILELOGGER_ENABLED, value);
}

QString Application::fileLoggerPath() const
{
    return settings()->loadValue(KEY_FILELOGGER_PATH,
            QVariant(specialFolderLocation(SpecialFolder::Data) + LOG_FOLDER)).toString();
}

void Application::setFileLoggerPath(const QString &path)
{
    if (m_fileLogger)
        m_fileLogger->changePath(path);
    settings()->storeValue(KEY_FILELOGGER_PATH, path);
}

bool Application::isFileLoggerBackup() const
{
    return settings()->loadValue(KEY_FILELOGGER_BACKUP, true).toBool();
}

void Application::setFileLoggerBackup(bool value)
{
    if (m_fileLogger)
        m_fileLogger->setBackup(value);
    settings()->storeValue(KEY_FILELOGGER_BACKUP, value);
}

bool Application::isFileLoggerDeleteOld() const
{
    return settings()->loadValue(KEY_FILELOGGER_DELETEOLD, true).toBool();
}

void Application::setFileLoggerDeleteOld(bool value)
{
    if (value && m_fileLogger)
        m_fileLogger->deleteOld(fileLoggerAge(), static_cast<FileLogger::FileLogAgeType>(fileLoggerAgeType()));
    settings()->storeValue(KEY_FILELOGGER_DELETEOLD, value);
}

int Application::fileLoggerMaxSize() const
{
    int val = settings()->loadValue(KEY_FILELOGGER_MAXSIZEBYTES, DEFAULT_FILELOG_SIZE).toInt();
    return std::min(std::max(val, MIN_FILELOG_SIZE), MAX_FILELOG_SIZE);
}

void Application::setFileLoggerMaxSize(const int bytes)
{
    int clampedValue = std::min(std::max(bytes, MIN_FILELOG_SIZE), MAX_FILELOG_SIZE);
    if (m_fileLogger)
        m_fileLogger->setMaxSize(clampedValue);
    settings()->storeValue(KEY_FILELOGGER_MAXSIZEBYTES, clampedValue);
}

int Application::fileLoggerAge() const
{
    int val = settings()->loadValue(KEY_FILELOGGER_AGE, 1).toInt();
    return std::min(std::max(val, 1), 365);
}

void Application::setFileLoggerAge(const int value)
{
    settings()->storeValue(KEY_FILELOGGER_AGE, std::min(std::max(value, 1), 365));
}

int Application::fileLoggerAgeType() const
{
    int val = settings()->loadValue(KEY_FILELOGGER_AGETYPE, 1).toInt();
    return ((val < 0) || (val > 2)) ? 1 : val;
}

void Application::setFileLoggerAgeType(const int value)
{
    settings()->storeValue(KEY_FILELOGGER_AGETYPE, ((value < 0) || (value > 2)) ? 1 : value);
}

ShutdownAction Application::shutdownAction() const
{
    return m_shutdownAction;
}

void Application::cleanup()
{

}

void Application::runExternalProgram(const BitTorrent::TorrentHandle *torrent) const
{
    QString program = preferences()->getAutoRunProgram().trimmed();
    program.replace("%N", torrent->name());
    program.replace("%L", torrent->category());

    QStringList tags = torrent->tags().toList();
    std::sort(tags.begin(), tags.end(), Utils::String::naturalLessThan<Qt::CaseInsensitive>);
    program.replace("%G", tags.join(','));

#if defined(Q_OS_WIN)
    const auto chopPathSep = [](const QString &str) -> QString
    {
        if (str.endsWith('\\'))
            return str.mid(0, (str.length() -1));
        return str;
    };
    program.replace("%F", chopPathSep(Utils::Fs::toNativePath(torrent->contentPath())));
    program.replace("%R", chopPathSep(Utils::Fs::toNativePath(torrent->rootPath())));
    program.replace("%D", chopPathSep(Utils::Fs::toNativePath(torrent->savePath())));
#else
    program.replace("%F", Utils::Fs::toNativePath(torrent->contentPath()));
    program.replace("%R", Utils::Fs::toNativePath(torrent->rootPath()));
    program.replace("%D", Utils::Fs::toNativePath(torrent->savePath()));
#endif
    program.replace("%C", QString::number(torrent->filesCount()));
    program.replace("%Z", QString::number(torrent->totalSize()));
    program.replace("%T", torrent->currentTracker());
    program.replace("%I", torrent->hash());

    log()->addMessage(tr("Torrent: %1, running external program, command: %2").arg(torrent->name(), program));

#if defined(Q_OS_WIN)
    std::unique_ptr<wchar_t[]> programWchar(new wchar_t[program.length() + 1] {});
    program.toWCharArray(programWchar.get());

    // Need to split arguments manually because QProcess::startDetached(QString)
    // will strip off empty parameters.
    // E.g. `python.exe "1" "" "3"` will become `python.exe "1" "3"`
    int argCount = 0;
    LPWSTR *args = ::CommandLineToArgvW(programWchar.get(), &argCount);

    QStringList argList;
    for (int i = 1; i < argCount; ++i)
        argList += QString::fromWCharArray(args[i]);

    QProcess::startDetached(QString::fromWCharArray(args[0]), argList);

    ::LocalFree(args);
#else
    QProcess::startDetached(QLatin1String("/bin/sh"), {QLatin1String("-c"), program});
#endif
}

void Application::sendNotificationEmail(const BitTorrent::TorrentHandle *torrent)
{
    // Prepare mail content
    const QString content = tr("Torrent name: %1").arg(torrent->name()) + '\n'
        + tr("Torrent size: %1").arg(Utils::Misc::friendlyUnit(torrent->wantedSize())) + '\n'
        + tr("Save path: %1").arg(torrent->savePath()) + "\n\n"
        + tr("The torrent was downloaded in %1.", "The torrent was downloaded in 1 hour and 20 seconds")
            .arg(Utils::Misc::userFriendlyDuration(torrent->activeTime())) + "\n\n\n"
        + tr("Thank you for using qBittorrent.") + '\n';

    // Send the notification email
    Net::Smtp *smtp = new Net::Smtp(this);
    smtp->sendMail(preferences()->getMailNotificationSender()
                   , preferences()->getMailNotificationEmail()
                   , tr("[qBittorrent] '%1' has finished downloading").arg(torrent->name())
                   , content);
}

void Application::torrentFinished(BitTorrent::TorrentHandle *const torrent)
{
    // AutoRun program
    if (preferences()->isAutoRunEnabled())
        runExternalProgram(torrent);

    // Mail notification
    if (preferences()->isMailNotificationEnabled()) {
        log()->addMessage(tr("Torrent: %1, sending mail notification").arg(torrent->name()));
        sendNotificationEmail(torrent);
    }
}

void Application::allTorrentsFinished()
{
    const bool isExit = preferences()->shutdownqBTWhenDownloadsComplete();
    const bool isShutdown = preferences()->shutdownWhenDownloadsComplete();
    const bool isSuspend = preferences()->suspendWhenDownloadsComplete();
    const bool isHibernate = preferences()->hibernateWhenDownloadsComplete();

    const bool haveAction = isExit || isShutdown || isSuspend || isHibernate;
    if (!haveAction) return;

    if (isSuspend)
        m_shutdownAction = ShutdownAction::Suspend;
    else if (isHibernate)
        m_shutdownAction = ShutdownAction::Hibernate;
    else if (isShutdown)
        m_shutdownAction = ShutdownAction::Shutdown;

    if (!confirmShutdown()) return;

    // Actually shut down
    if (m_shutdownAction != ShutdownAction::Exit) {
        qDebug("Preparing for auto-shutdown because all downloads are complete...");
        // Disabling it for next time
        preferences()->setShutdownWhenDownloadsComplete(false);
        preferences()->setSuspendWhenDownloadsComplete(false);
        preferences()->setHibernateWhenDownloadsComplete(false);
    }

    shutdown();
}

void LogMsg(const QString &message, const Log::MsgType &type)
{
    Application::instance()->log()->addMessage(message, type);
}
