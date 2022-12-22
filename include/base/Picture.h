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

#include "base/Attachment.h"

namespace GroupMe {
    //TODO Add a constructor to upload as with a `std::vector<unsigned char>`
    /**
     * This class is used to hold the Picture attatchment type and can be used
     * in messages to send a picture.
     *
     * @brief A class used to hold a Picture attatchment
     *
     */
    class Picture : public Attachment {
        public:
            /**
             * This constructor should be used when you want to upload a
             * picture attatchment from a local file on your system. You should
             * pass the path of the pctire as a `std::filesytstem::path`
             *
             * @brief Constructs a new `GroupMe::Picture` object
             *
             * @param accessToken The senders access token which is needed to upload the picture
             * @param path The path to the picture to upload
             *
             */
            Picture(const std::string& accessToken, const std::filesystem::path& path);

            /**
             * This constructor should be used when you want to upload a
             * picture attatchment from a URL endpoint. This constructor will
             * download whatever is at the endpoint of the URL and use that
             * as the attatchment to upload.
             *
             * @brief Constructs a new `GroupMe::Picture` object
             *
             * @param accessToken The senders access token which is needed to upload the picture
             * @param contentURL A URL of a picture to upload as a picture
             *
             */
            Picture(const std::string& accessToken, const web::uri& contentURL);

            ~Picture();

            /**
             * This member function will upload the picture to the GroupMe
             * servers concurrently. It uses Microsofts pplx library to
             * achieve this. Since this function runs conqurently, we
             * must wait for the upload to complete. We can do this with
             * the `.get()` function.
             *
             * For example:
             * `std::string url = picture.upload().get();`
             *
             * @brief Uploads the picture to the server
             *
             * @return pplx::task<std::string>
             *
             */
            pplx::task<std::string> upload();

        private:
            web::http::http_request m_request;
            
            web::http::client::http_client m_client;
            
            nlohmann::json m_json;

            std::fstream m_file;

            pplx::task<void> m_task;
    };

}
