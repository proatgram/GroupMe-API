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

#include <string>
#include <memory>
#include <set>

#include "User.h"

namespace GroupMe {
    struct UserCompare {
        using is_transparent = void;

        inline size_t operator()(const std::shared_ptr<User>& first, const std::shared_ptr<User>& second) const {
            return static_cast<size_t>(first->getID() < second->getID());
        }

        inline size_t operator()(const std::shared_ptr<User>& user, const std::string& matchId) const {
            return static_cast<size_t>(user->getID() < matchId);
        }

        inline size_t operator()(const std::string& matchId, const std::shared_ptr<User>& user) const {
            return static_cast<size_t>(user->getID() < matchId);
        }
    };

    /**
     * @brief This type should be used to hold `GroupMe::User`'s
     *
     * Essentially this is just a std::set with some custom comparisons
     *
     */
    using UserSet = std::set<std::shared_ptr<User>, UserCompare, std::allocator<std::shared_ptr<User>>>;
}
