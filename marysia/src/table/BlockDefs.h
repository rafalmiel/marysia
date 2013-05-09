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

#ifndef BLOCKDEFS_H
#define BLOCKDEFS_H

#include "page/Page.h"

#define block_init_empty(block) \
block.block_header.block_len = 0; \
block.block_header.next_nr = 0; \
block.block_header.next_page = 0; \
block.block_data = 0; \

#define block_header_init_empty(bh) \
bh.block_len = 0; \
bh.next_nr = 0; \
bh.next_page = 0; \

#define is_block_header_empty(bh) \
((bh.next_page == 0) && (bh.next_nr == 0) && (bh.block_len == 0))

typedef struct {
    uint8_t chunk_nr;
    uint8_t chunk_count;
} block_dict_entry_t;

typedef struct {
    page_id_t next_page;
    uint8_t next_nr;
    uint16_t block_len;
} block_header_t;

typedef struct {
    block_header_t block_header;
    uint8_t* block_data;
} block_t;

typedef struct {
    page_id_t page_id;
    uint8_t nr;
} block_ptr_t;

typedef struct {
    block_ptr_t block_ptr;
    uint32_t block_total_len;
    uint8_t* block_data;
} block_desc_t;

typedef struct {
    page_id_t page_id;
    uint8_t nr;
    uint32_t total_len;
} long_data_info_t;

typedef struct {
    uint32_t total_len;
    uint8_t* raw_data;
} long_data_t;

#endif // BLOCKDEFS_H
