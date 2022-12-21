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
#include <memory>

#include <cpprest/http_client.h>
#include <cpprest/uri.h>

namespace GroupMe {
    /**
     * This is a generic class that represents different types of attatchments
     * that can be sent and recieved with GroupMe
     *
     * The types of attatchments are:
     * - Picture (`GroupMe::Picture`)
     * - Picture (`GroupMe::Video`)
     * - File (`GroupMe::File`)
     * - Location (Yet to be implemented)
     * - Emoji (Yet to be  implemented)
     *
     * @brief A class that can represent different types of attatchments
     *
     */
    class Attatchment {
        public:
            //TODO Implement Location and Emoji attatchment types
            /**
             * @brief Enum class to represent different types of attatchments
             *
             */
            enum class Types {
                Picture,
                Video,
                File
            };
            
            /**
             * This constructor should be used when you want to create an attatchment from a local file
             * on your system. You can pass the path of the file into the contructor as a `std::filesystem::path`
             *
             * @brief Contructs a new `GroupMe::Attatchment` object
             *
             * @param contentPath The path on the filesystem of the attatchment
             * @param type The type of attatchment as a `GroupMe::Attatchment::Types`
             * @param accessToken The senders access token which is needed to upload attatchments
             *
             */
            Attatchment(const std::filesystem::path& contentPath, const Attatchment::Types& type, const std::string& accessToken);

            /**
             * This constructor should be used when you want to create an attatchment from a vector of
             * binary data. The data contained inside the vector should be a valid file type that can be
             * sent using GroupMe.
             *
             * @brief Constructs a new `GroupMe::Attatchment` object
             *
             * @param contentBinary A `std::vector<char>` that containes binary data for an attatchment to send
             * @param type The type of attatchment as a `GroupMe::Attatchment::Types`
             * @param accessToken The senders access token which is needed to upload attatchments
             *
             */
            Attatchment(const std::vector<unsigned char>& contentBinary, const Attatchment::Types& type, const std::string& accessToken);

            /**
             * This constructor should be used when you want to create an attatchment from data
             * on the internet. This constructor will grab the content located at the passed in URL
             * and send that as the attatchment. The URL passed in **MUST** be the endpoint for the attatchment.
             *
             * For example a URL like https://ExampleSite.com/content/02-20-22/example.jpg will work.
             *
             * @brief Constructs a new `GroupMe::Attatchment` object
             *
             * @param contentURL A URL of a valid attatchment to send
             * @param type The type of attatchment as a `GroupMe::Attatchment::Types`
             * @param accessToken The senders access token which is needed to upload attatchments
             *
             */
            Attatchment(const web::uri& contentURL, const Attatchment::Types& type, const std::string& accessToken);

            /**
             * This constructor should be used when you want to create an attatchment from data
             * in a string. Data should be the same as the constructor with the vector but encoded as
             * a string.
             *
             * @brief Constructs a new `GroupMe::Attatchment` object
             *
             * @param content A `std::string` that contains binary data for an attatchment to send
             * @param type The type of attatchment as a `GroupMe::Attatchment::Types`
             * @param accessToken The senders access token which is needed to upload attatchments
             *
             */
            Attatchment(const std::string& content, const Attatchment::Types& type, const std::string& accessToken);

            /**
             * The content URL will contain the endpoint of the uploaded attatchment
             * to send alongside a message.
             *
             * @brief Gets the content URL.
             *
             */
            web::uri getContentURL();

            /**
             * @brief Gets the type of attatchment
             *
             */
            Attatchment::Types getType();

        protected:

            /**
             * @brief The type of attatchment
             *
             */
            Attatchment::Types m_type;
        
            /**
             * @brief The content path on the system disk
             *
             */
            std::filesystem::path m_contentPath;

            /**
             * @brief The binary file represented as a string
             *
             */
            std::string m_content;

            /**
             * @brief The binary file represented as a vector
             *
             */
            std::vector<unsigned char> m_contentBinary;

            /**
             * @brief The senders access token which is needed to upload attatchments
             *
             */
            std::string m_accessToken;
    };

} 
