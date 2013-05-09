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


#ifndef PAGEINDEXINTERN_H
#define PAGEINDEXINTERN_H

#include "page/data/PageIndex.h"

struct key_info_t {
    KeyValue* key_val;
    page_id_t page_id;
};

/**
 * PAGE STRUCTURE:
 *
 * x |   HEADER
 * 4 |   autoincrement_info(int 4)
 * 8 |   keysOnPage (int 2) | keysSize (int 2) | writeKeyPos(int 2)
 * 12|   prevPageId(int 4) | nextPageId(int 4) | parentPageId (int 4)
 * 6 |   len (int 2) infimumKeyPage(int 4)
 * x |*  len (int 2) | page_id (int 4) | values*
 * end| page directory (ptrs int 2 to keys)
 */
class PageIndexIntern : public PageIndex
{
public:
    enum IndexInternOffset {
        OFFSET_INFIMUM = OFFSET_ROWS_LIST
    };
    
    PageIndexIntern(ByteData* data);

    virtual void initEmpty();

    page_id_t pagePtrAt( uint16_t pos ) const;
    
    void replaceKey( KeyValue* valToReplace, KeyValue* replaceVal, page_id_t newPageId = 0 );

    void insertKey( KeyValue* key_value, page_id_t page_id );
    page_id_t findMatchingNode( KeyValue* key_value ) const;

    key_info_t splitNode( PageIndexIntern* pageIndex, KeyValue* insert_key, page_id_t ref_page, bool& return_insert );
    void setInfimumKey(page_id_t page_id);
    page_id_t infimumKey() const;

    virtual KeyValue* firstKeyValue(KeyInfo* key_info) const;
    virtual void redistributeNode(PageIndex* pageIndex, PageIndex* anchorNode, KeyInfo* key_info, bool atEnd = true);
private:
    virtual KeyValue* createKeyValueAtPtr(KeyInfo* key_info, uint16_t pos) const;

    void writeKeyDataAt( uint16_t pos , KeyValue* key_value, page_id_t page_id );
    void writeKeyDataAtPtr(uint16_t pos , KeyValue* key_value, page_id_t page_id);

};

#endif // PAGEINDEXINTERN_H
