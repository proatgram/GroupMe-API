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

#include "Chat.h"

using namespace GroupMe;

Chat::Chat(unsigned long long int createdAt) :
    m_createdAt(createdAt), 
    m_task(pplx::task<void>([]() -> void {
        return;
    }))
{

}

Chat::Chat() :
    m_createdAt(),
    m_task(pplx::task<void>([]() -> void {
        return;
    }))
{
    
}

Chat::~Chat() {
    m_task.wait();
}

unsigned long long int Chat::getCreatedAt() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_createdAt;
}

bool Chat::queryMessages(const Message &referenceMessage, QueryType queryType, unsigned int messageCount) {
    return false;
}

bool Chat::queryMessagesBefore(const Message &beforeMessage, unsigned int messageCount) {
    return false;
}

bool Chat::queryMessagesAfter(const Message &afterMessage, unsigned int messageCount) {
    return false;
}

bool Chat::queryMessagesSince(const Message &sinceMessage, unsigned int nessageCount) {
    return false;
}
