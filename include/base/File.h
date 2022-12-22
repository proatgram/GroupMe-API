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

#include "base/Attachment.h"
#include "base/Exceptions.h"

namespace GroupMe {
    /**
     * This class is used to hold the File attatchment type and can be used
     * in messages to send a file.
     *
     * @brief A class used to hold a File attatchment
     *
     */
    class File : public Attachment {
        public:
            /**
             * This constructor should be used when you want to upload a
             * file attatchment from a local file on your system. You should
             * pass the path of the file as a `std::filesystem::path`
             *
             * @brief Constructs a new `GroupMe::File` object
             *
             * @param accessToken The senders access token which is needed to upload the file
             * @param path The path of the file to upload
             * @param conversationID The ID of the conversation to upload the file to
             *
             */
            File(std::string accessToken, std::filesystem::path path, std::string conversationID);

            //TODO Add name paremeter
            /**
             * This constructor should be used when you want to upload a
             * file attatchment from a vector of data.
             *
             * @brief Constructs a new `GroupMe::File` object
             *
             * @param accessToken The senders access token which is needed to upload the file
             * @param contentVector A vector full of data to upload
             * @param conversationID The ID of the conversation to upload the file to
             *
             */
            File(std::string accessToken, std::vector<unsigned char> contentVector, std::string conversationID);

            /**
             * This constructor should be used when you want to upload a
             * file attatchment from a URL endpoint. This constructor will
             * download whatever is at the endpoint of the URL and use that
             * as the attatchment to upload.
             *
             * @brief Constructs a new `GroupMe::File` object
             *
             * @param accessToken The senders access token which is needed to upload the file
             * @param contentURL A URL of a file to upload as a file
             * @param conversationID The ID of the conversation to upload the file to
             *
             */
            File(std::string accessToken, web::uri contentURL, std::string conversationID);

            ~File();

            /**
             * This member function will upload the file to the GroupMe
             * servers concurrently. It uses Microsofts pplx library to
             * achieve this. Since this function runs conqurently, we
             * must wait for the upload to complete. We can do this with
             * the `.get()` function.
             *
             * For example:
             * `std::string url = file.upload().get();`
             *
             * @brief Uploads the file to the server
             *
             * @return pplx::task<std::string>
             *
             */
            pplx::task<std::string> upload();

        private:

            // This function creates a url from two strings. Saves me a lot of pain
            static std::string getURL(std::string conversationID, std::string filename);

            web::http::http_request m_request;

            std::string m_conversationID;

            pplx::task<void> m_task;

            nlohmann::json m_json;

            web::http::client::http_client m_client;
    };
}
