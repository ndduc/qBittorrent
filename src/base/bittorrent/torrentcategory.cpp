/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2017  Vladimir Golovnev <glassez@yandex.ru>
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

#include "torrentcategory.h"

#include "../utils/fs.h"
#include "session.h"
#include "torrenthandle.h"

namespace BitTorrent
{
    TorrentCategory::TorrentCategory(QObject *parent)
        : QObject {parent}
    {
    }

    TorrentCategory::~TorrentCategory()
    {
        qDeleteAll(m_subCategories);
    }

    TorrentCategory *TorrentCategory::parentCategory() const
    {
        return m_parentCategory;
    }

    bool TorrentCategory::addCategory(const QString &name, TorrentCategory *category)
    {
        Q_ASSERT(category);

        if (name.isEmpty()) return false;
        if (m_subCategories.contains(name)) return false;

        if (category->parentCategory())
            category->parentCategory()->m_subCategories.remove(name());

        m_subCategories.insert(name, category);
        emit categoryAdded(name, category);
    }

    QString TorrentCategory::name() const
    {
        if (!parentCategory())
            return "";

        return parentCategory()->m_subCategories.key(this);
    }

    QString TorrentCategory::fullName() const
    {
        if (!parentCategory())
            return "";

        const QString parentFullName = parentCategory()->fullName();
        if (parentFullName.isEmpty())
            return name();

        return parentFullName + QLatin1Char('/') + name();
    }

    QString TorrentCategory::savePath() const
    {
        return m_savePath;
    }

    void TorrentCategory::setSavePath(const QString &savePath)
    {
        QString newSavePath = Utils::Fs::fromNativePath(savePath);
        if (m_savePath == newSavePath) return;

        m_savePath = newSavePath;
        emit savePathChanged();
    }

    bool TorrentCategory::contains(const TorrentCategory *category) const
    {
        if (!category) return false;

        for (auto subCategory : qAsConst(m_subCategories)) {
            if (subCategory == category || subCategory->contains(category))
                return true;
        }

        return false;
    }

    bool TorrentCategory::contains(const TorrentHandle *torrent) const
    {
        if (!torrent) return false;
        return (this == torrent->category()) || contains(torrent->category());
    }
}
