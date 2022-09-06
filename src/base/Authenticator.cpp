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

#include "base/Authenticator.h"
#include <cpprest/http_msg.h>
#include <mutex>
#include <condition_variable>

using namespace GroupMe;

Authenticator::Authenticator(std::string callbackURL, std::string appURL) :
    m_callback(callbackURL),
    m_app(appURL),
    m_token("NULL"),
    m_listener(m_callback),
    m_ready(false),
    m_error(false),
    m_mutex()
{
    m_listener.support(web::http::methods::GET, [this](web::http::http_request request){handleGET(request);});
}

std::string Authenticator::getToken() {
    std::printf("Please visit: %s to sign in and authenticate.\n", m_app.c_str());
    try {
        m_listener.open().then([this]() {std::printf("Listening...\n");}).wait();
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this]{return m_ready;});
        printf("Done...\n");
    }
    catch (std::exception const & exception) {
        std::printf("%s\n", exception.what());
    }
    return m_token;
}

void Authenticator::handleGET(web::http::http_request request) {
    try {
        m_token = web::uri::split_query(request.request_uri().query()).at("access_token");
        m_error = false;
    }
    catch (std::exception const & exception) {
        std::printf("Query for 'access_token' was not found for the given request.\n");
        m_error = true;
    }
    printf("Token: %s\n", m_token.c_str());
    request.reply(web::http::status_codes::OK);
    if (!m_error) {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_ready = true;
        }
        m_cv.notify_one();
    }
}
