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

Video::Video(const std::string& accessToken, const std::filesystem::path& path, const std::string& conversationID) :
    Attatchment(path, Attatchment::Types::Video, accessToken),
    m_request(),
    m_client("https://video.groupme.com/transcode"),
    m_parser(),
    m_conversationID(conversationID),
    m_task(),
    m_json()
{
    m_task = pplx::task<void>([this]() {
        AVFormatContext* context = avformat_alloc_context();//m_avContext.get();
        avformat_open_input(&context, m_contentPath.c_str(), NULL, NULL);

        if (context->duration / static_cast<double>(AV_TIME_BASE) > 60.0) {
            throw LargeFile();
        }

        avformat_close_input(&context);

        m_request.set_method(web::http::methods::POST);
        m_request.headers().add("X-Access-Token", m_accessToken);
        m_request.headers().add("X-Conversation-Id", m_conversationID);

        m_parser.AddFile(m_contentPath);
        m_request.headers().add("Content-Type", "multipart/form-data; boundary=" + m_parser.boundary());
        return;
    });
}

Video::Video(const std::string& accessToken, const std::vector<uint8_t>& contentVector, const std::string& conversationID) :
    Attatchment(contentVector, Attatchment::Types::Video, accessToken),
    m_request(),
    m_client("https://video.groupme.com/transcode"),
    m_parser(),
    m_conversationID(conversationID),
    m_task(),
    m_json()
{
    m_task = pplx::task<void>([this, contentVector]() {
        Util::InMemoryAVFormat format;
        format.openMemory(contentVector);

        if (format.get()->duration / static_cast<double>(AV_TIME_BASE) > 60.0) {
            throw LargeFile();
        }

        format.closeMemory();

        m_request.set_method(web::http::methods::POST);
        m_request.headers().add("X-Access-Token", m_accessToken);
        m_request.headers().add("X-Conversation-Id", m_conversationID);

        m_parser.AddFile(contentVector, "file.mp4");
        m_request.headers().add("Content-Type", "multipart/form-data;boundary=" + m_parser.boundary());
    });
}

Video::Video(const std::string& accessToken, const web::uri& contentURL,const  std::string& conversationID) :
    Attatchment(contentURL, Attatchment::Types::Video, accessToken),
    m_request(),
    m_client(m_content),
    m_parser(),
    m_conversationID(conversationID),
    m_task(),
    m_json()
{
    m_content = contentURL.to_string();

    m_task = pplx::task<void>([this]() -> void {

        m_request.set_method(web::http::methods::GET);
        pplx::task<web::http::http_response> task = m_client.request(m_request);
        task.then([this](web::http::http_response response) {

            std::stringstream strm;

            strm << response.extract_string(true).get();

            m_contentBinary.assign(std::istreambuf_iterator<char>(strm), std::istreambuf_iterator<char>());

            m_parser.AddFile(m_contentBinary, "file.mp4");

        });
        m_client = web::http::client::http_client("https://video.groupme.com/transcode");

        m_request.set_method(web::http::methods::POST);
        m_request.headers().add("X-Access-Token", m_accessToken);
        m_request.headers().add("Content-Type", "multipart/form-data;boundary=" + m_parser.boundary());
        m_request.set_body("");
    });
}

Video::~Video() {
    if (!m_task.is_done()) {
        m_task.wait();
    }
}

pplx::task<std::string> Video::upload() {
    while(!m_task.is_done()) {

    }
    m_request.set_body(m_parser.GenBodyContent());

    return pplx::task<std::string>([this]() -> std::string {
        std::stringstream strm(m_client.request(m_request).get().extract_string(true).get());

        strm >> m_json;

        std::string tmp = m_json["status_url"].dump();

        tmp.erase(std::remove(tmp.begin(), tmp.end(), '"'), tmp.end());

        m_content = tmp;

        m_client = web::http::client::http_client(m_content);

        m_request.set_method(web::http::methods::GET);

        m_request.set_body("");

        bool done = false;

        while (!done) {
            m_client.request(m_request).then([&done, this](web::http::http_response response) {
                if (response.status_code() == web::http::status_codes::Created) {

                    std::string content;

                    m_json.clear();

                    std::stringstream strm(response.extract_string(true).get());

                    strm >> m_json;

                    content = m_json["url"].dump();

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
