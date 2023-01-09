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
#include <unordered_set>
#include <boost/functional/hash.hpp>

#include "base/User.h"

namespace GroupMe {
    namespace {
        struct UserCompare {
            inline size_t operator()(const std::shared_ptr<User>& a, const std::shared_ptr<User>&b ) const {
                return *(a.get()) == *(b.get());
            }
        };

        struct UserHash {
            inline std::size_t operator()(const std::shared_ptr<User>& p) const {
                std::size_t idHash = std::hash<std::string>()(p.get()->getID());
                std::size_t nicknameHash = std::hash<std::string>()(p.get()->getNickname());
                std::size_t phonenumberHash = std::hash<std::string>()(p.get()->getPhoneNumber());
                std::size_t emailHash = std::hash<std::string>()(p.get()->getEmail());

                std::size_t hash = 0;

                boost::hash_combine(hash, idHash);
                boost::hash_combine(hash, nicknameHash);
                boost::hash_combine(hash, phonenumberHash);
                boost::hash_combine(hash, emailHash);

                return hash;
            }
        };
    }

    typedef std::unordered_set<std::shared_ptr<User>, UserHash, UserCompare> UserSet;
}
