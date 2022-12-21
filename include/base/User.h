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

/**
 * This class holds user data for any generic user, such as the nickname,
 * phone number, email, etc.
 *
 * If you want to hold data for a user that has an access token (One that has been authenticated)
 * then you should use `GroupMe::Self`
 *
 * @brief Class to represent generic users
 *
 */
namespace GroupMe {
    class User {
        public:
            /**
             * This constructor should be used when you want to initialize a user with
             * their respective items.
             *
             * @brief Constructs a new `GroupMe::User` object
             *
             * @param userID The users unique ID [(See this)](https://dev.groupme.com/docs/v3#members)
             * @param userNickname The users name/nickname
             * @param userProfileImageURL The users profile image URL location. This should point to an image file
             * @param userPhoneNumber The users phone number
             * @param userEmail The users email
             * @param userGUID The users GUID [(See this)](https://dev.groupme.com/docs/v3#members)
             *
             */
            User(const std::string& userID, const std::string& userNickname, const std::string& userProfileImageURL, const std::string& userPhoneNumber, const std::string& userEmail, const std::string& userGUID);

            /**
             * This contructor should be used when you want want to initialize a user with
             * their respective items later by calling the setters.
             *
             * @brief Constructs a new `GroupMe::User` object
             *
             */
            User();

            /**
             * @brief Gets the users ID
             *
             */
            std::string getID() const;

            /**
             * @brief Sets the users ID
             *
             * @param userID The new ID to set
             *
             */
            void setID(const std::string& userID);

            /**
             * @brief Gets the users nickname
             *
             */
            std::string getNickname() const;

            /**
             * @brief Sets the users nickname
             *
             * @param userNickname The new nickname to set
             *
             */
            void setNickname(const std::string& userNickname);

            /**
             * @brief Gets the users profile image URL
             *
             */
            std::string getProfileImageURL() const;

            /**
             * @brief Sets the users profile image URL
             *
             * @param userProfileImageURL The new URL to set
             *
             */
            void setProfileImageURL(const std::string& userProfileImageURL);

            /**
             * @brief Gets the users phone number
             *
             */
            std::string getPhoneNumber() const;

            /**
             * @brief Sets the users phone number
             *
             * @param userPhoneNumber The new phone number to set
             *
             */
            void setPhoneNumber(const std::string& userPhoneNumber);

            /**
             * @brief Gets the users email
             *
             */
            std::string getEmail() const;

            /**
             * @brief Sets the users email
             *
             * @param userEmail The new email to set
             *
             */
            void setEmail(const std::string& userEmail);

            /**
             * @brief Gets the users GUID
             *
             */
            std::string getGUID() const;

            /**
             * @brief Sets the users GUID
             *
             * @param userGUID The new GUID to set
             *
             */
            void setGUID(const std::string& userGUID);

            /**
             * @brief Gets the users locale
             *
             */
            std::string getLocale() const;

            /**
             * @brief Sets the users locale
             *
             * @param locale The new locale to set
             *
             */
            void setLocal(const std::string& locale);

            /**
             * @brief Gets the users zipcode
             *
             */
            std::string getZipcode() const;

            /**
             * @brief Sets the users zipcode
             *
             * @param zipcode The new zipcode to set
             *
             */
            void setZipcode(const std::string& zipcode);

            /**
             * The share URL can be shared with other people to get their info.
             *
             * @brief Gets the users share URL.
             *
             */
            std::string getShareURL() const;

            /**
             * @brief Sets the users share URL
             *
             * @param shareURL The new share URL to set
             *
             */
            void setShareURL(const std::string& shareURL);

            /**
             * @brief Gets the users QR Code share URL
             *
             */
            std::string getShareQRCodeURL() const;

            /**
             * @brief Sets the users QR Code share URL
             *
             * @param shareQRCodeURL The new QR Code share URL to set
             *
             */
            void setShareQRCodeURL(const std::string& shareQRCodeURL);

            /**
             * @brief Gets the time the account was created at
             *
             */
            unsigned int getCreatedAt() const;

            /**
             * @brief Sets the time the account was created at
             *
             * @param createdAt The new time to set
             *
             */
            void setCreatedAt(unsigned int createdAt);

            /**
             * @brief Gets the time the account was last updated at
             *
             */
            unsigned int getUpdatedAt() const;

            /**
             * @brief Sets the time the account was last updated at
             *
             * @param updatedAt The new time to set
             *
             */
            void setUpdatedAt(unsigned int updatedAt);

            /**
             * @brief Returns whether or not the user is set up to use the SMS feature
             *
             */
            bool usingSMS() const;

            /**
             * @brief Sets whether or not the user is set up to use the SMS feature
             *
             * @param usingSMS The new value to set
             *
             */
            void setUsingSMS(bool usingSMS);

            /**
             * @brief Returns whether or not Facebook is connected to the users account
             *
             */
            bool getFacebookConnected() const;

            /**
             * @brief Sets whether or not Facebook is connected to the users account
             *
             * @param facebookConnected The new value to set
             *
             */
            void setFacebookConnected(bool facebookConnected);

            /**
             * @brief Returns whether or not Twitter is connected to the users account
             *
             */
            bool getTwitterConnected() const;

            /**
             * @brief Sets whether or not Twitter is connected to the users account
             *
             * @param twitterConnected The new value to set
             *
             */
            void setTwitterConnected(bool twitterConnected);

            bool operator==(const User& user) const;

            bool operator!=(const User& user) const;

        protected:
            
            /**
             * @brief The users ID
             *
             */
            std::string m_userID;

            /**
             * @brief The users name/nickname
             *
             */
            std::string m_userNickname;

            /**
             * @brief The users profile image URL
             *
             */
            std::string m_userProfileImageURL;

            /**
             * @brief The users phone number
             *
             */
            std::string m_userPhoneNumber;

            /**
             * @brief The users email
             *
             */
            std::string m_userEmail;

            /**
             * @brief The users GUID
             *
             */
            std::string m_userGUID;

            /**
             * @brief The users locale
             *
             */
            std::string m_locale;

            /**
             * @brief The users zipcode
             *
             */
            std::string m_zipcode;

            /**
             * @brief The users share URL
             *
             */
            std::string m_shareURL;

            /**
             * @brief The users QR Code share URL
             *
             */
            std::string m_shareQRCodeURL;

            /**
             * @brief The time the account was created at
             *
             */
            unsigned int m_createdAt;

            /**
             * @brief The time the account was last updated at
             *
             */
            unsigned int m_updatedAt;

            /**
             * @brief Is the user using the SMS feature?
             *
             */
            bool m_isSMS;

            /**
             * @brief Is the users Facebook account connected?
             *
             */
            bool m_isFacebookConnected;

            /**
             * @brief Is the users Twitter account connected?
             *
             */
            bool m_isTwitterConnected;
    };
}
