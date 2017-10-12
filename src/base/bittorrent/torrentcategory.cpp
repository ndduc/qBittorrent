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

#include "torrentcategory.h"

#include "base/exceptions.h"
#include "base/global.h"
#include "base/utils/fs.h"
#include "session.h"
#include "torrenthandle.h"

namespace BitTorrent
{
    TorrentCategory::TorrentCategory(Session *session, TorrentCategory *parentCategory
                                     , const QString &name, const QString &savePath)
        : QObject {session}
        , m_name {name}
    {
        setSavePath(savePath);
        setParentCategory(parentCategory); // can throw
    }

    TorrentCategory::~TorrentCategory()
    {
    }

    TorrentCategory *TorrentCategory::parentCategory() const
    {
        return m_parentCategory;
    }

    void TorrentCategory::setParentCategory(TorrentCategory *parentCategory)
    {
        if (parentCategory == m_parentCategory)
            return;

        if (parentCategory && parentCategory->m_subcategories.contains(m_name))
            throw RuntimeError(tr("Subcategory with the given name already exists."));

        if (m_parentCategory)
            m_parentCategory->m_subcategories.remove(m_name);

        if (parentCategory)
            parentCategory->m_subcategories.insert(m_name, this);

        m_parentCategory = parentCategory;
    }

    QString TorrentCategory::name() const
    {
        return m_name;
    }

    QString TorrentCategory::fullName() const
    {
        if (!parentCategory())
            return name();

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

        for (TorrentCategory *subCategory : asConst(m_subcategories)) {
            if ((subCategory == category) || subCategory->contains(category))
                return true;
        }

        return false;
    }

    bool TorrentCategory::contains(const TorrentHandle *torrent) const
    {
        if (!torrent) return false;
        return (this == torrent->category()) || contains(torrent->category());
    }
    
    CategoryDict TorrentCategory::subcategories() const
    {
        return m_subcategories;
    }
}
