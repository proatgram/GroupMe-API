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
#include <memory>

namespace GroupMe {
    class User {
        public:
            User(std::string userID, std::string userNickname, std::string userProfileImageURL, std::string userPhoneNumber, std::string userEmail, std::string userGUID);

            std::string getUserID();

            const std::string getUserID() const;

            std::string getUserNickname();

            const std::string getUserNickname() const;

            std::string getUserProfileImageURL();

            const std::string getUserProfileImageURL() const;

            std::string getUserPhoneNumber();

            const std::string getUserPhoneNumber() const;

            std::string getUserEmail();

            const std::string getUserEmail() const;

            std::string getUserGUID();

            const std::string getUserGUID() const;

            bool operator==(const User& user) const;

            bool operator!=(const User& user) const;

        private:
            
            std::string m_userID;

            std::string m_userNickname;

            std::string m_userProfileImageURL;

            std::string m_userPhoneNumber;

            std::string m_userEmail;

            std::string m_userGUID;
    };
}
