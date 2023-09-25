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

#include <cpprest/uri_builder.h>
#include <libavutil/pixfmt.h>

#include "BasicGroupChat.h"

using namespace GroupMe;

BasicGroupChat::BasicGroupChat(const std::string &token, const std::string &groupId, const std::string &name, const std::string &description, const web::uri &imageUrl, unsigned long long int createdAt, unsigned long long int updatedAt) :
    BasicChat(groupId, token, createdAt),
    m_groupDescription(description),
    m_groupImageUrl(imageUrl.to_string()),
    m_updatedAt(updatedAt),
    m_name(name)
{

}

BasicGroupChat::BasicGroupChat(const std::string &token, const std::string &groupId) :
    BasicChat(groupId, token),
    m_updatedAt()
{

}

BasicGroupChat::BasicGroupChat() :
    m_updatedAt()
{

}

std::string BasicGroupChat::getDescription() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupDescription;
}

void BasicGroupChat::setDescription(const std::string &description) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_groupDescription = description;
}

std::string BasicGroupChat::getImageUrl() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupImageUrl;
}

pplx::task<BasicChat::Result> BasicGroupChat::setImage(GroupMe::Picture &image) {
    return pplx::task<BasicChat::Result>([&image, this]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_groupImageUrl = image.upload().get();
        return BasicChat::Result::Success;
    });
}

void BasicGroupChat::setImage(const web::uri &url) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_groupImageUrl = url.to_string();
}

unsigned long long int BasicGroupChat::getUpdatedAt() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_updatedAt;
}

const MessageSet& BasicGroupChat::getMessages() const {
    return std::as_const(m_messages);
}

pplx::task<BasicChat::Result> BasicGroupChat::queryMessages(const Message &referenceMessage, BasicChat::QueryType queryType, unsigned int messageCount) {
    switch (queryType) {
        case BasicChat::QueryType::Before: {
            return queryMessagesBefore(referenceMessage, messageCount);
        }
        case BasicChat::QueryType::After: {
            return queryMessagesAfter(referenceMessage, messageCount);
        }
        case BasicChat::QueryType::Since: {
            return queryMessagesSince(referenceMessage, messageCount);
        }
        default: {
            return pplx::task<BasicChat::Result>([]() -> BasicChat::Result { return BasicChat::Result::InvalidQueryType; });
        }
    }
}

std::string BasicGroupChat::getName() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_name;
}

void BasicGroupChat::setName(const std::string &name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_name = name;
}

