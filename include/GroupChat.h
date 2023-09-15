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
    class GroupChat final : public BasicGroupChat {
        public:

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

            GroupChat(const BasicGroupChat &other) = delete;

            GroupChat(BasicGroupChat &&other) = delete;

            GroupChat& operator=(const BasicGroupChat &other) = delete;

            GroupChat& operator=(BasicGroupChat &&other) = delete;

             ~GroupChat() override = default;

            /**
             * @brief Gets the groups share URL
             *
             * @return std::string the share URL for the group
             *
             */
            std::string getShareUrl() const;

            /**
             * @brief Adds members to the group
             *
             * @param users The users to add to the group as a `std::vector<GroupMe::User>`
             *
             * @return pplx::task<BasicChat::Result> The return of the concurrent task will be the result of the operation
             *
             * The return should not be discarded. Make sure the task being run is finished before program termination.
             *
             */
            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> addMembers(const std::vector<User> &users);

            /**
             * @brief Adds members to the group
             *
             * @param users The user to add to the group as encapsulated in a `std::vector<std::shared_ptr<GroupMe::User>>`
             *
             * @return pplx::task<BasicChat::Result> The return of the concurrent task will be the result of the operation
             *
             * The return should not be discarded. Make sure the task being run is finished before program termination.
             *
             */
            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> addMembers(const std::vector<std::shared_ptr<User>> &users);

            /**
             * @brief Adds members to the group
             *
             * @param users The users to add to the group as a `GroupMe::User`
             *
             * @return pplx::task<BasicChat::Result> The return of the concurrent task will be the result of the operation
             *
             * The return should not be discarded. Make sure the task being run is finished before program termination.
             *
             */
            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> addMembers(const User &users);

            /**
             * @brief Adds members to the group
             *
             * @param users The user to add to the group as encapsulated in a `std::shared_ptr<GroupMe::User>`
             *
             * @return pplx::task<BasicChat::Result> The return of the concurrent task will be the result of the operation
             *
             * The return should not be discarded. Make sure the task being run is finished before program termination.
             *
             */
            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> addMembers(const std::shared_ptr<User> &users);

            /**
             * @brief Removes a member from the group
             *
             * @param user The user to remvoe from the group as a `GroupMe::User`
             *
             * @return pplx::task<BasicChat::Result> The return of the concurrent task will be the result of the operation
             *
             * The return should not be discarded. Make sure the task being run is finished before program termination.
             *
             */
            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> removeMember(const GroupMe::User &user);

            /**
             * @brief Removes a member from the group
             *
             * @param user The user to remove from the group encapsulated in a `std::shared_ptr<GroupMe::User>`
             *
             * @return pplx::task<BasicChat::Result> The return of the concurrent task will be the result of the operation
             *
             * The return should not be discarded. Make sure the task being run is finished before program termination.
             *
             */
            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> removeMember(const std::shared_ptr<GroupMe::User> &user);

            /**
             * @brief Disbands / destroys a group
             *
             * This action is only available to the group creator
             *
             * @return pplx::task<BasicChat::Result> The return of the concurrent task will be the result of the operation
             *
             * The return should not be discarded. Make sure the task being run is finished before program termination.
             *
             */
            [[nodiscard("Manage the task")]]
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
             * The return should not be discarded. Make sure the task being run is finished before program termination.
             *
             */
            [[nodiscard("Manage the task")]]
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
             * The return should not be discarded. Make sure the task being run is finished before program termination.
             *
             */
            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> changeGroupOwner(const std::shared_ptr<GroupMe::User> &user);

            /**
             * @brief Sets the groups visibility / joinability
             *
             * @param visibility The new visibility of the group to set
             *
             * @return void
             *
             */
            void setVisibility(GroupMe::GroupChat::VisibilityType visibility);

            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> queryMessages(unsigned int messageCount = DEFAULT_QUERY_LENGTH) final;

        private:
            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> queryMessagesBefore(const GroupMe::Message &beforeMessage, unsigned int messageCount = DEFAULT_QUERY_LENGTH) final;

            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> queryMessagesAfter(const GroupMe::Message &afterMessage, unsigned int messageCount = DEFAULT_QUERY_LENGTH) final;

            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> queryMessagesSince(const GroupMe::Message &sinceMessage, unsigned int messageCount = DEFAULT_QUERY_LENGTH) final;

            static constexpr std::string_view GROUP_ENDPOINT_QUERY = "groups";

            // std::list<std::shared_ptr<SubGroupChat>> m_subgroups;
    };
}
