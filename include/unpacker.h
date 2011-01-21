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

#ifndef __UNPACKER_H__
#define __UNPACKER_H__

#include <string>
#include "base_types.h"
#include "fat.h"

class Unpacker : private NonCopyable
{
public:
    Unpacker(FILE* pak_file, bool verbose, bool list_only=false, bool no_caps=false) 
        : in_f(pak_file)
        , fat(pak_file, no_caps)
        , verbose(verbose)
        , list_only(list_only)
    {
        if(list_only) this->verbose = true;
    }
    
    void unpack();
private:
    void unpackDir(const PakDir& dir);
    void simpleCopy(FILE* out, size_t len);
    //
    FILE* in_f;
    FAT fat;
    // options
    bool verbose;
    bool list_only;
};

#endif //__EXTRACTOR_H__