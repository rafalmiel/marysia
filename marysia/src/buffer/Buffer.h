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

#ifndef BUFFER_H
#define BUFFER_H

#include <map>
#include <string.h>

#include "page/Page.h"

class File;
class PageFactory;

#define DATA_FILE "data.bin"

using namespace std;

struct cmp_str
{
    bool operator()(char const *a, char const *b)
    {
        return strcmp(a, b) < 0;
    }
};

struct buffer_item_struct
{
    Page* page;
    uint8_t usage_count;
};

class Buffer
{
public:
    Buffer(const String& filename);
    Buffer(File* file);
    Buffer();

    virtual ~Buffer();

    void appendFile(File* file);

    Page* getPage(page_id_t id, const String& filename = "data.bin", bool fixedSizePage = true);
    void savePage(Page* page);

    
    File* fileByFilename(const String& filename);
    void ensureFileInFilemap(const String& filename);
private:

    void writePageToDisk(Page* page);

    typedef map<std::string, File*> file_map_t;
    typedef map<std::string, buffer_item_struct*> buffer_map_t;

    static String createBufferKey(const String& filename, page_id_t page_id);

    file_map_t m_map;
    buffer_map_t m_buffer;
};

#endif // BUFFER_H
