/*
    This is a library used to communicate with the GroupMe API efficiently and seamlessly.
    Copyright (C) 2022 Timothy Hutchins

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <fstream>
#include <cstdlib>


#include "util/AVFileMem.h"

int main(int argc, char** argv) {
    
    std::fstream file("/home/thetimbrick/Videos/vid.mp4", std::fstream::in | std::fstream::binary);

    std::vector<uint8_t> vec((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    GroupMe::Util::AVFormat mem(vec);

    std::cout << (mem->duration / AV_TIME_BASE) << std::endl;

    return EXIT_SUCCESS;
}
