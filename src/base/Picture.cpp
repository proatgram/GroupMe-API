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
    Attatchment(path, Attatchment::Types::Picture),
    m_request(),
    m_client("https://image.groupme.com/pictures"),
    m_header()
{
    m_request.set_method(web::http::methods::POST);
    m_request.headers().add("X-Access-Token", accessToken);
    m_request.headers().add("Content-Type", "image/jpeg");

    std::fstream file(m_contentPath, std::ios::in | std::ios::binary);
    if (!file) {
        throw std::fstream::failure("Failed to open file.");
    }
    file.seekg(0, std::ios::end);
    int size = file.tellg();
    file.seekg(0);
    
    for (unsigned int i = 0; i < size; i++) {
        m_binaryData.push_back(file.get());
    }
    
    m_request.set_body(m_binaryData);
    
    m_client.request(m_request).then([this](web::http::http_response response) {
            std::stringstream strm(response.extract_string(true).get());
            strm >> m_json;
            m_contentURL = m_json["payload"]["picture_url"].dump(1);
            m_contentURL.erase(m_contentURL.cend() - 1);
            m_contentURL.erase(m_contentURL.cbegin());
        
    }).wait();
}

Picture::Picture(std::string accessToken, std::string contentURL) :
    Attatchment(contentURL, Attatchment::Types::Picture),
    m_client("https://image.groupme.com/pictures"),
    m_header()
{

}
