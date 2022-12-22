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
    Attachment(path, Attachment::Types::Video, accessToken),
    m_request(),
    m_client("https://video.groupme.com/transcode"),
    m_parser(),
    m_conversationID(conversationID),
    m_task(),
    m_json()
{
    m_task = pplx::task<void>([this]() {
        // avformat is used to grab the duration of
        // the video to make sure we don't upload a
        // video that is too long. Max is 1 minute
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

Video::Video(const std::string& accessToken, const std::vector<unsigned char>& contentVector, const std::string& conversationID) :
    Attachment(contentVector, Attachment::Types::Video, accessToken),
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
    Attachment(contentURL, Attachment::Types::Video, accessToken),
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
    m_task.wait();
}

pplx::task<std::string> Video::upload() {
    m_task.wait();

    m_request.set_body(m_parser.GenBodyContent());

    return pplx::task<std::string>([this]() -> std::string {

        m_json = nlohmann::json::parse(m_client.request(m_request).get().extract_string(true).get());

        m_content = m_json["status_url"].dump();

        m_content.erase(std::remove(m_content.begin(), m_content.end(), '"'), m_content.end());

        m_client = web::http::client::http_client(m_content);

        m_request.set_method(web::http::methods::GET);

        m_request.set_body("");

        bool done = false;

        // The video upload request if done correctly give us
        // a status url for an upload so we keep checking that
        // until the url says that it is finished.
        while (!done) {
            m_client.request(m_request).then([&done, this](web::http::http_response response) {
                if (response.status_code() == web::http::status_codes::Created) {

                    std::string content;

                    m_json.clear();

                    m_json = nlohmann::json::parse(response.extract_string(true).get());

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
