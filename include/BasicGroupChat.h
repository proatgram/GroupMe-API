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

#include <list>
#include <memory>
#include <map>
#include <thread>
#include <chrono>
#include <utility>
#include <nlohmann/json.hpp>

#include "BasicChat.h"

namespace GroupMe {
    /**
     * A class to represent GroupMe group chats
     *
     * @brief A class to represent GroupMe group chats
     *
     */
    class BasicGroupChat : public BasicChat {
        public:
            /**
             * @brief The visibility / joinability of the groups
             *
             */
            enum class VisibilityType {
                Public,
                Private,
                Hidden
            };

            /**
             * @brief Constructs a new `GroupMe::BasicGroupChat` object
             *
             * @param token The users access token
             *
             * @param groupId The group ID
             *
             * @param name The group name
             *
             * @param type The visibility of the group
             * 
             * @param description The description of the group
             *
             * @param imageUrl The URL of the image for the group
             *
             * @param creator The group creator
             *
             * @param createdAt The date the group was created at
             *
             * @param updatedAt The date the group was updated at
             *
             * @param shareUrl The share URL for the group
             *
             */
           BasicGroupChat(const std::string &token, const std::string &groupId, const std::string &name, VisibilityType type, const std::string &description, const web::uri &imageUrl, const std::shared_ptr<User> &creator, unsigned long long int createdAt, unsigned long long int updatedAt, const web::uri &shareUrl);

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

            BasicGroupChat(const BasicGroupChat &other) = delete;

            BasicGroupChat(BasicGroupChat &&other) = delete;

            BasicGroupChat& operator=(const BasicGroupChat &other) = delete;

            BasicGroupChat& operator=(BasicGroupChat &&other) = delete;

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
             * @brief Sets the groups image
             *
             * @param image A `GroupMe::Picture` holding an image to set
             *
             * @return void
             *
             */
            void setImage(GroupMe::Picture &image);

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
             * @brief Gets the creator of the group
             *
             * @return 
             *
             */
            std::shared_ptr<const GroupMe::User> getCreator() const;

            /**
             * @brief Gets the visibility for the group
             *
             * @return `Chat::VisibilityType` The visibility of the group
             *
             */
            GroupMe::BasicGroupChat::VisibilityType getVisibility() const;

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
            pplx::task<BasicChat::Result> update();

            /**
             * @brief Returns the messages in the groupchat
             *
             * @return std::map<unsigned long long int, GroupMe::Message>
             *
             */
            const std::map<unsigned long long int, GroupMe::Message>& getMessages() const;

            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> queryMessages(const GroupMe::Message &referenceMessage, GroupMe::BasicChat::QueryType queryType, unsigned int messageCount = DEFAULT_QUERY_LENGTH) final;

        protected:
            std::string m_groupDescription;

            std::string m_groupImageUrl;

            std::string m_groupShareUrl;

            BasicGroupChat::VisibilityType m_groupVisibility;

            unsigned long long int m_updatedAt;

            std::shared_ptr<GroupMe::User> m_groupCreator;
    };
}
