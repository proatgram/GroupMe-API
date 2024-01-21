/*
    This is a library used to communicate with the GroupMe API efficiently and seamlessly.
    Copyright (C) 2023 Timothy Hutchins

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

#include "BasicGroupChat.h"

namespace GroupMe {
    /**
     * A class to represent GroupMe SubGroup chats
     *
     */
    class SubGroupChat final : public BasicGroupChat {
        public:
            
            /**
             * @brief Constructs a new `GroupMe::SubGroupChat` object
             *
             * @param token The users access token
             *
             * @param groupId The group ID
             *
             * @param parentId The parent GroupChat ID
             *
             * @param name The group name
             *
             * @param description The description of the group
             *
             * @param imageUrl The URL of the image for the group
             *
             * @param createdAt The date the group was created at
             *
             * @param updatedAt The date the group was updated at
             *
             * @param members The members inherited from the GroupChat
             *
             */
           SubGroupChat(const std::string &token, const std::string &groupId, const std::string &parentId, const std::string &name, const std::string &description, const web::uri &imageUrl, unsigned long long int createdAt, unsigned long long int updatedAt, const std::shared_ptr<UserSet> &members);

            /**
             * @brief Constructs a new `GroupMe::SubGroupChat` object
             *
             * This will try to sync data about the group using the API
             *
             * @param token The users access token
             *
             * @param groupId The group ID
             *
             * @param parentId The parent GroupChat ID
             *
             * @param members The members inherited from the GroupChat
             *
             */
            SubGroupChat(const std::string &token, const std::string &groupId, const std::string &parentId, const std::shared_ptr<UserSet> &members);

            /**
             * @brief Constructs a new `GroupMe::SubGroupChat` object
             *
             */
            SubGroupChat() = default;

            SubGroupChat(const SubGroupChat &other) = default;

            SubGroupChat(SubGroupChat &&other) noexcept = default;

            SubGroupChat& operator=(const SubGroupChat &other) = default;

            SubGroupChat& operator=(SubGroupChat &&other) noexcept = default;

            ~SubGroupChat() override = default;

            /**
             * @brief Constructs a `SubGroupChat` object from a JSON object
             *
             * @param json The JSON object to use.
             *
             * @param members The members of the SubGroupChat.
             * The members will match the Parent Chat.
             *
             * @param accessToken The access token to be used to query the server
             *
             * @return std::shared_ptr<SubGroupChat>
             *
             */
            static std::shared_ptr<SubGroupChat> createFromJson(const nlohmann::json &json, const std::shared_ptr<UserSet> &members, const std::string &accessToken);

            /**
             * @brief Returns the ID of the Parent Group Chat this
             * SubGroupChat is associated with
             *
             * @return std::string
             *
             */
            std::string getParentId() const;

            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> update() final;

            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> queryMessages(unsigned int messageCount = DEFAULT_QUERY_LENGTH) final;

            /**
             * Send a message in the group
             *
             * See `GroupMe::Message` for more info
             */
            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> sendMessage(const Message& message) final;

        private:
            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> queryMessagesBefore(const GroupMe::Message &beforeMessage, unsigned int messageCount = DEFAULT_QUERY_LENGTH) final;

            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> queryMessagesAfter(const GroupMe::Message &afterMessage, unsigned int messageCount = DEFAULT_QUERY_LENGTH) final;

            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> queryMessagesSince(const GroupMe::Message &sinceMessage, unsigned int messageCount = DEFAULT_QUERY_LENGTH) final;
            std::string m_parentChatId;

            static constexpr std::string_view SUBGROUP_ENDPOINT_QUERY = "subgroups";
    };
}
