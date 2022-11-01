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
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

#include "base/Attatchment.h"

namespace GroupMe {

    class Picture : public Attatchment {
        public:
            Picture(std::string accessToken, std::filesystem::path path);

            Picture(std::string accessToken, web::uri contentURL);

            pplx::task<std::string> upload();

        private:
            web::http::http_request m_request;
            
            web::http::client::http_client m_client;
            
            nlohmann::json m_json;

            std::vector<unsigned char> m_binaryData;

            std::fstream m_file;

            pplx::task<void> m_task;
    };

}
