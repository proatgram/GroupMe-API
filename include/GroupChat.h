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
    class GroupChat : public GroupMe::BasicChat {
        public:
            /**
             * The visibility / joinability of the Groups.
             *
             * @brief The visibility / joinability of the groups
             *
             */
            enum class VisibilityType {
                Public,
                Private,
                Hidden
            };

            /**
             * @brief Constructs a new `GroupMe::GroupChat` object
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
           GroupChat(const std::string &token, const std::string &groupId, const std::string &name, VisibilityType type, const std::string &description, const web::uri &imageUrl, const std::shared_ptr<User> &creator, unsigned long long int createdAt, unsigned long long int updatedAt, const web::uri &shareUrl);

           /**
            * @brief Constructs a new `GroupMe::GroupChat` object
            *
            * This will try to sync data about the group using the API
            *
            * @param token The users access token
            *
            * @param groupId The group ID
            *
            */
           GroupChat(const std::string &token, const std::string &groupId);

            ~GroupChat() override = default;

            /**
             * @brief Gets the group ID
             *
             * @return std::string The group ID
             * 
             */
            std::string getGroupId() const;

            /**
             * @brief Gets the group name
             *
             * @return std::string The group name
             *
             */
            std::string getGroupName() const;

            /**
             * @brief Sets the group name
             *
             * @param name The new name to set
             *
             * @return void
             *
             */
            void setGroupName(const std::string &name);

            /**
             * @brief Gets the groups description
             *
             * @return std::string The description
             *
             */
            std::string getGroupDescription() const;

            /**
             * @brief Sets the group description
             *
             * @return void
             *
             */
            void setGroupDescription(const std::string &description);

            /**
             * @brief Gets the groups image URL
             *
             * @return std::string The URL to the image
             *
             */
            std::string getGroupImageUrl() const;

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
            void setGroupImage(GroupMe::Picture &image);

            /**
             * @brief Sets the groups image to the image based on a url
             *
             * @param url A `web::uri` holding an image url to set
             *
             * @return void
             *
             */
            void setGroupImage(const web::uri &url);

            /**
             * @brief Gets the creator of the group
             *
             * @return 
             *
             */
            std::shared_ptr<const GroupMe::User> getCreator() const;

            /**
             * @brief Gets the groups share URL
             *
             * @return std::string the share URL for the group
             *
             */
            std::string getGroupShareUrl() const;

            /**
             * @brief Gets the visibility for the group
             *
             * @return `Chat::VisibilityType` The visibility of the group
             *
             */
            GroupMe::GroupChat::VisibilityType getGroupVisibility() const;

            /**
             * @brief Sets the groups visibility / joinability
             *
             * @param visibility The new visibility of the group to set
             *
             * @return void
             *
             */

            void setGroupVisibility(GroupMe::GroupChat::VisibilityType visibility);
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
             * @brief Gets the members in the group
             *
             * @return `GroupMe::UserSet` The container that holds the members
             *
             */
            const GroupMe::UserSet& getGroupMembers() const;

            /**
             * @brief Adds a member to the group
             *
             * @param user The user to add to the group as a `GroupMe::User`
             *
             * @return pplx::task<BasicChat::Result> The return of the concurrent task will be the result of the operation
             *
             */
            pplx::task<BasicChat::Result> addGroupMember(const GroupMe::User &user);

            /**
             * @brief Adds a member to the group
             *
             * @param user The user to add to the group as encapsulated in a `std::shared_ptr<GroupMe::User>`
             *
             * @return pplx::task<BasicChat::Result> The return of the concurrent task will be the result of the operation
             *
             */
            pplx::task<BasicChat::Result> addGroupMember(const std::shared_ptr<GroupMe::User> &user);

            /**
             * @brief Removes a member from the group
             *
             * @param user The user to remvoe from the group as a `GroupMe::User`
             *
             * @return pplx::task<BasicChat::Result> The return of the concurrent task will be the result of the operation
             *
             */
            pplx::task<BasicChat::Result> removeGroupMember(const GroupMe::User &user);

            /**
             * @brief Removes a member from the group
             *
             * @param user The user to remove from the group encapsulated in a `std::shared_ptr<GroupMe::User>`
             *
             * @return pplx::task<BasicChat::Result> The return of the concurrent task will be the result of the operation
             *
             */
            pplx::task<BasicChat::Result> removeGroupMember(const std::shared_ptr<GroupMe::User> &user);

            /**
             * @brief Disbands / destroys a group
             *
             * This action is only available to the group creator
             *
             * @return pplx::task<BasicChat::Result> The return of the concurrent task will be the result of the operation
             *
             */
            pplx::task<BasicChat::Result> destroyGroup();

            /**
             * @brief Change the owner of the group
             *
             * This action is only available to the group creator
             *
             * @param user The new owner to set as a `GroupMe::User`
             *
             * @return pplx::task<BasicChat::Result> The return of the concurrent task will be the result of the operation
             *
             */
            pplx::task<BasicChat::Result> changeGroupOwner(const GroupMe::User &user);

            /**
             * @brief Change the owner of the group
             *
             * This action is only available to the group creator
             *
             * @param user The new owner to set as a `std::shared_ptr<GroupMe::User>`
             *
             * @return pplx::task<BasicChat::Result> The return of the concurrent task will be the result of the operation
             *
             */
            pplx::task<BasicChat::Result> changeGroupOwner(const std::shared_ptr<GroupMe::User> &user);

            /**
             * @brief Updates a group after creation
             * 
             * @return bool true if succeeded and false if not
             *
             */
            pplx::task<BasicChat::Result> update();

            /**
             * @brief Returns the messages in the groupchat
             *
             * @return std::map<unsigned long long int, GroupMe::Message>
             *
             */
            const std::map<unsigned long long int, GroupMe::Message>& getMessages() const;

            pplx::task<BasicChat::Result> queryMessages(const GroupMe::Message &referenceMessage, GroupMe::BasicChat::QueryType queryType, unsigned int messageCount = DEFAULT_QUERY_LENGTH) override;

            pplx::task<BasicChat::Result> queryMessages(unsigned int messageCount = DEFAULT_QUERY_LENGTH) override;

        private:
            pplx::task<BasicChat::Result> queryMessagesBefore(const GroupMe::Message &beforeMessage, unsigned int messageCount = DEFAULT_QUERY_LENGTH) override;

            pplx::task<BasicChat::Result> queryMessagesAfter(const GroupMe::Message &afterMessage, unsigned int messageCount = DEFAULT_QUERY_LENGTH) override;

            pplx::task<BasicChat::Result> queryMessagesSince(const GroupMe::Message &sinceMessage, unsigned int messageCount = DEFAULT_QUERY_LENGTH) override;

            web::uri m_endpointUrl;

            web::http::http_request m_request;

            web::http::client::http_client m_client;

            std::string m_accessToken;

            std::string m_groupName;

            std::string m_groupDescription;

            std::string m_groupImageUrl;

            std::string m_groupShareUrl;

            GroupChat::VisibilityType m_groupVisibility;

            unsigned long long int m_updatedAt;

            std::shared_ptr<GroupMe::User> m_groupCreator;

            std::map<std::shared_ptr<GroupMe::User>, std::string, GroupMe::UserCompare> m_memberGroupId;

            GroupMe::UserSet m_groupMembers;
    };
}
