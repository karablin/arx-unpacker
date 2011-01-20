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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "app_exception.h"
#include "fat.h"

namespace _impl
{
    const char* full_key = "AVQF3FCKE50GRIAYXJP2AMEYO5QGA0JGIIH2NHBTVOA1VOGGU5H3GSSIARKPRQPQKKYEOIAQG1XRX0J4F5OEAEFI4DD3LL45VJTVOA1VOGGUKE50GRIAYX";
    const char* demo_key = "NSIARKPRQPHBTE50GRIH3AYXJP2AMF3FCEYAVQO5QGA0JGIIH2AYXKVOA1VOGGU5GSQKKYEOIAQG1XRX0J4F5OEAEFI4DD3LL45VJTVOA1VOGGUKE50GRI";
    
    const char* selectKey(uint32_t first_bytes)
    {
        switch(first_bytes) {
        case 0x46515641: 
            return full_key;
        case 0x4149534E: 
            return demo_key;
        default:
            throw AppException("cannot determine type of decrypting key");
        }

    }

    const char* ERR_BAD_FILE = "error while parsing file allocation table";

    inline void check(bool if_true, const char* exc_msg)
    {
        if(if_true)
            throw AppException(exc_msg);
    }
}
using namespace _impl;


FAT::FAT(FILE* f)
    : raw_fat(0)
{
    readFAT(f);
}

FAT::~FAT()
{
    free(raw_fat);
    raw_fat = 0;
}

void FAT::readFAT(FILE* f)
{
    uint32_t fat_offset;
    uint32_t fat_size;

    // get table offset and size
    check(1 != fread(&fat_offset, sizeof(fat_offset), 1, f), ERR_BAD_FILE);
    check(0 != fseek(f, fat_offset, SEEK_SET), ERR_BAD_FILE);
    check(1 != fread(&fat_size, sizeof(fat_size), 1, f), ERR_BAD_FILE);

    // allocate space for raw fat data and read it
    raw_fat = (uint8_t*)malloc(fat_size);
    check(1 != fread(raw_fat, fat_size, 1, f), ERR_BAD_FILE);

    // figure out what key is used
    FATDecryptor dec(selectKey(*((uint32_t*)raw_fat)), raw_fat, fat_size); 

    while(dec.bytesLeft())
    {
        // read dir
        char* dir_name = dec.decryptString();

        if(*dir_name != '\0') {
            PakDir d;
            d.name = dir_name;
            dirs.push_back(d);
        }

        // read files count
        uint32_t files_count = dec.decryptDword();

        while(files_count--) {
            PakFile f;

            f.name = dec.decryptString();
            f.offset = dec.decryptDword();
            f.flags  = dec.decryptDword();
            f.p3     = dec.decryptDword();
            f.size   = dec.decryptDword();

            dirs.back().files.push_back(f);
        }
    }
}

/*
 * FATDecryptor methods
 */

uint8_t FATDecryptor::decryptChar()
{
    uint8_t c = (*raw_fat ^= key_str[key_pos]);
    
    if(++key_pos >= key_str.size()) 
        key_pos = 0;
    
    ++raw_fat;
    --fat_size;

    return c;
}

uint32_t FATDecryptor::decryptDword()
{
    uint32_t* rval = (uint32_t*)raw_fat;

    for(size_t i = 0; i < 4; ++i) {
        decryptChar();
    }

    return *rval;
}

char* FATDecryptor::decryptString() 
{
    char* str = (char*)raw_fat;
    while( decryptChar() );

    return str;
}
