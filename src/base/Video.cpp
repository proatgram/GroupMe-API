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
    m_binaryData(),
    m_boundaries("------CpPrEsTaPi_-_LiBgRoUpMeApIcPp_-_DAd------")
{
    m_request.set_method(web::http::methods::POST);
    m_request.headers().add("X-Access-Token", accessToken);
    m_request.headers().add("X-Conversation-Id","85177831");
    m_request.headers().add("Content-Type", "multipart/form-data; boundary=" + m_boundaries);
    std::fstream file(m_contentPath.string(), std::ios::in | std::ios::binary);

    if (!file) {
        throw std::fstream::failure("Failed to open file.");
    }
    file.seekg(0, std::ios::end);
    int size = file.tellg();
    file.seekg(0);
    for (unsigned int i = 0; i < m_boundaries.size(); i++) {
        m_binaryData.push_back(m_boundaries.data()[i]);
    }

    for (unsigned int i = 0; i < size; i++) {
        m_binaryData.push_back(file.get());
    }

    for (unsigned int i = 0; i < m_boundaries.size(); i++) {
        m_binaryData.push_back(m_boundaries.data()[i]);
    }
    for (unsigned int i = 0; i < m_binaryData.size(); i++) {
        std::cout << m_binaryData.at(i);
    }
    std::cout.flush();
}

Video::Video(std::string accessToken, std::vector<unsigned char> contentVector) :
    Attatchment(contentVector, Attatchment::Types::Video),
    m_request(),
    m_client("https://video.groupme.com/transcode"),
    m_header(),
    m_json(),
    m_binaryData(contentVector),
    m_boundaries("------CpPrEsTaPi_-_LiBgRoUpMeApIcPp_-_123123asDasDAd------")
{
    m_request.set_method(web::http::methods::POST);
    m_request.headers().add("X-Access-Token", accessToken);
    m_request.headers().add("Content-Type", "multipart/form-data;boundary=" + m_boundaries);
}

Video::Video(std::string accessToken, web::uri contentURL) :
    Attatchment(contentURL, Attatchment::Types::Video),
    m_request(),
    m_client("https://video.groupme.com/transcode"),
    m_header(),
    m_json(),
    m_binaryData(),
    m_boundaries("------CpPrEsTaPi_-_LiBgRoUpMeApIcPp_-_123123asDasDAd------")
{
    m_contentURL = contentURL;
    
    m_request.set_method(web::http::methods::POST);
    m_request.headers().add("X-Access-Token", accessToken);
    m_request.headers().add("Content-Type", "multipart/form-data;boundary=" + m_boundaries);
}

bool Video::upload() {
    m_request.set_body(m_contentBinary);
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
