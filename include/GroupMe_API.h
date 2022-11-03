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

#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>

#include "base/Authenticator.h"
#include "base/Picture.h"
#include "base/Video.h"
#include "base/File.h"

namespace GroupMe {

    class Main {
        public:

            Main(std::string token);

            void getGroups();

            void getChats();

            void createChat();

            void createGroup(std::string groupName);

            bool changeName(std::string name);

            bool changeAvatar(std::string path);

        private:
        
            std::string m_token;
    };

}
