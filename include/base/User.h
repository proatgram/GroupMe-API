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

            User();

            std::string getID() const;

            void setID(const std::string& userID);

            std::string getNickname() const;

            void setNickname(const std::string& userNickname);

            std::string getProfileImageURL() const;

            void setProfileImageURL(const std::string& userProfileImageURL);

            std::string getPhoneNumber() const;

            void setPhoneNumber(const std::string& userPhoneNumber);

            std::string getEmail() const;

            void setEmail(const std::string& userEmail);

            std::string getGUID() const;

            void setGUID(const std::string& userGUID);

            std::string getLocale() const;

            void setLocal(const std::string& locale);

            std::string getZipCode() const;

            void setZipCode(const std::string& zipCode);

            std::string getShareURL() const;

            void setShareURL(const std::string& shareURL);

            std::string getShareQRCodeURL() const;

            void setShareQRCodeURL(const std::string& shareQRCodeURL);

            unsigned int getCreatedAt() const;

            void setCreatedAt(unsigned int createdAt);

            unsigned int getUpdatedAt() const;

            void setUpdatedAt(unsigned int updatedAt);

            bool usingSMS() const;

            void setUsingSMS(bool usingSMS);

            bool getFacebookConnected() const;

            void setFacebookConnected(bool facebookConnected);

            bool getTwitterConnected() const;

            void setTwitterConnected(bool twitterConnected);

            bool operator==(const User& user) const;

            bool operator!=(const User& user) const;

        protected:
            
            std::string m_userID;

            std::string m_userNickname;

            std::string m_userProfileImageURL;

            std::string m_userPhoneNumber;

            std::string m_userEmail;

            std::string m_userGUID;

            std::string m_locale;

            std::string m_zipCode;

            std::string m_shareURL;

            std::string m_shareQRCodeURL;

            unsigned int m_createdAt;

            unsigned int m_updatedAt;

            bool m_isSMS;

            bool m_isFacebookConnected;

            bool m_isTwitterConnected;


    };
}
