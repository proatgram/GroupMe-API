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

    class Video : public Attatchment {
        public:
            Video(std::string accessToken, std::filesystem::path path, std::string conversationID);

            Video(std::string accessToken, std::vector<uint8_t>& contentVector, std::string conversationID);

            Video(std::string accessToken, web::uri contentURL, std::string conversationID);

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
