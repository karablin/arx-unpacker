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

#include <iostream>
#include "tclap/CmdLine.h"
#include "app_exception.h"
#include "file_handle.h"
#include "unpacker.h"


using namespace std;

int main(int argc, const char* argv[])
{
    if(argc < 2) {
        printf("you must specify filename to unpack\n");
        return -1;
    }

    const char* fname = argv[1];
    
    try {
        // command line options
        TCLAP::CmdLine cmd("Arx Fatalis .pak file unpacker", ' ', "0.2", true);
        TCLAP::UnlabeledValueArg<string> pak_name("input", "input file name", true, "", ".pak file", cmd);
        cmd.parse(argc, argv);

        // open input
        FileHandle in_f(pak_name.getValue().c_str(), "rb");
        if(!in_f)
            throw AppException("specified pak file not found");

        // unpack
        Unpacker unp(in_f, false);
        unp.unpack();

    } catch(std::exception &e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}
