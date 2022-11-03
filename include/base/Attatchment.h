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

    class Attatchment {
        public:
            enum class Types {
                Picture,
                Video,
                File
            };
            
            Attatchment(std::filesystem::path contentPath, Attatchment::Types type, std::string accessToken);

            Attatchment(web::uri contentURL, Attatchment::Types type, std::string accessToken);

            Attatchment(std::vector<unsigned char> contentBinary, Attatchment::Types type, std::string accessToken);

            web::uri getContentURL();

            Attatchment::Types getType();

        protected:

            Attatchment::Types m_type;
        
            std::filesystem::path m_contentPath;

            std::string m_content;

            std::vector<unsigned char> m_contentBinary;

            std::string m_accessToken;

        private:

    };

} 
