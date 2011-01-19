/*
 *   This file is part of arx-unpacker.
 *
 *   arx-unpacker is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   arx-unpacker is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with arx-unpacker.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef __FAT_H__
#define __FAT_H__

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <vector>

// file info
struct PakFile
{
    std::string name;
    uint32_t offset;
    uint32_t flags;
    uint32_t p3;
    uint32_t size;

    enum Flags { PACKED = 1 };
};

// directory info
struct PakDir
{
    std::string          name;
    std::vector<PakFile> files;
};


// Arx .pak file allocation table
class FAT
{
public:
    FAT(const char* fname);
    ~FAT();
    
    std::vector<PakDir> dirs;
private:
    FAT(); // no default construction

    FILE* f;          // handle to .pak file
    uint8_t *raw_fat; // place where fat stored in memory

    void readFAT();   // all things happens there
};

// arx fat decryptor
class FATDecryptor
{
    std::string key_str;
    size_t      key_pos;
    uint8_t    *raw_fat;
    size_t      fat_size;
public:
    FATDecryptor(const char* key, uint8_t* fat_loc, size_t fat_sz) 
        : key_str(key), key_pos(0), raw_fat(fat_loc), fat_size(fat_sz) {};

    uint8_t  decryptChar();
    uint32_t decryptDword();
    char*    decryptString();
    size_t   bytesLeft() { return fat_size; }
private:
    FATDecryptor(); // not intended to use with default constructor
};


#endif // __FAT_H__