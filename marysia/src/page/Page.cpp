/*
 *  Marysia database server
 *  Copyright (C) 2011  Rafal Mielniczuk rafalm3@gmail.com
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "Page.h"

#include "io/File.h"
#include "utils/ByteData.h"

Page::Page(ByteData* data): m_is_deallocated( false )
{
    m_data = data;
    m_is_dirty = false;
}


Page::~Page()
{
    delete m_data;
}

page_id_t Page::pageId() const
{
    return m_data->readUInt32At(OFFSET_PAGE_ID);
}

Page::PageType Page::pageType() const
{
    return static_cast<PageType>(m_data->readUInt8At(OFFSET_PAGE_TYPE));
}

segment_id_t Page::segmentId() const
{
    return m_data->readUInt16At(OFFSET_SEGMENT_ID);
}

void Page::setPageId(page_id_t id)
{
    m_data->writeUInt32At(OFFSET_PAGE_ID, id);
    m_is_dirty = true;
}

void Page::setPageType(PageType type)
{
    m_data->writeUInt8At(OFFSET_PAGE_TYPE, type);
    m_is_dirty = true;
}

void Page::setSegmentId(segment_id_t segment_id)
{
    m_data->writeUInt16At(OFFSET_SEGMENT_ID, segment_id);
    m_is_dirty = true;
}

void Page::deallocate()
{
    m_is_deallocated = true;
    markDirty();
}

void Page::resetData()
{
    m_data->dataSetAt( 0, 0, m_data->size() );
}

void Page::flushToFile(const String& filename)
{
    File file;
    file.mopen(filename);
    if ( ! m_is_deallocated)
    {
        file.mwrite(0, m_data->dataPtr(), m_data->size());
    }
    else
    {
        m_data->dataSetAt(0, 0, m_data->size());
        file.mwrite(0, m_data->dataPtr(), m_data->size());
    }
   
    file.mflush();
    file.mclose();
}

