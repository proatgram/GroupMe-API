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
    m_task(),
    m_contacts()
{
    m_task = pplx::task<void>([this]() -> void {
        m_request.set_method(web::http::methods::GET);

        //
        m_request.headers().add("X-Access-Token", m_accessToken);

        m_client.request(m_request).then([this](web::http::http_response response) {
            if (response.status_code() != web::http::status_codes::OK) {
                return;
            }

            auto json = nlohmann::json::parse(response.extract_string(true).get());

            if (json["response"] == "null") {
                return;
            }

            // This just grabs the responses from the json object and parses it
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

            m_isSMS = json["response"]["sms"];

            m_locale = json["response"]["locale"];
            m_locale.erase(std::remove(m_locale.begin(), m_locale.end(), '\"'), m_locale.end());

            m_shareURL = json["response"]["share_url"];
            m_shareURL.erase(std::remove(m_locale.begin(), m_locale.end(), '\"'), m_locale.end());

            m_shareQRCodeURL = json["response"]["share_qr_code_url"];
            m_shareQRCodeURL.erase(std::remove(m_shareQRCodeURL.begin(), m_shareQRCodeURL.end(), '\"'), m_shareQRCodeURL.end());


        }).wait();
    });
}

Self::~Self() {
    // Waits for the task to finish so that we don't have the possibility
    // of the main thread finishing before the task
    m_task.wait();
}

pplx::task<web::http::status_code> Self::push() {
    // Waits for the task to be complete just in case there are tasks happening
    // that need to finish before we push
    m_task.wait();

    return pplx::task<web::http::status_code>([this]() -> web::http::status_code {
        nlohmann::json json;

        std::lock_guard<std::mutex> lock(m_mutex);

        // Adds stuff to the JSON object to push to the API
        json["email"] = m_userEmail;
        json["facebook_connected"] = m_isFacebookConnected;
        json["image_url"] = m_userProfileImageURL;
        json["locale"] = m_locale;
        json["name"] = m_userNickname;
        json["phone_number"] = m_userPhoneNumber;
        json["sms"] = m_isSMS;
        json["twitter_connected"] = m_isTwitterConnected;
        json["zip_code"] = m_zipcode;

        m_request.set_method(web::http::methods::POST);

        m_request.set_body(json.dump());

        // API endpoint
        m_client = web::http::client::http_client("https://api.groupme.com/v3/users/update");

        web::http::status_code statusCode;

        m_client.request(m_request).then([&statusCode](web::http::http_response response) {
            statusCode = response.status_code();
            return;
        }).wait();
        return statusCode;
    });
}

pplx::task<web::http::status_code> Self::pull() {
    // Waits for the task to be complete just in case there are tasks happening
    // that need to finish before we pull
    m_task.wait();

    return pplx::task<web::http::status_code>([this]() -> web::http::status_code {
        m_request.set_method(web::http::methods::GET);

        // Resets the body of the request just incase theres
        // old data in it.
        m_request.set_body("");

        web::http::status_code statusCode;

        // Again, the API endpoint
        m_client = web::http::client::http_client("https://api.groupme.com/v3/users/me");

        m_client.request(m_request).then([&statusCode, this](web::http::http_response response) {

            statusCode = response.status_code();

            if (statusCode != web::http::status_codes::OK) {
                return;
            }

            auto json = nlohmann::json::parse(response.extract_string(true).get());

            // The response section of the JSON will be null
            // if a problem occured
            if (json["response"] == "null") {
                //TODO Add checking to the errors section of the JSON if the resonse is null
                return;
            }

            std::lock_guard<std::mutex> lock(m_mutex);

            // This just grabs the responses from the json object and parses it
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

            m_isSMS = json["response"]["sms"];

            m_locale = json["response"]["locale"];
            m_locale.erase(std::remove(m_locale.begin(), m_locale.end(), '\"'), m_locale.end());

            m_shareURL = json["response"]["share_url"];
            m_shareURL.erase(std::remove(m_locale.begin(), m_locale.end(), '\"'), m_locale.end());

            m_shareQRCodeURL = json["response"]["share_qr_code_url"];
            m_shareQRCodeURL.erase(std::remove(m_shareQRCodeURL.begin(), m_shareQRCodeURL.end(), '\"'), m_shareQRCodeURL.end());

        }).wait();
        return statusCode;
    });
}
/*
 * Overridden functions to prevent data race senarios
 * Functions must be const for User::operator== and User::operator!=
 * But also must be thread safe. It would be inifeceint to have this in
 * the `User` class because this is the only time stuff is being accessed
 * in another thread
 */

std::string Self::getNickname() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_userNickname;
}

void Self::setNickname(const std::string& userNickname) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_userNickname = userNickname;
}

std::string Self::getProfileImageURL() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_userProfileImageURL;
}

void Self::setProfileImageURL(const std::string& userProfileImageURL) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_userProfileImageURL = userProfileImageURL;
}

std::string Self::getPhoneNumber() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_userPhoneNumber;
}

void Self::setPhoneNumber(const std::string& userPhoneNumber) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_userPhoneNumber = userPhoneNumber;
}

std::string Self::getEmail() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_userEmail;
}

void Self::setEmail(const std::string& userEmail) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_userEmail = userEmail;
}

std::string Self::getZipcode() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_zipcode;
}

void Self::setZipcode(const std::string& zipcode) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_zipcode = zipcode;
}

bool Self::usingSMS() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_isSMS;
}

void Self::setUsingSMS(bool usingSMS) {
    //TODO There should be stuff here to create SMS mode possibly.
    std::lock_guard<std::mutex> lock(m_mutex);
    m_isSMS = usingSMS;
}

bool Self::getFacebookConnected() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_isFacebookConnected;
}

void Self::setFacebookConnected(bool facebookConnected) {
    //TODO There should be stuff here to add Facebook to the users account
    std::lock_guard<std::mutex> lock(m_mutex);
    m_isFacebookConnected = facebookConnected;
}

bool Self::getTwitterConnected() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_isTwitterConnected;
}

void Self::setTwitterConnected(bool twitterConnected) {
    //TODO There should be stuff here to add Twitter to the users account
    std::lock_guard<std::mutex> lock(m_mutex);
    m_isTwitterConnected = twitterConnected;
}
