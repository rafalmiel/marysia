/*
    Marysia database server
    Copyright (C) 2011  Rafal Mielniczuk rafalm3@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef PAGEINDEX_H
#define PAGEINDEX_H

#include "page/Page.h"

class KeyInfo;
class PageTableInfo;
class KeyValue;

/**
 * PAGE STRUCTURE:
 *
 * x|   HEADER
 * 4|   autoincrement_info(int 4)
 * 8|   rowsOnPage (int 2) | rowsSize (int 2) | writeRowPos(int 2)
 * 8|   prevPageId(int 4) | nextPageId(int 4)
 * x|*  len (int 2) | value_ptrs* (int 2) | values*
 * end| page directory (ptrs int 2 to rows)
 */
class PageIndex : public Page
{
public:
    PageIndex(ByteData* data);
    
    enum IndexOffset
    {
        OFFSET_AI_COUNTER = PAGE_HEADER_SIZE,
        OFFSET_INDEX_INFO = OFFSET_AI_COUNTER + 4,
        OFFSET_NEIGHBOUR_PAGES_ID = OFFSET_INDEX_INFO + 8,
        OFFSET_ROWS_LIST = OFFSET_NEIGHBOUR_PAGES_ID + 8
    };

    bool deleteRow( KeyValue* key_value, bool findExact = true );
    void deleteRowAt( uint16_t pos );

    KeyValue* keyValueAt( KeyInfo* key_info, uint16_t pos ) const;
    virtual KeyValue* firstKeyValue( KeyInfo* key_info ) const = 0;

    virtual void initEmpty();
    void setPageTableinfo( PageTableInfo* page_tableinfo );
    
    uint32_t autoincrementCounter() const;
    void setAutoincrementCounter(uint32_t cntr);
    
    uint16_t rowsOnPage() const;
    uint16_t rowsSize() const;
    uint16_t writePos() const;

    uint16_t freeSpace() const;
    uint16_t usedSpace() const;
    int16_t fillFactor() const;
    
    uint32_t prevPage() const;
    uint32_t nextPage() const;
    
    bool isRoot() const;
    bool isLeaf() const;
    bool isIntern() const;

    bool hasEnoughSpace(uint16_t size) const { return freeSpace() > size; }
    bool hasEnoughData() const;
    bool isFull() const { return rowsOnPage() >= maxRowsOnPage(); }
    bool canRedistributeData( bool fromEnd = true ) const;
    
    void setNextPage(page_id_t page_id);
    void setPrevPage(page_id_t page_id);
    
    void setRowsOnPage( uint16_t rows );
    void setRowsSize( uint16_t size );
    void setWritePos( uint16_t pos );

    uint16_t insertPos(KeyValue* key_value, bool& found) const;

    void mergeNode( PageIndex* pageIndex, bool atEnd = true );
    virtual void redistributeNode( PageIndex* pageIndex, PageIndex* anchorNode, KeyInfo* key_info, bool atEnd = true ) = 0;
    uint16_t findMatchingEntry( KeyValue* key_value ) const;
    KeyValue* findMatchingKeyValue( KeyValue* key_value ) const;
    
    friend class PageIndexLeaf;
    friend class PageIndexIntern;
    friend class BTree;
protected:
    virtual KeyValue* createKeyValueAtPtr( KeyInfo* key_info, uint16_t pos ) const = 0;

    uint16_t dictStartPos() const;

    void storePosInDict(uint16_t place_num, uint16_t value);
    uint16_t removePosFromDict(uint16_t place_num, uint16_t count = 1);
    uint16_t posFromDict(uint16_t place_num) const;

    uint16_t findDictEntryNum( KeyValue* key_value, bool& found_exact ) const;
    uint16_t findInsertPos( KeyValue* key_value, bool& found_exact ) const;

    virtual void writeRawDataAtPtr(uint16_t pos, uint8_t* row_data, uint16_t size);
    void deleteDataAtPtr(uint16_t pos);
    void saveRawDataAtEnd(uint8_t* raw_data, uint16_t size);
    void saveRawDataAtBegining(uint8_t* raw_data, uint16_t size);
    void updateWriteValue(uint16_t searchStartPtr);

    uint8_t* rawEntryAt( uint16_t dictPos, uint16_t& size ) const;

    uint16_t rowSize() const;
    uint16_t maxRowsOnPage() const;
    uint16_t minRowsOnPage() const;
    
    PageTableInfo* m_page_tableinfo;
};

#endif // PAGEINDEX_H
