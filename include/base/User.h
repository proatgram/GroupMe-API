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
            User(const std::string& userID, const std::string& userNickname, const std::string& userProfileImageURL, const std::string& userPhoneNumber, const std::string& userEmail, const std::string& userGUID);

            std::string getUserID() const;

            void setUserID(const std::string& userID);

            std::string getUserNickname() const;

            void setUserNickname(const std::string& userNickname);

            std::string getUserProfileImageURL() const;

            void setUserProfileImageURL(const std::string& userProfileImageURL);

            std::string getUserPhoneNumber() const;

            void setUserPhoneNumber(const std::string& userPhoneNumber);

            std::string getUserEmail() const;

            void setUserEmail(const std::string& userEmail);

            std::string getUserGUID() const;

            void setUserGUI(const std::string& userGUID);

            bool usingSMS() const;

            bool operator==(const User& user) const;

            bool operator!=(const User& user) const;

        private:
            
            std::string m_userID;

            std::string m_userNickname;

            std::string m_userProfileImageURL;

            std::string m_userPhoneNumber;

            std::string m_userEmail;

            std::string m_userGUID;

            bool m_isSMS;
    };
}
