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

#include "Picture.h"

using namespace GroupMe;

Picture::Picture(const std::string& accessToken, const std::filesystem::path& path) :
    Attachment(path, Attachment::Types::Picture, accessToken),
    m_client("https://image.groupme.com/pictures"),
    m_file(path)
{
    m_task = pplx::task<void>([this]() -> void {
        m_request.set_method(web::http::methods::POST);
        m_request.headers().add("X-Access-Token", m_accessToken);
        m_request.headers().add("Content-Type", "image/jpeg");

        if (!m_file) {
            throw std::fstream::failure("Failed to open file.");
        }

        m_contentBinary.assign(std::istreambuf_iterator<char>(m_file), std::istreambuf_iterator<char>());

    });
}

Picture::Picture(const std::string& accessToken, const web::uri& contentURL) :
    Attachment(contentURL, Attachment::Types::Picture, accessToken),
    m_client(m_content)
{
    m_task = pplx::task<void>([this]() -> void {
        m_request.set_method(web::http::methods::GET);

        m_client.request(m_request).then([this](const web::http::http_response& response) {
            std::stringstream strm;

            strm << response.extract_string(true).get();

            m_contentBinary.assign(std::istreambuf_iterator<char>(strm), std::istreambuf_iterator<char>());
        }).wait();

        m_client = web::http::client::http_client("https://image.groupme.com/pictures");

        m_request.set_method(web::http::methods::POST);
        m_request.headers().add("X-Access-Token", m_accessToken);
        m_request.headers().add("Content-Type", "image/jpeg");
        m_request.set_body("");
    });
}

Picture::Picture(const web::uri &contentURL) :
    Attachment(contentURL, Attachment::Types::Picture),
    m_client(m_content),
    m_task(pplx::task<void>([]() -> void {}))
{
    // TODO: Check if subdomain is equal
    // to i (e.g. https://*i*.groupme.com)
    m_content = contentURL.to_string();
}

Picture::Picture(const Picture& other) :
    Attachment(other),
    m_request(other.m_request),
    m_client(other.m_client),
    m_json(other.m_json),
    m_task(other.m_task)
{

}

Picture::~Picture() {
    m_task.wait();
}

Picture& Picture::operator=(const Picture& other) {
    if(this != &other) {
        Attachment::operator=(other);
        m_request = other.m_request;
        m_client = other.m_client;
        m_json = other.m_json;
        m_task = other.m_task;
    }
    return *this;
}

pplx::task<std::string> Picture::upload() {
    m_task.wait();

    // Prevents from re-upload
    if (!m_content.empty()) {
        return pplx::task<std::string>([m_content = m_content]() -> std::string {return m_content;});
    }

    return pplx::task<std::string>([this]() -> std::string {
        m_request.set_body(m_contentBinary);
        m_client.request(m_request).then([this](const web::http::http_response& response) -> void {
                if (response.status_code() != web::http::status_codes::OK) {
                    return;
                }

                m_json = nlohmann::json::parse(response.extract_string(true).get());

                m_content = m_json["payload"]["picture_url"].dump();

                m_content.erase(std::remove(m_content.begin(), m_content.end(), '"'), m_content.end());

        }).wait();
        return m_content;
    });
}

nlohmann::json Picture::toJson() const {
    if (m_content.empty()) {
        return {};
    }

    nlohmann::json json;
    
    json["type"] = "image";
    json["url"] = m_content;

    return json;
}
