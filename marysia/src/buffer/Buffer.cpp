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


#include "Buffer.h"

#include <iostream>
#include <stdlib.h>

#include "io/File.h"
#include "page/PageFactory.h"
#include "utils/ByteData.h"

using namespace std;

Buffer::Buffer(const String& filename)
{
    File* file = new File();
    
    file->mopen(filename);
    appendFile(file);
}

Buffer::Buffer(File* file)
{
    appendFile(file);
}

Buffer::Buffer()
{

}

Buffer::~Buffer()
{
    for (file_map_t::const_iterator it = m_map.begin(); it != m_map.end(); ++it)
    {
        (*it).second->mclose();
        delete (*it).second;
    }

    m_map.clear();
}

void Buffer::appendFile(File *file)
{
    m_map[file->filename().c_str()] = file;
}

String Buffer::createBufferKey(const String& filename, page_id_t page_id)
{
    return filename + ":" + page_id;
}

Page* Buffer::getPage(page_id_t id, const String& filename, bool fixedSizePage)
{
    String key = createBufferKey(filename, id);

    if (m_buffer.find(key) == m_buffer.end())
    {
        ensureFileInFilemap(filename);
        
        const uint16_t size = (fixedSizePage)?PAGE_SIZE:m_map[filename.c_str()]->msize();
        
        uint8_t* data = new uint8_t[size];
        
        if ( fixedSizePage )
        {
            m_map[filename.c_str()]->mread(id*size,data,size);
        }
        else
        {
            m_map[filename.c_str()]->mread(0,data,size);
        }
        
        Page* page = PageFactory::createPage( new ByteData( data, size ) );
        page->setFilename( filename );

        buffer_item_struct* item = new buffer_item_struct;
        item->page = page;
        item->usage_count = 1;

        m_buffer[key] = item;
        return page;
    } else {
        m_buffer.at(key)->usage_count++;
        return m_buffer.at(key)->page;
    }
    

}

File* Buffer::fileByFilename(const String& filename)
{
    return m_map[ filename.c_str() ];
}

void Buffer::ensureFileInFilemap(const String& filename)
{
    File* file = NULL;

    if ( ! ( file = m_map[ filename.c_str() ] ) )
    {
        file = new File();
        file->mopen( filename );
        appendFile( file );
    }
}

void Buffer::writePageToDisk(Page* page)
{
    const String& filename = page->filename();
    
    ensureFileInFilemap( filename.c_str() );

    page_id_t page_id = page->pageId();

    if ( page->isDeallocated() )
    {
        page->resetData();
    }

    if (page->isFixedLenghtPage() && (page->pageId() == 129 ))
    {
        File file;
        file.mopen((String)"page_"+page_id);
        file.mwrite(0,page->data()->dataPtr(), page->data()->size());
        file.mflush();
        file.mclose();
    }
    
    ByteData* data = page->data();
    
    if ( page->isFixedLenghtPage() )
    {
        m_map[ filename.c_str() ]->mwrite( page_id * page->pageSize(), data->dataPtr(), data->size() );
    }
    else
    {
        m_map[ filename.c_str() ]->mwrite( 0, data->dataPtr(), data->size() );
    }
    
    m_map[ filename.c_str() ]->mflush();
}

void Buffer::savePage(Page* page)
{
    String key = createBufferKey(page->filename(), page->pageId());
    buffer_map_t::iterator it = m_buffer.find(key);
    

    if (it != m_buffer.end())
    {
        buffer_item_struct* item = it->second;
        item->usage_count--;

        //cout << "USAGE COUNT " << (uint32_t) item->usage_count << " page id = " << item->page->pageId() << endl;

        if (item->usage_count == 0)
        {
            if ( page->isDirty() )
            {
                writePageToDisk(page);
            }
            delete page;
            delete item;
            m_buffer.erase(key);
        } /*else {
            writePageToDisk(page);
        }*/
    } else {
        if ( page->isDirty() )
        {
            writePageToDisk(page);
        }
        delete page;
    }
}

