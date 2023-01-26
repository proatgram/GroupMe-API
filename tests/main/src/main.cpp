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
#include <cstdlib>

#include "UserSet.hpp"
#include "User.h"

int main(int argc, char** argv) {

    GroupMe::UserSet set;

    std::shared_ptr<GroupMe::User> user1(std::make_shared<GroupMe::User>("123123", "Nickname", "http::amogus.net/usr.png", "asdasd", "asd.com", "asdGHA"));
    std::shared_ptr<GroupMe::User> user2(std::make_shared<GroupMe::User>("asdasd123123", "Nickname", "http::amogus.net/usr.png", "asdasd", "asd.com", "asdGHA"));

    set.insert(user1);
    set.insert(user2);
    set.find(user2)->get()->setEmail("jahsgdjashgd");
    std::cout << set.find("asdasd123123")->get()->getEmail() << std::endl;

    return EXIT_SUCCESS;
}
