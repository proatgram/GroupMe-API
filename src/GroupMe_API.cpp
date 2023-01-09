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

    UserSet set;

    std::shared_ptr<User> user1(std::make_shared<User>(User("123123", "Nickname", "http::amogus.net/usr.png", "asdasd", "asd.com", "asdGHA")));
    std::shared_ptr<User> user2(std::make_shared<User>(User("asdasd123123", "Nickname", "http::amogus.net/usr.png", "asdasd", "asd.com", "asdGHA")));

    set.insert(user1);
    set.insert(user2);

    std::cout << set.count(user2) << std::endl << set.size() << std::endl;
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
