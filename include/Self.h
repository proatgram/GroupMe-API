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
#include <algorithm>
#include <mutex>
#include <utility>

#include <cpprest/http_client.h>
#include <cpprest/http_headers.h>
#include <cpprest/uri.h>
#include <nlohmann/json.hpp>

#include "User.h"
#include "UserSet.hpp"

namespace GroupMe {
    /**
     * This class holds user data for the current user, such as the nickname,
     * access token, phone number, etc. like the  `GroupMe::User` class, but has the ability to upload and download
     * this data to the server using the API
     *
     * @brief Class used to represent the current authenticated user
     *
     */
    class Self : public User {
        public:

            /**
             * @brief Constructs a new `GroupMe::Self` object
             *
             * @param accessToken The users access token
             *
             */
            explicit Self(const std::string& accessToken);

            Self(const Self& other) = delete;

            Self(Self&& other) = delete;

            /**
             * Makes sure to wait for all of the concurrent tasks to finish
             * before moving on.
             *
             * @brief The destructor
             *
             */
            ~Self();

            Self& operator=(const Self&) = delete;

            Self& operator=(Self&&) = delete;

            /**
             * @brief Pushes all of the user data to the server
             *
             * @return pplx::task<web::http::status_code>
             *
             */
            pplx::task<web::http::status_code> push();

            /**
             * @brief Pulls user data from the server
             *
             * @return pplx::task<web::http::status_code>
             *
             */
            pplx::task<web::http::status_code> pull();

            /**
             * @brief Gets the nickname of the authenticated user
             *
             * @return std::string
             *
             */
            std::string getNickname() const;

            /**
             * @brief Sets the nickname of the authenticated user
             *
             * @param userNickname The new nickanme to set
             *
             */
            void setNickname(const std::string& userNickname);

            /**
             * @brief Gets the profile image URL of the authenticated user
             *
             * @return std::string
             *
             */
            std::string getProfileImageURL() const;

            /**
             * See the `GroupMe::Picture` class to upload an image to the server.
             *
             * You can also input any URL that points to an image.
             *
             * @brief Sets the profile image URL of the authenticated user.
             *
             * @param userProfileImageURL The new URL to set
             *
             */
            void setProfileImageURL(const std::string& userProfileImageURL);

            /**
             * @brief Gets the phone number of the authenticated user
             *
             * @return std::string
             *
             */
            std::string getPhoneNumber() const;

            /**
             * @brief Sets the phone number of the authenticated user
             *
             * @param userPhoneNumber The new phone number to set
             *
             */
            void setPhoneNumber(const std::string& userPhoneNumber);

            /**
             * @brief Gets the email of the authenticated user
             *
             * @return std::string
             *
             */
            std::string getEmail() const;

            /**
             * @brief Sets the email of the authenticated user
             *
             * @param userEmail The new email to set
             *
             */
            void setEmail(const std::string& userEmail);

            /**
             * @brief Gets the zipcode of the authenticated user
             *
             * @return std::string
             *
             */
            std::string getZipcode() const;

            /**
             * @brief Sets the zipcode of the authenticated user
             *
             * @param zipcode The new zipcode to set
             *
             */
            void setZipcode(const std::string& zipcode);

            /**
             * @brief Returns whether or not the user is set up to use the SMS feature
             *
             * @return std::string
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
             * @brief Gets whether or not Facebook is connected to the users account
             *
             * @return bool
             *
             */
            bool getFacebookConnected() const;

            /**
             * @brief brief_description
             *
             * @param facebookConnected The new value to set
             *
             */
            void setFacebookConnected(bool facebookConnected);

            /**
             * @brief Gets whether or not the user has their Twitter connected
             *
             * @return bool
             *
             */
            bool getTwitterConnected() const;

            /**
             * @brief Sets whether or not the user has their Twitter connected
             *
             * @param twitterConnected The new value to set
             *
             */
            void setTwitterConnected(bool twitterConnected);

            /**
             * @brief Adds a contact to the contacts contacts set
             *
             * @param contact The new contact to add as a `std::shared_ptr<GroupMe::User>`
             *
             * @return std::pair<GroupMe::UserSet::iterator, bool> This is the same return value as the single insertion for std::set
             *
             */
            std::pair<GroupMe::UserSet::const_iterator, bool> addContact(const std::shared_ptr<GroupMe::User>& contact);

            /**
             * @brief Finds a contact from the contacts set
             * 
             * @param userID The ID of the user to search for
             *
             * @return A const_iterator to the contact if found, or past-the-end iterator is returned
             *
             */
            GroupMe::UserSet::const_iterator findContact(const std::string& userID) const;

            /**
             * @brief Merges the two UserSets
             *
             * @param set The UserSet to merge
             *
             * @return void
             *
             */
            void mergeContacts(UserSet& set);
        private:
            std::string m_accessToken;

            web::http::http_request m_request;

            web::http::client::http_client m_client;

            pplx::task<void> m_task;

            GroupMe::UserSet m_contacts;

            // Needed because we have variables that are read and written
            // to in another thread
            mutable std::mutex m_mutex;
    };
}
