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

#include "base/Video.h"

using namespace GroupMe;

Video::Video(std::string accessToken, std::filesystem::path path) :
    Attatchment(path, Attatchment::Types::Video),
    m_request(),
    m_client("https://video.groupme.com/transcode"),
    m_header(),
    m_json(),
    m_parser()
{
    m_request.set_method(web::http::methods::POST);
    m_request.headers().add("X-Access-Token", accessToken);
    m_request.headers().add("X-Conversation-Id","***REMOVED***");
    m_parser.AddFile(m_contentPath);
    m_request.headers().add("Content-Type", "multipart/form-data; boundary=" + m_parser.boundary());
}

Video::Video(std::string accessToken, std::vector<unsigned char> contentVector) :
    Attatchment(contentVector, Attatchment::Types::Video),
    m_request(),
    m_client("https://video.groupme.com/transcode"),
    m_header(),
    m_json(),
    m_parser()
{
    m_request.set_method(web::http::methods::POST);
    m_request.headers().add("X-Access-Token", accessToken);
    m_request.headers().add("Content-Type", "multipart/form-data;boundary=");
}

Video::Video(std::string accessToken, web::uri contentURL) :
    Attatchment(contentURL, Attatchment::Types::Video),
    m_request(),
    m_client("https://video.groupme.com/transcode"),
    m_header(),
    m_json(),
    m_parser()
{
    m_contentURL = contentURL;
    
    m_request.set_method(web::http::methods::POST);
    m_request.headers().add("X-Access-Token", accessToken);
    m_request.headers().add("Content-Type", "multipart/form-data;boundary=");
}

bool Video::upload() {
    m_request.set_body(m_parser.GenBodyContent());
    std::cout << m_parser.body_content();
    bool did = true;
    m_client.request(m_request).then([this, &did](web::http::http_response response) {
        std::printf("%u, %s\n", response.status_code(), response.to_string().c_str());
        if (response.status_code() != 200 || response.status_code() != 201) {
            did = false;
            return;
        }
        std::stringstream strm(response.extract_string(true).get());
        strm >> m_json;

        std::string tmp = m_json["payload"]["url"].dump(1);
        tmp.erase(tmp.end() - 1);
        tmp.erase(tmp.begin());
        
        m_contentURL = tmp;
        return;
    }).wait();
    return did;
}
