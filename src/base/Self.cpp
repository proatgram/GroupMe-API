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

#include "base/Self.h"

using namespace GroupMe;

Self::Self(std::string accessToken) :
    User(),
    m_accessToken(accessToken),
    m_request(),
    m_client("https://api.groupme.com/v3/users/me"),
    m_task()
{
    m_task = pplx::task<void>([this]() -> void {
        m_request.set_method(web::http::methods::GET);

        m_request.headers().add("X-Access-Token", m_accessToken);
        m_client.request(m_request).then([this](web::http::http_response response) {
            if (response.status_code() != web::http::status_codes::OK) {
                std::cout << "Non OK code" << std::endl;
                std::cout << response.to_string();
                return;
            }

            auto json = nlohmann::json::parse(response.extract_string(true).get());

            if (json["response"] == "null") {
                std::cout << "Response is 'null'" << std::endl;
                return;
            }

            m_userID = json["response"]["id"];
            m_userID.erase(std::remove(m_userID.begin(), m_userID.end(), '\"'), m_userID.end());

            m_userPhoneNumber = json["response"]["phone_number"];
            m_userPhoneNumber.erase(std::remove(m_userPhoneNumber.begin(), m_userPhoneNumber.end(), '\"'), m_userPhoneNumber.end());

            m_userProfileImageURL = json["response"]["image_url"];
            m_userProfileImageURL.erase(std::remove(m_userProfileImageURL.begin(), m_userProfileImageURL.end(), '\"'), m_userProfileImageURL.end());

            m_createdAt = json["response"]["created_at"];

            m_updatedAt = json["response"]["updated_at"];

            m_userEmail = json["response"]["email"];
            m_userEmail.erase(std::remove(m_userEmail.begin(), m_userEmail.end(), '\"'), m_userEmail.end());

            m_isSMS = (json["response"]["sms"] == "\"true\"" ? true : false);

        }).wait();
    });
    m_task.wait();
}
