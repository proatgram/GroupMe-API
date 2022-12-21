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

#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <filesystem>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <sstream>
#include <algorithm>
#include <thread>
#include <chrono>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include "base/Attatchment.h"
#include "base/multipart_parser.h"
#include "base/Exceptions.h"
#include "base/AVFileMem.h"

namespace GroupMe {
    /**
     * This class is used to hold the Video attatchment type and can be used
     * in messages to send videos.
     *
     * @brief A class used to hold a Video attatchment
     *
     */
    class Video : public Attatchment {
        public:
            /**
             * This constructor should be used when you want to upload a
             * video attatchment from a local file on your system. You should
             * pass the path of the file as a `std::filesystem::path`
             *
             * @brief Constructs a new `GroupMe::Video` object
             *
             * @param accessToken The senders access token which is needed to upload the video
             * @param path The path to the video to upload
             * @param conversationID The ID of the conversation to upload the Video to
             *
             */
            Video(const std::string& accessToken, const std::filesystem::path& path, const std::string& conversationID);

            /**
             * This constructor should be used when you want to upload a
             * video attatchment from a vector of data.
             *
             * @brief Constructs a new `GroupMe::Video` object
             *
             * @param accessToken The senders access token which is needed to upload the video
             * @param contentVector A vector full of data to upload
             * @param conversationID The ID of the conversation to upload the Video to
             *
             */
            Video(const std::string& accessToken, const std::vector<unsigned char>& contentVector, const std::string& conversationID);

            /**
             * This constructor should be used when you want to upload a
             * video attatchment from a URL endpoint. This constructor will
             * download whatever is at the endpoint of the URL and use that
             * as the attatchment to upload.
             *
             * @brief Constructs a new `GroupMe::Video` object
             *
             * @param accessToken The senders access token which is needed nto upload the video
             * @param contentURL A URL of a file to upload as a video
             * @param conversationID The ID of the conversation to upload the Video to
             *
             */
            Video(const std::string& accessToken, const web::uri& contentURL, const std::string& conversationID);

            ~Video();

            /**
             * This member function will upload the video to the GroupMe
             * servers concurrently. It uses Microsofts pplx library to
             * achieve this. Since this function runs conqurently, we
             * must wait for the upload to complete. We can do this with
             * the `.get()` function.
             *
             * For example:
             * `std::string url = video.upload().get();`
             *
             * @brief Uploads the video to the server
             *
             * @return pplx::tasl<std::string>
             *
             */
            pplx::task<std::string> upload();

        private:
            web::http::http_request m_request;

            web::http::client::http_client m_client;

            web::http::MultipartParser m_parser;

            std::string m_conversationID;

            pplx::task<void> m_task;

            nlohmann::json m_json;
    };

}
