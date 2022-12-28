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

#include "base/Message.h"

using namespace GroupMe;

Message::Message(std::shared_ptr<const GroupMe::User> sender, const std::string& message, const std::string& GUID) :
    m_id(),
    m_guid(GUID),
    m_createdAt(),
    m_pinned(),
    m_pinnedAt(),
    m_pinnedBy(),
    m_sender(sender),
    m_attachments(),
    m_favoritedBy(),
    m_text(message)
{

}

Message::Message(const std::string& message, const std::string& GUID) :
    m_id(),
    m_guid(GUID),
    m_createdAt(),
    m_pinned(),
    m_pinnedAt(),
    m_pinnedBy(),
    m_sender(),
    m_attachments(),
    m_favoritedBy(),
    m_text(message)
{

}

Message::Message(std::shared_ptr<const GroupMe::User> sender, const std::string& GUID) :
    m_id(),
    m_guid(GUID),
    m_createdAt(),
    m_pinned(),
    m_pinnedAt(),
    m_pinnedBy(),
    m_sender(sender),
    m_attachments(),
    m_favoritedBy(),
    m_text()
{

}

Message::Message(const std::string& GUID) :
    m_id(),
    m_guid(GUID),
    m_createdAt(),
    m_pinned(),
    m_pinnedAt(),
    m_pinnedBy(),
    m_sender(),
    m_attachments(),
    m_favoritedBy(),
    m_text()
{

}

std::string Message::getID() {
    return m_id;
}

void Message::setID(const std::string& id) {
    m_id = id;
}

unsigned int Message::getCreatedAt() {
    return m_createdAt;
}

void Message::setCreatedAt(unsigned int createdAt) {
    m_createdAt = createdAt;
}

