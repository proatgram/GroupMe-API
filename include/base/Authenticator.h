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
#include <cpprest/http_listener.h>
#include <cpprest/json.h>

namespace GroupMe {
    class Authenticator {
        public:
            Authenticator(std::string callbackURL, std::string appURL);
            
            std::string getToken();

        private:
            
            void handleGET(web::http::http_request request);

            std::string m_callback;

            std::string m_app;

            std::string m_token;

            web::http::experimental::listener::http_listener m_listener;

            bool m_ready;

            bool m_error;

            std::mutex m_mutex;
            
            std::condition_variable m_cv;
    };
}
