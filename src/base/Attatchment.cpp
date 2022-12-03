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

#include "base/Attatchment.h"

using namespace GroupMe;

Attatchment::Attatchment(const std::filesystem::path& contentPath, const Attatchment::Types& type, const std::string& accessToken) :
    m_type(type),
    m_contentPath(contentPath),
    m_content(),
    m_accessToken(accessToken)
{

}

Attatchment::Attatchment(const std::vector<unsigned char>& contentBinary, const Attatchment::Types& type, const std::string& accessToken) :
    m_type(type),
    m_contentPath(),
    m_content(),
    m_contentBinary(contentBinary),
    m_accessToken(accessToken)
{

}

Attatchment::Attatchment(const web::uri& contentURL, const Attatchment::Types& type, const std::string& accessToken) :
    m_type(type),
    m_contentPath(),
    m_content(contentURL.to_string()),
    m_contentBinary(),
    m_accessToken(accessToken)
{

}

Attatchment::Types Attatchment::getType() {
    return m_type;
}

web::uri Attatchment::getContentURL() {
    return m_content;
}
