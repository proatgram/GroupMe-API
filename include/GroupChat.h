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

#include <variant>

#include "BasicGroupChat.h"
#include "SubGroupChat.h"

namespace GroupMe {
    /**
     * A class to represent GroupMe group chats
     *
     */
    class GroupChat final : public BasicGroupChat {
        public:
            /**
             * @brief The permissions for editing the group settings
             *
             * - Private
             *      Anyone in the group can edit the permissions
             * - Closed
             *      Only Admins can edit the group settings
             */
            enum class GroupPermissions {
                Closed,
                Private
            };

            /**
             * @brief The permissions for deleting messages in the group
             *
             */
            enum class DeletionPermissions {
                AdminOnly,
                Both
            };
            
            /**
             * @brief The type of the groups
             *
             * This isn't really mentioned in any requests or documentation,
             * it's just sent in a request, so we support it here. No request
             * seems to change this however.
             *
             */
            enum class GroupType {
                Public,
                Private
            };

            /**
             * @brief The visibility of the group
             *
             */
            enum class Visibility {
                Hidden,
                Searchable
            };

            enum class Joinability {
                ApprovedMembers,
                Anyone
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
             * @param visibility The visibility of the group
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
             */
           GroupChat(const std::string &token, const std::string &groupId, const std::string &name, Visibility visibility, const std::string &description, const web::uri &imageUrl, const std::shared_ptr<User> &creator, unsigned long long int createdAt, unsigned long long int updatedAt);

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

            GroupChat(const GroupChat &other) = default;

            GroupChat(GroupChat &&other) noexcept = default;

            GroupChat& operator=(const GroupChat &other) = default;

            GroupChat& operator=(GroupChat &&other) noexcept = default;

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
            void setVisibility(GroupMe::GroupChat::Visibility visibility);

            /**
             * @brief Gets the creator of the group
             *
             * @return std::shared_ptr<const GroupMe::User>
             *
             */
            std::shared_ptr<const GroupMe::User> getCreator() const;

            /**
             * @brief Gets the visibility for the group
             *
             * @return `GroupChat::Visibility` The visibility of the group
             *
             */
            GroupMe::GroupChat::Visibility getVisibility() const;

            /**
             * @brief Sets the Group Permissions for the group
             *
             * @param permissions
             *
             * @return void
             *
             */
            void setGroupPermissions(const GroupChat::GroupPermissions &permissions);

            /**
             * @brief Gets the Group Permissions for the group
             *
             * @return GroupChat::GroupPermissions
             *
             */
            GroupChat::GroupPermissions getGroupPermissions() const;

            /**
             * @brief Sets the Group Deletion Permissions for messages
             *
             * @param permissions
             *
             * @return void
             *
             */
            void setDeletionPermissions(const GroupChat::DeletionPermissions &permissions);

            /**
             * @brief Gets the Group Deletion Permissions for messages
             *
             * @return GroupChat::DeletionPermissions
             *
             */
            GroupChat::DeletionPermissions getDeletionPermissions() const;

            /**
             * @brief Sets who can join the group
             *
             * @param memberType
             *
             * @return void
             *
             */
            void setJoinability(const GroupChat::Joinability &memberType);

            /**
             * @brief Gets who can join the group
             *
             * @return GroupChat::Joinability
             *
             */
            GroupChat::Joinability getJoinability() const;

            /**
             * @brief Changes if the group requires approval
             * from an admin to join the group.
             *
             * @param requiresApproval
             *
             * @return void
             *
             */
            void setRequiresApproval(bool requiresApproval);

            /**
             * @brief Gets the approval mode
             *
             * @return bool
             *
             */
            bool getRequiresApproval() const;

            /**
             * @brief Sets if it should show a join
             * question to new members requesting to
             * join the group
             *
             * @param showJoinQuestion
             *
             * @return void
             *
             */
            void setShowJoinQuestion(bool showJoinQuestion);

            /**
             * @brief Gets if it should show a join
             * question to new members requesting to
             * join the group
             *
             * @return bool
             *
             */
            bool getShowJoinQuestion() const;

            /**
             * @brief Sets a join question
             *
             * @param question
             *
             * @return void
             *
             */
            void setJoinQuestion(const std::string &question);

            /**
             * @brief Gets the join question
             *
             * @return const std::list<std::string>&
             *
             */
            const std::string& getJoinQuestion() const;

            /**
             * @brief Gets the SubGroups of this GroupChat
             *
             * @return `std::list<SubGroupChat>`
             *
             */
            const std::list<std::unique_ptr<SubGroupChat>>& getSubGroups() const;

            /**
             * @brief Creates a SubGroup for this GroupChat
             *
             * @param topic The name for the SubGroup (known on the API side as a `topic`)
             *
             * @param description The description for the SubGroup
             *
             * @return pplx::task<BasicChat::Result> The return of the concurrent task will be the result of the operation
             *
             */
            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> createSubGroup(const std::string &topic, const std::string &description = "");

            /**
             * @brief Deletes a SubGroup for this GroupChat
             *
             * @param chatId The ID of the SubGroupChat
             *
             * @return pplx::task<BasicChat::Result> The return of the concurrent task will be the result of the operation
             *
             */
            [[nodiscard("Manage the task")]]
            pplx::task<std::variant<std::list<std::unique_ptr<GroupMe::SubGroupChat>>::iterator, BasicChat::Result>> 
            destroySubGroup(const std::string &subGroupId);

            /**
             * @brief Deletes a SubGroup for this GroupChat
             *
             * @param subGroupChat The SubGroupChat to delete
             *
             * @return pplx::task<BasicChat::Result> The return of the concurrent task will be the result of the operation
             *
             */
            [[nodiscard("Manage the task")]]
            pplx::task<std::variant<std::list<std::unique_ptr<GroupMe::SubGroupChat>>::iterator, BasicChat::Result>> 
            destroySubGroup(const GroupMe::SubGroupChat &subGroupChat);

            /**
             * @brief Deletes a SubGroup for this GroupChat
             *
             * @param subGroupChat The SubGroupChat to delete
             *
             * @return pplx::task<BasicChat::Result> The return of the concurrent task will be the result of the operation
             *
             */
            [[nodiscard("Manage the task")]]
            pplx::task<std::variant<std::list<std::unique_ptr<GroupMe::SubGroupChat>>::iterator, BasicChat::Result>> 
            destroySubGroup(const std::unique_ptr<GroupMe::SubGroupChat> &subGroupChat);

            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> update() final;

            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> queryMessages(unsigned int messageCount = DEFAULT_QUERY_LENGTH) final;

        private:
            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> queryMessagesBefore(const GroupMe::Message &beforeMessage, unsigned int messageCount = DEFAULT_QUERY_LENGTH) final;

            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> queryMessagesAfter(const GroupMe::Message &afterMessage, unsigned int messageCount = DEFAULT_QUERY_LENGTH) final;

            [[nodiscard("Manage the task")]]
            pplx::task<BasicChat::Result> queryMessagesSince(const GroupMe::Message &sinceMessage, unsigned int messageCount = DEFAULT_QUERY_LENGTH) final;

            bool m_requiresApproval;

            bool m_showJoinQuestion;

            std::string m_joinQuestion;

            GroupMe::GroupChat::GroupType m_groupType;

            GroupMe::GroupChat::Visibility m_groupVisibility;

            GroupMe::GroupChat::DeletionPermissions m_messageDeletionMode;

            GroupMe::GroupChat::GroupPermissions m_groupPermissions;

            GroupMe::GroupChat::Joinability m_groupJoinability;

            std::shared_ptr<GroupMe::User> m_groupCreator;

            std::string m_groupShareUrl;

            std::list<std::unique_ptr<SubGroupChat>> m_subgroups;
    };
}
