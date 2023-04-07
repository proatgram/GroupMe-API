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
#include <cpprest/http_client.h>
#include <cpprest/uri.h>

#include "UserSet.hpp"
#include "Message.h"
#include "Picture.h"
#include "User.h"

namespace GroupMe {
    /**
     * A class to represent GroupMe group chats
     *
     * @brief A class to represent GroupMe group chats
     *
     */
    class GroupChat {
        public:
            /**
             * The visibility / joinability of the Groups.
             *
             * @brief The visibility / joinability of the groups
             *
             */
            enum class VisibilityType {
                Public,
                Private
            };

            /**
             * Represents the query type for message querying
             * 
             *  - Before Message
             *    + Gives messages returned before a given message
             *  - After Message
             *    + Gives message created immediately after the given message
             *  - Since Message
             *    + Returns _**most recent**_ messages created after the given message            *
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
             * The default message query amount if not specified
             * in the web request is 20
             *
             * @brief Default query amount
             *
             */
            static constexpr int DEFAULT_QUERY_LENGTH = 20;

            /**
             * @brief Constructs a new `GroupMe::GroupChat` object
             *
             */
            GroupChat();

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
             * @brief Sets the group name if you have the permission
             *
             * @param name The new name to set
             *
             * @return bool true if it succeeded and false if it didn't
             *
             */
            bool setGroupName(const std::string &name);

            /**
             * @brief Gets the groups description
             *
             * @return std::string The description
             *
             */
            std::string getGroupDescription() const;

            /**
             * Sets the group description if you have the permission
             *
             * In order to be able to do this you need to have the
             * proper permissions on the API side.
             *
             * @brief Sets the group description
             *
             * @return bool true if it succeeded and false if it didn't
             *
             */
            bool setGroupDescription(const std::string &description);

            /**
             * @brief Gets the groups image URL
             *
             * @return std::string The URL to the image
             *
             */
            std::string getGroupImageUrl() const;

            /**
             * Sets the groups image to the image if you have the permission
             *
             * In order to be able to do this you need to have the
             * proper permissions on the API side.
             *
             * @brief Sets the groups image
             *
             * @param image A `GroupMe::Picture` holding an image to set
             *
             * @return bool true of it succeeded and false if it didn't
             *
             */
            bool setGroupImage(const GroupMe::Picture &image);

            /**
             * Sets the groups image to the image based on a url if
             * you have the permission
             *
             * In order to be able to do this you need to have the
             * proper permissions on the API side.
             *
             * @brief Sets the groups image
             *
             * @param url A `web::uri` holding an image url to set
             *
             * @return bool true of it succeeded and false if it didn't
             *
             */
            bool setGroupImage(const web::uri &url);

            /**
             * @brief Gets the creators user ID 
             *
             * @return std::string The user ID for the creator of the group
             *
             */
            std::string getCreatorUserId() const;

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
             * @return `GroupChat::VisibilityType` The visibility of the group
             *
             */
            GroupChat::VisibilityType getGroupVisibility() const;

            /**
             * Sets the groups visibility / joinability
             * if you have the permission
             *
             * In order to be able to do this you need to have the
             * proper permissions on the API side.
             *
             * @brief Sets the visibility
             *
             * @param visibility The new visibility of the group to set
             *
             * @return bool true if it succeeded and false if it didn't
             *
             */
            bool setGroupVisibility(GroupChat::VisibilityType visibility);

            /**
             * Gets the date the group was created at
             *
             * Not really sure the format as not much about
             * the dates are given in the documentation
             *
             * @brief Gets the date and time the group was created at
             *
             * @return unsigned long long int The date the group was created
             *
             */
            unsigned long long int getCreatedAt() const;

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
             * @brief Adds members to the group
             *
             * @param user The user to add to the group as a `GroupMe::User`
             *
             * @return bool true if succeeded and false if not
             *
             */
            bool addGroupMember(const GroupMe::User &user);

            /**
             * Query messages in the group
             *
             * There are three ways you can query messages in a group
             *
             *  - Before Message
             *    + Gives messages returned before a given message
             *  - After Message
             *    + Gives message created immediately after the given message
             *  - Since Message
             *    + Returns _**most recent**_ messages created after the given message
             *
             * There is also a limit to how many it will give you, the default the API sets is 20
             *
             * @brief Query messages in the group
             *
             * @param referenceMessage The message you want to use as the reference to query
             *
             * @param queryType The query type you would like to use as a `GroupMe::GroupChat::QueryType`
             *
             * @param messageCount The count of messages that will be queried. The default is 20 if none is supplied.
             *
             * @return bool true if it succeeded and false if it didn't
             *
             */
            bool queryMessages(const GroupMe::Message &referenceMessage, GroupMe::GroupChat::QueryType queryType, unsigned int messageCount = DEFAULT_QUERY_LENGTH);

        private:
            bool queryMessagesBefore(const GroupMe::Message &beforeMessage, unsigned int messageCount = DEFAULT_QUERY_LENGTH);

            bool queryMessagesAfter(const GroupMe::Message &afterMessage, unsigned int messageCount = DEFAULT_QUERY_LENGTH);

            bool queryMessagesSince(const GroupMe::Message &sinceMessage, unsigned int messageCount = DEFAULT_QUERY_LENGTH);

            std::string m_groupId;
            
            std::string m_groupName;

            std::string m_description;

            std::string m_imageUrl;

            std::string m_creatorUserId;

            std::string m_shareUrl;

            GroupChat::VisibilityType m_visibilityType;

            unsigned long long int m_createdAt;

            unsigned long long int m_updatedAt;

            GroupMe::UserSet m_members;

            std::vector<GroupMe::Message> m_messages;
            
    };
}
