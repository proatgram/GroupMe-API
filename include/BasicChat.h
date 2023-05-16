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

#include "Chat.h"

namespace GroupMe {
    class BasicChat : public GroupMe::Chat {
        public:
            /**
             * Represents the query type for message querying
             * 
             *  - Before Message
             *    + Gives messages returned before a given message
             *  - After Message
             *    + Gives message created immediately after the given message
             *  - Since Message
             *    + Returns _**most recent**_ messages created after the given message
             *
             * @brief Represents the query type for message querying
             *
             */
            enum class QueryType {
                Before,
                After,
                Since
            };

            /**
             * @brief Represents different results for Chat operations
             *
             */
            enum class Result {
                Success,
                Failure,
                ResultsExpired,
                NotFound,
                InvalidQueryType,
                RequesterIsNewOwner,
                RequesterIsNotOwner,
                MissingData
            };

            /**
             * The default message query amount if not specified
             * in the web request is 20
             *
             * @brief Default query amount
             *
             */
            static constexpr int DEFAULT_QUERY_LENGTH = 20;

            /**
             * @brief Constructs a new `GroupMe::BasicChat` object
             *
             * @param chatId The ID of the chat
             *
             * @param createdAt The date the group was created at
             *
             */
            BasicChat(const std::string &chatId, unsigned long long int createdAt);

            /**
             * @brief Constructs a new `GroupMe::BasicChat` object
             *
             * @param chatId The ID of the chat
             *
             */
            explicit BasicChat(const std::string &chatId);

            BasicChat();

            ~BasicChat() override;

            BasicChat(const BasicChat &other) = delete;

            BasicChat(BasicChat &&other) = delete;

            BasicChat& operator=(const BasicChat &other) = delete;

            BasicChat& operator=(BasicChat &&other) = delete;

            /**
             * Gets the date the chat was created at
             *
             * Not really sure the format as not much about
             * the dates are given in the documentation
             *
             * @brief Gets the date and time the chat was created at
             *
             * @return unsigned long long int The date the chat was created
             *
             */
            unsigned long long int getCreatedAt() const;

            /**
             * Query messages in the group
             *
             * There are three ways you can query messages in a chat
             *
             *  - Before Message
             *    + Gives messages returned before a given message
             *  - After Message
             *    + Gives message created immediately after the given message
             *  - Since Message
             *    + Returns _**most recent**_ messages created after the given message
             *
             * There is also a limit to how many it will give you, the default the API sets is 20
             * Not only that there is a maximum it can give you, which is 100 messages
             *
             * @brief Query messages in the chat
             *
             * @param referenceMessage The message you want to use as the reference to query
             *
             * @param queryType The query type you would like to use as a `GroupMe::Chat::QueryType`
             *
             * @param messageCount The count of messages that will be queried. The default is 20 if none is supplied.
             *
             * @return BasicChat::Result The result of the operation
             *
             */
            virtual pplx::task<BasicChat::Result> queryMessages(const GroupMe::Message &referenceMessage, GroupMe::BasicChat::QueryType queryType, unsigned int messageCount = DEFAULT_QUERY_LENGTH) = 0;

            virtual pplx::task<BasicChat::Result> queryMessages(unsigned int messageCount = DEFAULT_QUERY_LENGTH) = 0;

        protected:
            virtual pplx::task<BasicChat::Result> queryMessagesBefore(const GroupMe::Message &beforeMessage, unsigned int messageCount = DEFAULT_QUERY_LENGTH) = 0;

            virtual pplx::task<BasicChat::Result> queryMessagesAfter(const GroupMe::Message &afterMessage, unsigned int messageCount = DEFAULT_QUERY_LENGTH) = 0;

            virtual pplx::task<BasicChat::Result> queryMessagesSince(const GroupMe::Message &sinceMessage, unsigned int messageCount = DEFAULT_QUERY_LENGTH) = 0;

            unsigned long long int m_createdAt;

            std::map<unsigned long long int, GroupMe::Message> m_messages;
            
            pplx::task<void> m_task;

            mutable std::mutex m_mutex;
    };
}
