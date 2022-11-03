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

#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <exception>
#include <thread>
#include <chrono>

#include <nlohmann/json.hpp>

#include "base/Attatchment.h"
#include "base/Exceptions.h"

namespace GroupMe {
    class File : public Attatchment {
        public:
            File(std::string accessToken, std::filesystem::path path, std::string conversationID);

            File(std::string accessToken, std::vector<uint8_t> contentVector, std::string conversationID);

            File(std::string accessToken, web::uri contentURL, std::string conversationID);

            pplx::task<std::string> upload();

        private:

            static std::string getURL(std::string conversationID, std::string filename);

            web::http::http_request m_request;

            std::string m_conversationID;

            pplx::task<void> m_task;

            nlohmann::json m_json;

            web::http::client::http_client m_client;
    };
}
