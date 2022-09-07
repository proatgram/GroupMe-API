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

#include "base/Video.h"

using namespace GroupMe;

Video::Video(std::string accessToken, std::filesystem::file file) :
    Attatchment(file, Attatchment::Types::Video),
    m_request(),
    m_client("https://video.groupme.com/transcode"),
    m_header(),
    m_json(),
    m_binaryData()
{
    m_request.set_method(web::http::methods::POST);
    m_request.headers().add("")
}
