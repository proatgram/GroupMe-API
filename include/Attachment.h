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
     * This is a generic class that represents different types of attachments
     * that can be sent and recieved with GroupMe
     *
     * The types of attachments are:
     * - Picture (`GroupMe::Picture`)
     * - Video (`GroupMe::Video`)
     * - File (`GroupMe::File`)
     * - Location (Yet to be implemented)
     * - Emoji (Yet to be  implemented)
     *
     * @brief A class that can represent different types of attachments
     *
     */
    class Attachment {
        public:
            //TODO Implement Location and Emoji attachment types
            /**
             * @brief Enum class to represent different types of attachments
             *
             */
            enum class Types {
                Picture,
                Video,
                File
            };
            
            /**
             * This constructor should be used when you want to create an attachment from a local file
             * on your system. You can pass the path of the file into the contructor as a `std::filesystem::path`
             *
             * @brief Contructs a new `GroupMe::Attachment` object
             *
             * @param contentPath The path on the filesystem of the attachment
             * @param type The type of attachment as a `GroupMe::Attachment::Types`
             * @param accessToken The senders access token which is needed to upload attachments
             *
             */
            Attachment(const std::filesystem::path& contentPath, const Attachment::Types& type, const std::string& accessToken);

            /**
             * This constructor should be used when you want to create an attachment from a vector of
             * binary data. The data contained inside the vector should be a valid file type that can be
             * sent using GroupMe.
             *
             * @brief Constructs a new `GroupMe::Attachment` object
             *
             * @param contentBinary A `std::vector<char>` that containes binary data for an attachment to send
             * @param type The type of attachment as a `GroupMe::Attachment::Types`
             * @param accessToken The senders access token which is needed to upload attachments
             *
             */
            Attachment(const std::vector<unsigned char>& contentBinary, const Attachment::Types& type, const std::string& accessToken);

            /**
             * This constructor should be used when you want to create an attachment from data
             * on the internet. This constructor will grab the content located at the passed in URL
             * and send that as the attachment. The URL passed in **MUST** be the endpoint for the attachment.
             *
             * For example a URL like https://ExampleSite.com/content/02-20-22/example.jpg will work.
             *
             * @brief Constructs a new `GroupMe::Attachment` object
             *
             * @param contentURL A URL of a valid attachment to send
             * @param type The type of attachment as a `GroupMe::Attachment::Types`
             * @param accessToken The senders access token which is needed to upload attachments
             *
             */
            Attachment(const web::uri& contentURL, const Attachment::Types& type, const std::string& accessToken);

            /**
             * This constructor should be used when you want to create an attachment from data
             * in a string. Data should be the same as the constructor with the vector but encoded as
             * a string.
             *
             * @brief Constructs a new `GroupMe::Attachment` object
             *
             * @param content A `std::string` that contains binary data for an attachment to send
             * @param type The type of attachment as a `GroupMe::Attachment::Types`
             * @param accessToken The senders access token which is needed to upload attachments
             *
             */
            Attachment(const std::string& content, const Attachment::Types& type, const std::string& accessToken);

            /**
             * This constructor should be used when you want to create an attachment from a URL
             * without the intention of uploading it.
             *
             * @brief Constructs a new `GroupMe::Attachment` object
             *
             * @param contentURL A URL of a valid attachment
             * @param type The type of attachment as a `GroupMe::Attachment::Types`
             *
             */
            Attachment(const web::uri &contentURL, const Attachment::Types &type);

            /**
             * This constructor should be used when you want to create an attachment and
             * set the content as a string without the intention of uploading it.
             * 
             * @brief Constructs a new `GroupMe::Attachment` object
             *
             * @param content The content you want to input
             * @param type The type of attachment as a `GroupMe::Attachment`
             *
             */
            Attachment(const std::string &content, const Attachment::Types &type);

            /**
             * The content URL will contain the endpoint of the uploaded attachment
             * @return web::uri The URL of the content
             *
             */
            web::uri getContentURL();

            /**
             * @brief Gets the type of attachment
             *
             * @return Attachment::Types The type of the attachment
             *
             */
            Attachment::Types getType();

            /**
             * @brief Sets the content URL
             *
             * @param url The URL to set
             *
             * @return void
             *
             */
            void setContentURL(const web::uri &url);

        protected:

            /**
             * @brief The type of attachment
             *
             */
            Attachment::Types m_type;
        
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
             * @brief The senders access token which is needed to upload attachments
             *
             */
            std::string m_accessToken;
    };

} 
