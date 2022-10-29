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

Video::Video(std::string accessToken, std::filesystem::path path, std::string conversationID) :
    Attatchment(path, Attatchment::Types::Video),
    m_request(),
    m_client("https://video.groupme.com/transcode"),
    m_header(),
    m_json(),
    m_parser(),
    m_avContext(avformat_alloc_context()),
    m_conversationID(conversationID)
{
    m_task = pplx::task<void>([this, accessToken]() {
        AVFormatContext* context = m_avContext.get();
        avformat_open_input(&context, m_contentPath.c_str(), NULL, NULL);

        if (context->duration / static_cast<double>(AV_TIME_BASE) > 60.0) {
            throw LargeFile();
        }

        avformat_close_input(&context);

        m_request.set_method(web::http::methods::POST);
        m_request.headers().add("X-Access-Token", accessToken);
        m_request.headers().add("X-Conversation-Id", m_conversationID);

        m_parser.AddFile(m_contentPath);
        m_request.headers().add("Content-Type", "multipart/form-data; boundary=" + m_parser.boundary());
        return;
    });
}

Video::Video(std::string accessToken, std::vector<uint8_t>& contentVector, std::string conversationID) :
    Attatchment(contentVector, Attatchment::Types::Video),
    m_request(),
    m_client("https://video.groupme.com/transcode"),
    m_header(),
    m_json(),
    m_parser(),
    m_avContext(avformat_alloc_context()),
    m_conversationID(conversationID)
{
    m_task = pplx::task<void>([this, contentVector, accessToken]() {
        Util::InMemoryAVFormat format;
        format.openMemory(contentVector);
        m_avContext.reset();

        m_avContext = format.get();

        if (m_avContext.get()->duration / static_cast<double>(AV_TIME_BASE) > 60.0) {
            throw LargeFile();
        }

        format.closeMemory();

        m_request.set_method(web::http::methods::POST);
        m_request.headers().add("X-Access-Token", accessToken);
        m_request.headers().add("X-Conversation-Id", m_conversationID);

        m_parser.AddFile(contentVector, "file.mp4");
        m_request.headers().add("Content-Type", "multipart/form-data;boundary=" + m_parser.boundary());
    });
}

Video::Video(std::string accessToken, web::uri contentURL, std::string conversationID) :
    Attatchment(contentURL, Attatchment::Types::Video),
    m_request(),
    m_client("https://video.groupme.com/transcode"),
    m_header(),
    m_json(),
    m_parser(),
    m_avContext(avformat_alloc_context()),
    m_conversationID(conversationID),
    m_task()
{
    m_contentURL = contentURL;
    
    m_request.set_method(web::http::methods::POST);
    m_request.headers().add("X-Access-Token", accessToken);

    m_request.headers().add("Content-Type", "multipart/form-data;boundary=" + m_parser.boundary());

    m_task = pplx::task<void>([this]() {
        web::http::client::http_client downloadClient(m_contentURL);
        web::http::http_request downloadRequest;

        downloadRequest.set_method(web::http::methods::GET);
        pplx::task<web::http::http_response> task = downloadClient.request(downloadRequest);
        task.then([this](web::http::http_response response) {

            std::stringstream strm;

            strm << response.body();

            m_contentBinary.assign(std::istreambuf_iterator<char>(strm), std::istreambuf_iterator<char>());

        });
        task.wait();
        return;
    });
}

pplx::task<std::string> Video::upload() {
    while(!m_task.is_done()) {

    }
    m_request.set_body(m_parser.GenBodyContent());

    std::stringstream strm(m_client.request(m_request).get().to_string());

    strm >> m_json;

    web::http::http_request statusRequest;
    web::http::client::http_client statusClient(m_json["status_url"].dump());

    statusRequest.set_method(web::http::methods::GET);

    bool done = false;
}
