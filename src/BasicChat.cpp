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

#include "BasicChat.h"

using namespace GroupMe;

BasicChat::BasicChat() :
    m_createdAt(),
    m_client("https://api.groupme.com/v3/"),
    m_members(std::make_shared<UserSet>())
{
    
}

BasicChat::BasicChat(const std::string &chatId) :
    m_chatId(chatId),
    m_createdAt(),
    m_client("https://api.groupme.com/v3/"),
    m_members(std::make_shared<UserSet>())
{

}

BasicChat::BasicChat(const std::string &chatId, const std::string &accessToken) :
    m_chatId(chatId),
    m_accessToken(accessToken),
    m_createdAt(),
    m_client("https://api.groupme.com/v3/"),
    m_members(std::make_shared<UserSet>())
{

}

BasicChat::BasicChat(const std::string &chatId, unsigned long long int createdAt) :
    m_chatId(chatId),
    m_createdAt(createdAt), 
    m_client("https://api.groupme.com/v3/"),
    m_members(std::make_shared<UserSet>())
{

}

BasicChat::BasicChat(const std::string &chatId, const std::string &accessToken, unsigned long long int createdAt) :
    m_chatId(chatId),
    m_accessToken(accessToken),
    m_createdAt(createdAt),
    m_client("https://api.groupme.com/v3/"),
    m_members(std::make_shared<UserSet>())
{

}

BasicChat::BasicChat(const BasicChat &other) :
    m_chatId(other.m_chatId),
    m_accessToken(other.m_accessToken),
    m_createdAt(other.m_createdAt),
    m_client(other.m_client),
    m_members(other.m_members)
{

}

BasicChat::BasicChat(BasicChat &&other) noexcept :
    m_chatId(std::move(other.m_chatId)),
    m_accessToken(std::move(other.m_accessToken)),
    m_createdAt(other.m_createdAt),
    m_client(other.m_client),
    m_members(std::move(other.m_members))
{

}

BasicChat& BasicChat::operator=(const BasicChat &other) {
    if (this != &other) {
        m_chatId = other.m_chatId;
        m_accessToken = other.m_accessToken;
        m_createdAt = other.m_createdAt;
        m_client = other.m_client;
        m_members = other.m_members;
    }
    return *this;
}

BasicChat& BasicChat::operator=(BasicChat &&other) noexcept {
    m_chatId = std::move(other.m_chatId);
    m_accessToken = std::move(other.m_accessToken);
    m_createdAt = other.m_createdAt;
    m_client = other.m_client;
    m_members = std::move(other.m_members);
    
    return *this;
}

const std::shared_ptr<GroupMe::UserSet>& BasicChat::getMembers() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_members;
}

unsigned long long int BasicChat::getCreatedAt() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_createdAt;
}

std::string BasicChat::getId() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_chatId;
}

