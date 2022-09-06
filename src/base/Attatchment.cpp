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

Attatchment::Attatchment(std::filesystem::path contentPath, Attatchment::Types type) :
    m_type(type),
    m_contentPath(contentPath),
    m_contentURL()
{

}

Attatchment::Attatchment(std::string contentURL, Attatchment::Types type) :
    m_type(type),
    m_contentPath(),
    m_contentURL(contentURL)
{

}

Attatchment::Types Attatchment::getType() {
    return m_type;
}

std::string Attatchment::getContentURL() {
    return m_contentURL;
}
