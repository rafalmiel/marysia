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

#ifndef BLOCKDATA_H
#define BLOCKDATA_H

#include <vector>

#include "page/data/PageBlockStorage.h"

class BlockData
{
public:
    BlockData();
    ~BlockData();
    
    void addBlock(const block_t& block);
    
    inline uint16_t blockCount() const { return m_blocks.size(); }
    block_t blockAt(uint16_t pos) const { return m_blocks.at(pos); }
    
private:
    typedef std::vector<block_t> block_vecotr_t; 
    block_vecotr_t m_blocks;
};

#endif // BLOCKDATA_H
