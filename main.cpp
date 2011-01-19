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

#include <exception>
#include <limits>
#include <direct.h>
#include "fat.h"
#include "file_handle.h"


extern "C" {
#include "blast.h"
}

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

void simple_write(FILE* in, FILE* out, size_t len)
{
    uint8_t buf[BUFFER_SIZE];
    while(len) {
        int n = fread(buf, 1, std::min(len, BUFFER_SIZE), in);
        fwrite(buf, 1, n, out);
        len -= n;
    }
}

void extractFiles(const PakDir& dir, FILE* in_f)
{
    size_t f_cnt = dir.files.size();
    for(size_t i=0; i<f_cnt; ++i) {
        const PakFile &f = dir.files[i];
        printf("[F]\t%s\n", f.name.c_str());

        std::string full_name = dir.name + f.name;
        FileHandle out_f = fopen(full_name.c_str(), "wb");
        if(!out_f)
            throw std::exception("can't create output file");

        fseek(in_f, f.offset, SEEK_SET);

        if(f.flags & PakFile::PACKED) {
            // unpack
            BlastReadContext rctx;
            rctx.f    = in_f;
            rctx.size = f.size;

            blast(blast_read, &rctx, blast_write, out_f);
        } else {
            // pure copy
            simple_write(in_f, out_f, f.size);
        }
    }
}

int main(int argc, const char* argv[])
{
    if(argc < 2) {
        printf("you must specify filename to unpack\n");
        return -1;
    }

    const char* fname = argv[1];
    
    try {
        FAT fat(fname);

        FileHandle in_f(fname, "rb");
        if(!in_f)
            throw std::exception("pak file is not accessible");

        size_t d_cnt = fat.dirs.size();
        for(size_t i=0; i<d_cnt; ++i) {
            const PakDir &d = fat.dirs[i];
            printf("[D] %s\n", d.name.c_str());

            mkdir(d.name.c_str());
            extractFiles(d, in_f);
        }

    } catch(std::exception &e) {
        printf("Error: %s\n", e.what());
    }

    return 0;
}