/*
 *  This is a library used to communicate with the GroupMe API efficiently and seamlessly.
 *  Copyright (C) 2022 Timothy Hutchins
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include "File.h"

using namespace GroupMe;

// This function just creates a url from two strings. Saves me a lot of pain.
std::string File::getURL(const std::string &conversationID, const std::string &filename) {
    std::string url = "https://file.groupme.com/v1/[GROUP_ID]/files?name=[FILE_NAME]";
    url.replace(url.find("[GROUP_ID]"), 10, conversationID);
    url.replace(url.find("[FILE_NAME]"), 11, filename);
    return url;
}

File::File(const std::string &accessToken, const std::filesystem::path &path, const std::string &conversationID) :
    Attachment(path, Attachment::Types::File, accessToken),
    m_conversationID(conversationID),
    m_client(getURL(m_conversationID, path.filename().string()))
{
    if (!std::filesystem::exists(path)) {
        throw std::filesystem::filesystem_error("File does not exist", std::make_error_code(std::errc::no_such_file_or_directory));
    }

    m_content = m_client.base_uri().to_string();

    m_task = pplx::task<void>([this, path]() -> void {
        std::fstream file(path, std::ios::in | std::ios::out | std::ios::binary);

        m_contentBinary.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

        m_request.set_method(web::http::methods::POST);
        m_request.headers().add("X-Access-Token", m_accessToken);
        m_request.headers().add("Content-Type", "application/json");
        m_request.headers().add("Accept-Encoding", "gzip, deflate");
        m_request.headers().add("Connection", "close");

    });
}

File::File(const std::string &accessToken, const std::vector<unsigned char> &contentVector, const std::string &conversationID) :
    Attachment(contentVector, Attachment::Types::File, accessToken),
    m_conversationID(conversationID),
    m_client(getURL(m_conversationID, "file"))
{
    m_contentBinary = contentVector;

    m_task = pplx::task<void>([this]() -> void {

        m_request.set_method(web::http::methods::POST);
        m_request.headers().add("X-Access-Token", m_accessToken);
        m_request.headers().add("Content-Type", "application/json");
        m_request.headers().add("Accept-Encoding", "gzip, deflate");
        m_request.headers().add("Connection", "close");

    });
}

File::File(const std::string &accessToken, const web::uri &contentURL, const std::string &conversationID) :
    Attachment(contentURL, Attachment::Types::File, accessToken),
    m_conversationID(conversationID),
    m_client(contentURL)
{
    m_task = pplx::task<void>([this, contentURL]() -> void {
        m_request.set_method(web::http::methods::GET);

        m_client.request(m_request).then([this](const web::http::http_response &response) {
            std::stringstream strm;

            strm << response.extract_string(true).get();

            m_contentBinary.assign(std::istreambuf_iterator<char>(strm), std::istreambuf_iterator<char>());
        }).wait();
        m_client = web::http::client::http_client(getURL(m_conversationID, web::uri::split_path(contentURL.path()).at(web::uri::split_path(contentURL.path()).size() - 1)));
        m_request.set_body("");

        m_request.set_method(web::http::methods::POST);
        m_request.headers().add("X-Access-Token", m_accessToken);
        m_request.headers().add("Content-Type", "application/json");
        m_request.headers().add("Accept-Encoding", "gzip, deflate");
        m_request.headers().add("Connection", "close");
        m_request.set_body("");
    });
}

File::File(const std::string &fileId) :
    Attachment(fileId, Attachment::Types::File),
    m_client(m_content),
    m_task(pplx::task<void>([]() -> void {}))
{

}

File::~File() {
    m_task.wait();
}

File::File(const File &other) :
    Attachment(other.m_content, other.m_type),
    m_request(other.m_request),
    m_client(other.m_client),
    m_json(other.m_json),
    m_task(other.m_task)
{

}

File& File::operator=(const File& other) {
    if(this != &other) {
        Attachment::operator=(other);
        m_request = other.m_request;
        m_client = other.m_client;
        m_json = other.m_json;
        m_task = other.m_task;
    }

    return *this;
}

pplx::task<std::string> File::upload() {
    m_task.wait();

    // Prevents from re-upload
    if (!m_content.empty()) {
        return pplx::task<std::string>([m_content = m_content]() -> std::string {return m_content;});
    }

    m_request.set_body(m_contentBinary);

    return pplx::task<std::string>([this]() -> std::string{

        m_json = nlohmann::json::parse(m_client.request(m_request).get().extract_string(true).get());

        m_content = m_json["status_url"].dump();

        m_content.erase(std::remove(m_content.begin(), m_content.end(), '"'), m_content.end());

        m_client = web::http::client::http_client(m_content);

        m_request.set_method(web::http::methods::GET);

        m_request.set_body("");

        bool done = false;

        while(!done) {
            m_client.request(m_request).then([&done, this](const web::http::http_response &response) {
                if (response.status_code() == web::http::status_codes::OK) {

                    std::string content;

                    m_json.clear();

                    m_json = nlohmann::json::parse(response.extract_string(true).get());

                    content = m_json["file_id"].dump();

                    content.erase(std::remove(content.begin(), content.end(), '"'), content.end());

                    m_content = content;

                    done = true;
                }
                else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));
                }
            }).wait();
        }
        return m_content;
    });
}

nlohmann::json File::toJson() const {
    if (m_content.empty()) {
        return {};
    }
    
    nlohmann::json json;

    json["type"] = "file";
    json["file_id"] = m_content;

    return json;
}
