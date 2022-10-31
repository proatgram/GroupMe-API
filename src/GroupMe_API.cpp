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

#include "GroupMe_API.h"

#include <fstream>

using namespace GroupMe;

Main::Main(std::string token) :
    m_token(token)

{
    std::fstream file("/home/thetimbrick/Videos/2022-10-29 21-41-14.mp4", std::ios::in | std::ios::out | std::ios::binary);

    std::string str;

    std::vector<uint8_t> vec;

    while(!file.eof()) {
        vec.push_back(file.get());
    }

    Video vid("***REMOVED***", vec, "85177831");
    std::cout << vid.upload().get() << std::endl;
}

bool Main::changeAvatar(std::string path) {

    return true;
}

bool Main::changeName(std::string name) {

    return true;
}

void Main::createChat() {

}

void Main::createGroup(std::string groupName) {

}

void Main::getChats() {

}

void Main::getGroups() {

}
