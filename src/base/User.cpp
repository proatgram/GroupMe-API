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

#include "base/User.h"

using namespace GroupMe;

User::User(std::string userID, std::string userNickname, std::string userProfileImageURL, std::string userPhoneNumber, std::string userEmail, std::string userGUID) :
    m_userID(userID),
    m_userNickname(userNickname),
    m_userProfileImageURL(userProfileImageURL),
    m_userPhoneNumber(userPhoneNumber),
    m_userEmail(userEmail),
    m_userGUID(userGUID)
{

}

std::string User::getUserID() {
    return m_userID;
}

const std::string User::getUserID() const {
    return m_userID;
}

std::string User::getUserNickname() {
    return m_userNickname;
}

const std::string User::getUserNickname() const {
    return m_userNickname;
}

std::string User::getUserProfileImageURL() {
    return m_userProfileImageURL;
}

const std::string User::getUserProfileImageURL() const {
    return m_userProfileImageURL;
}

std::string User::getUserPhoneNumber() {
    return m_userPhoneNumber;
}

const std::string User::getUserPhoneNumber() const {
    return m_userPhoneNumber;
}

std::string User::getUserEmail() {
    return m_userEmail;
}

const std::string User::getUserEmail() const {
    return m_userEmail;
}

std::string User::getUserGUID() {
    return m_userGUID;
}

const std::string User::getUserGUID() const {
    return m_userGUID;
}


bool User::operator==(const User& user) const {
    if (this->getUserGUID() != user.getUserGUID()) {
        return false;
    }
    else if (this->getUserID() != user.getUserID()) {
        return false;
    }
    else if (this->getUserPhoneNumber() != user.getUserPhoneNumber()) {
        return false;
    }
    else if (this->getUserEmail() != user.getUserEmail()) {
        return false;
    }
    else if (this->getUserProfileImageURL() != user.getUserProfileImageURL()) {
        return false;
    }
    else if (this->getUserNickname() != user.getUserNickname()) {
        return false;
    }
    else {
        return true;
    }
}

bool User::operator!=(const User& user) const {
    return !(this->operator==(user));
}
