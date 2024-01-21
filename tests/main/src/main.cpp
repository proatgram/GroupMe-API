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

#include "GroupChat.h"

#include "File.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
    GroupMe::GroupChat achat("", "");
    GroupMe::GroupChat chat(std::move(achat));

    std::cout << "Group name: " << chat.getName() << std::endl;
    chat.queryMessages().wait();

    std::shared_ptr<GroupMe::File> file = std::make_shared<GroupMe::File>("", std::filesystem::path(""), "");
    std::cout << file->upload().get() << std::endl;
    GroupMe::Message message("Message", "");

    message.attach(file);
    chat.sendMessage(message).wait();

    for (const auto &subgroup : chat.getSubGroups()) {
        subgroup->sendMessage(message).wait();
    }

    return EXIT_SUCCESS;
}
