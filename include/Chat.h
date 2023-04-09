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

#include <string>
#include <mutex>
#include <cpprest/http_client.h>
#include <cpprest/uri.h>

#include "UserSet.hpp"
#include "Message.h"
#include "Picture.h"
#include "User.h"

namespace GroupMe {
    /**
     * @brief A class that represents the most basic form of a chat
     * only consisting of an ID for the chat
     *
     */
    class Chat {
        public:
            /**
             * @brief Constructs a new `GroupMe::Chat` object
             *
             * @param chatId The ID of the Chat
             *
             */
            explicit Chat(const std::string &chatId);

            /**
             * @brief Constructs a new `GroupMe::Chat` object
             *
             */
            Chat() = default;

            virtual ~Chat() = default;

            Chat(const Chat &other) = delete;

            Chat(Chat &&other) = delete;

            Chat& operator=(const Chat &other) = delete;

            Chat& operator=(Chat &&other) = delete;
        protected:
            std::string m_chatId;
    };
}
