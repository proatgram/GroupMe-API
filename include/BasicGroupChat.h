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

#include <nlohmann/json.hpp>

#include "Picture.h"

#include "BasicChat.h"

namespace GroupMe {
    /**
     * An abstract class to represent GroupMe group chats
     *
     */
    class BasicGroupChat : public BasicChat {
        public:
            /**
             * @brief Constructs a new `GroupMe::BasicGroupChat` object
             *
             */
            BasicGroupChat();

            /**
             * @brief Constructs a new `GroupMe::BasicGroupChat` object
             *
             * @param token The users access token
             *
             * @param groupId The group ID
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
             */
           BasicGroupChat(const std::string &token, const std::string &groupId, const std::string &name, const std::string &description, const web::uri &imageUrl, unsigned long long int createdAt, unsigned long long int updatedAt);

            /**
             * @brief Constructs a new `GroupMe::BasicGroupChat` object
             *
             * This will try to sync data about the group using the API
             *
             * @param token The users access token
             *
             * @param groupId The group ID
             *
             */
            BasicGroupChat(const std::string &token, const std::string &groupId);

            BasicGroupChat(const BasicGroupChat &other) = default;

            BasicGroupChat(BasicGroupChat &&other) noexcept = default;

            BasicGroupChat& operator=(const BasicGroupChat &other) = default;

            BasicGroupChat& operator=(BasicGroupChat &&other) noexcept = default;

             ~BasicGroupChat() override = default;

            /**
             * @brief Gets the groups description
             *
             * @return std::string The description
             *
             */
            std::string getDescription() const;

            /**
             * @brief Sets the group description
             *
             * @return void
             *
             */
            void setDescription(const std::string &description);

            /**
             * @brief Gets the groups image URL
             *
             * @return std::string The URL to the image
             *
             */
            std::string getImageUrl() const;

            /**
             * Sets the groups image to the image
             *
             * This will attempt to upload the image to GroupMe's servers
             * to use in the GroupChat if it's not already uploaded.
             *
             * @brief Sets the groups image
             *
             * @param image A `GroupMe::Picture` holding an image to set
             *
             * @return pplx::task<BasicChat::Result> The return of the concurrent task will be the result of the operation
             *
             */
            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> setImage(GroupMe::Picture &image);

            /**
             * @brief Sets the groups image to the image based on a url
             *
             * @param url A `web::uri` holding an image url to set
             *
             * @return void
             *
             */
            void setImage(const web::uri &url);

            /**
             * Gets the date the group was updated at
             *
             * Not really sure the format as not much about
             * the dates are given in the documentation
             *
             * @brief Gets the date and time the group was updated at
             *
             * @return unsigned long long int The date the group was updated
             *
             */
            unsigned long long int getUpdatedAt() const;

            /**
             * @brief Updates a group after creation
             * 
             * @return bool true if succeeded and false if not
             *
             * The return should not be discarded. Make sure the task being run is finished before program termination.
             *
             */
            [[nodiscard("Manage the task")]]
            virtual pplx::task<BasicChat::Result> update() = 0;

            /**
             * @brief Returns the messages in the groupchat
             *
             * @return MessageSet (std::set with custom compare)
             *
             */
            const MessageSet& getMessages() const;

            /**
             * @brief Gets the group name
             *
             * @return std::string The group name
             *
             */
            std::string getName() const;

            /**
             * @brief Sets the group name
             *
             * @param name The new name to set
             *
             * @return void
             *
             */
            void setName(const std::string &name);

            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> queryMessages(const GroupMe::Message &referenceMessage, GroupMe::BasicChat::QueryType queryType, unsigned int messageCount = DEFAULT_QUERY_LENGTH) final;

        protected:
            std::string m_groupDescription;

            std::string m_groupImageUrl;

            unsigned long long int m_updatedAt;

            std::string m_name;

            static constexpr std::string_view GROUP_ENDPOINT_QUERY = "groups";
    };
}
