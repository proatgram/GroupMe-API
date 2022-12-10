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

#include <cpprest/http_client.h>
#include <cpprest/http_headers.h>
#include <cpprest/uri.h>
#include <nlohmann/json.hpp>

#include "base/User.h"

namespace GroupMe {
    class Self : public User {
        public:
            Self(std::string accessToken);

            ~Self();

            pplx::task<web::http::status_code> push();

            pplx::task<web::http::status_code> pull();

            std::string getNickname() const;

            void setNickname(const std::string& userNickname);

            std::string getProfileImageURL() const;

            void setProfileImageURL(const std::string& userProfileImageURL);

            std::string getPhoneNumber() const;

            void setPhoneNumber(const std::string& userPhoneNumber);

            std::string getEmail() const;

            void setEmail(const std::string& userEmail);

            std::string getZipcode() const;

            void setZipcode(const std::string& zipcode);

            bool usingSMS() const;

            void setUsingSMS(bool usingSMS);

            bool getFacebookConnected() const;

            void setFacebookConnected(bool facebookConnected);

            bool getTwitterConnected() const;

            void setTwitterConnected(bool twitterConnected);

        private:
            std::string m_accessToken;

            web::http::http_request m_request;

            web::http::client::http_client m_client;

            pplx::task<void> m_task;

            mutable std::mutex m_mutex;
    };
}
