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

#include "base/Picture.h"

using namespace GroupMe;

Picture::Picture(std::string accessToken, std::filesystem::path path) :
    Attatchment(path, Attatchment::Types::Picture, accessToken),
    m_request(),
    m_client("https://image.groupme.com/pictures"),
    m_json(),
    m_file(path),
    m_task()
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

Picture::Picture(std::string accessToken, web::uri contentURL) :
    Attatchment(contentURL, Attatchment::Types::Picture, accessToken),
    m_request(),
    m_client(m_content),
    m_json(),
    m_file(),
    m_task()
{
    m_task = pplx::task<void>([this]() -> void {
        m_request.set_method(web::http::methods::GET);

        m_client.request(m_request).then([this](web::http::http_response response) {
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

pplx::task<std::string> Picture::upload() {
    while (!m_task.is_done()) {

    }

    return pplx::task<std::string>([this]() -> std::string {
        m_request.set_body(m_contentBinary);
        m_client.request(m_request).then([this](web::http::http_response response) -> void {
                if (response.status_code() != 200) {

                    return;
                }

                std::stringstream strm(response.extract_string(true).get());

                strm >> m_json;

                std::string tmp = m_json["payload"]["picture_url"].dump(1);

                tmp.erase(tmp.cend() - 1);
                tmp.erase(tmp.cbegin());

                m_content = tmp;

        }).wait();
        return m_content;
    });
}
