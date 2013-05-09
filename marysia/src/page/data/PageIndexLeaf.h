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


#ifndef PAGEINDEXLEAF_H
#define PAGEINDEXLEAF_H

#include "page/data/PageIndex.h"

class Row;
class KeyValue;

/**
 * PAGE STRUCTURE:
 *
 * x|   HEADER
 * 4|   autoincrement_info(int 4)
 * 8|   rowsOnPage (int 2) | rowsSize (int 2) | writeRowPos(int 2)
 * 8|   prevPageId(int 4) | nextPageId(int 4) | parentPageId (int 4)
 * x|*  len (int 2) | value_ptrs* (int 2) | values*
 * end| page directory (ptrs int 2 to rows)
 **/
class PageIndexLeaf : public PageIndex
{
public:
    PageIndexLeaf(ByteData* data);

    Row* rowAt(uint16_t pos) const;

    void insertRow( Row* row, uint16_t pos );
    void insertRow( Row* row );

    void splitNode( PageIndexLeaf* pageIndex );

    virtual KeyValue* firstKeyValue( KeyInfo* key_info ) const;
    virtual void redistributeNode(PageIndex* pageIndex, PageIndex* anchorNode, KeyInfo* key_info, bool atEnd = true);
    
    
private:
    KeyValue* createKeyValueAtPtr( KeyInfo* key_info, uint16_t pos ) const;
    Row* createRowFromPtr( uint16_t pos ) const;

    void writeRowDataAt(uint16_t pos, Row* row);
    void writeRowDataAtPtr(uint16_t pos, Row* row);

    void updateFieldPtrs(uint16_t rowPtr);

    virtual void writeRawDataAtPtr(uint16_t pos, uint8_t* row_data, uint16_t size);
};

#endif // PAGEINDEXLEAF_H
