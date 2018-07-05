/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2015-2018  Vladimir Golovnev <glassez@yandex.ru>
 * Copyright (C) 2010  Christophe Dumez <chris@qbittorrent.org>
 * Copyright (C) 2010  Arnaud Demaiziere <arnaud@qbittorrent.org>
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

#include "rss_feed.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include "../global.h"
#include "rss_article.h"

const QString KEY_URL(QStringLiteral("url"));
const QString KEY_TITLE(QStringLiteral("title"));
const QString KEY_LASTBUILDDATE(QStringLiteral("lastBuildDate"));
const QString KEY_ISLOADING(QStringLiteral("isLoading"));
const QString KEY_HASERROR(QStringLiteral("hasError"));
const QString KEY_ARTICLES(QStringLiteral("articles"));

using namespace RSS;

Feed::Feed(qint64 id, const QString &url, const QString &path)
    : Item {id, path}
    , m_url {url}
{
}

QString Feed::url() const
{
    return m_url;
}

QJsonValue Feed::toJsonValue(bool withData) const
{
    QJsonObject jsonObj;
    jsonObj.insert(KEY_URL, url());

    if (withData) {
        jsonObj.insert(KEY_TITLE, title());
        jsonObj.insert(KEY_LASTBUILDDATE, lastBuildDate());
        jsonObj.insert(KEY_ISLOADING, isLoading());
        jsonObj.insert(KEY_HASERROR, hasError());

        QJsonArray jsonArr;
        for (Article *article : asConst(articles()))
            jsonArr << article->toJsonObject();
        jsonObj.insert(KEY_ARTICLES, jsonArr);
    }

    return jsonObj;
}
