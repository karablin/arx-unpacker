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

#include "unpacker.h"

#if defined(_WIN32)
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include "app_exception.h"
#include "file_handle.h"
extern "C" {
#include "blast.h"
}

/*
 * Data decompression callbacks
 */
namespace {

const size_t BUFFER_SIZE = 2048;
struct BlastReadContext
{
    uint8_t buffer[BUFFER_SIZE];
    FILE*   f;
    size_t  size;
};

unsigned blast_read(void *how, unsigned char **buf)
{
    BlastReadContext *ctx = (BlastReadContext *)how;
    *buf = ctx->buffer;
    
    int n = fread(ctx->buffer, 1, std::min(ctx->size, BUFFER_SIZE), ctx->f);

    return n;
}

int blast_write(void *how, unsigned char *buf, unsigned len)
{
    FILE *f = (FILE*)how;

    fwrite(buf, 1, len, f);

    return 0;
}

};

/*
 * Unpacker methods
 */

void Unpacker::simpleCopy(FILE* out, size_t len)
{
    uint8_t buf[BUFFER_SIZE];
    while(len) {
        int n = fread(buf, 1, std::min(len, BUFFER_SIZE), in_f);
        fwrite(buf, 1, n, out);
        len -= n;
    }
}

void Unpacker::unpackDir(const PakDir& dir)
{
    size_t f_cnt = dir.files.size();
    for(size_t i=0; i<f_cnt; ++i) {
        const PakFile &f = dir.files[i];
        printf("[F]\t%s\n", f.name.c_str());

        std::string full_name = dir.name + f.name;
        FileHandle out_f(fopen(full_name.c_str(), "wb"));
        if(!out_f)
            throw AppException("can't create output file");

        fseek(in_f, f.offset, SEEK_SET);

        if(f.flags & PakFile::PACKED) {
            // unpack
            BlastReadContext rctx;
            rctx.f    = in_f;
            rctx.size = f.size;

            blast(blast_read, &rctx, blast_write, out_f);
        } else {
            // simple copy
            simpleCopy(out_f, f.size);
        }
    }
}

void Unpacker::unpack()
{
    size_t d_cnt = fat.dirs.size();
    for(size_t i=0; i<d_cnt; ++i) {
        const PakDir &d = fat.dirs[i];
        printf("[D] %s\n", d.name.c_str());

#if defined(_WIN32)
        mkdir(d.name.c_str());
#else
        mkdir(d.name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
        unpackDir(d);
    }
}
