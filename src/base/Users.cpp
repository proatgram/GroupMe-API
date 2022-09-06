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

#include "base/Users.h"

using namespace GroupMe;

Users::Users() :
    m_users()
{

}

bool Users::addUser(std::shared_ptr<User> user, Users::Policy policy) {
    if (userExists(user, policy)) {
        return false;
    }
    m_users.push_back(user);
    return true;
}

bool Users::userExists(std::string userID, Users::Policy policy) {
    for (unsigned int i = 0; i < m_users.size(); i++) {
        std::shared_ptr<User> currentUser = m_users.at(i);
        if (currentUser->getUserID() == userID) {
            return true;
        }
    }
    return false;
}

bool Users::userExists(std::shared_ptr<User> user, Users::Policy policy) {
    if (policy == Users::Policy::Faithful) {
        for (unsigned int i = 0; i < m_users.size(); i++) {
            std::shared_ptr<User> currentUser = m_users.at(i);
            if (currentUser == user) {
                return true;
            }
        }
    }
    if (policy == Users::Policy::Strict) {
        for (unsigned int i = 0; i < m_users.size(); i++) {
            std::shared_ptr<User> currentUser = m_users.at(i);
            if (currentUser == user) {
                throw std::runtime_error("Policy: Strict exception thrown. User exists.");
            }
        }
    }
    return false;
}

bool Users::removeUser(std::shared_ptr<User> user, Users::Policy policy) {
    if (!userExists(user)) {
        if (policy == Users::Policy::Faithful) {
            return false;
        }
        if (policy == Users::Policy::Strict) {
            throw std::runtime_error("Policy: Strict exception thown. User does not exist.");
        }
    }
    if (policy == Users::Policy::Faithful) {
        for (unsigned int i = 0; i < m_users.size(); i++) {
            if (m_users.at(i)->getUserID() == user->getUserID()) {
                m_users.erase(m_users.begin() + i);
                return true;
            }
        }
    }
    if (policy == Users::Policy::Strict) {
        for (unsigned int i = 0; i < m_users.size(); i++) {
            if (m_users.at(i) == user) {
                m_users.erase(m_users.begin() + 1);
                return true;
            }
        }
    }
    return false;
}

std::shared_ptr<User> Users::grabUser(std::string userID, Users::Policy policy) {
    if (!userExists(userID)) {
        if (policy == Users::Policy::Faithful) {
            return std::make_shared<User>("NULL", "NULL", "NULL", "NULL", "NULL", "NULL");
        }
        else if (policy == Users::Policy::Strict) {
            throw std::runtime_error("Policy: Strict exception thrown. User does not exists");
        }
    }
    for (unsigned int i = 0; i < m_users.size(); i++) {
        if (m_users.at(i)->getUserID() == userID) {
            return m_users.at(i);
        }
    }
    return std::make_shared<User>("NULL", "NULL", "NULL", "NULL", "NULL", "NULL");
}
