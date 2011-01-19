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

#ifndef __FILE_HANDLE_H__
#define __FILE_HANDLE_H__

#include <stdio.h>
#include "base_types.h"

class FileHandle : private NonCopyable
{
	public:
		FileHandle(FILE* f) : fh(f) {}
		FileHandle(const char* fname, const char* mode)
		{
			open(fname, mode);
		}

		~FileHandle() throw() { close(); }
		void open(const char* fname, const char* mode) { fh = ::fopen(fname, mode); }
		int close() 
		{ 
			int r = 0;
			if(fh) { r = ::fclose(fh); fh = NULL; }
			return r; 
		}
		operator FILE*() { return fh; }
	private:
		FILE* fh;	
};

// close is the only operation that required do through class call
inline int fclose(FileHandle& fh) { return fh.close(); }

#endif // __FILE_HANDLE_H__

