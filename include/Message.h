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
#include <memory>
#include <nlohmann/json.hpp>

#include "Attachment.h"
#include "User.h"
#include "UserSet.hpp"

namespace GroupMe {
    /**
     * This class holds message data that can be sent.
     * You can attatch different attatchments to it, and
     * attatch text to it.
     *
     * @brief Class to represent messages
     *
     */
    class Message {
        public:

            enum class Type {
                User,
                System
            };

            /**
             * This constructor should be used when you want to initialize a message
             * with some text. This constructor also has an optional parameter that
             * is used as the GUID for the message. If it is not set here it will be
             * generated later.
             *
             * @brief Constructs a new `GroupMe::Message` object
             *
             * @param sender The user who sent this message as a `GroupMe::User` wrapped in a `std::shared_ptr`
             * @param message Message text you want to send
             * @param GUID The messages unique GUID
             * @param type The type of message
             *
             */
            Message(const std::shared_ptr<GroupMe::User> &sender, const std::string& message, const std::string& GUID = "", const Message::Type &type = Message::Type::User);

            /**
             * This constructor should be used when you want to initialize a message
             * with some text only, without specifying a sender. This constructor also
             * has an optional parameter that is used as the GUID for the message. If it
             * is not set here it will be generated later.
             *
             * @brief Constructs a new `GroupMe::Message` object
             *
             * @param message Message text you want to send
             * @param GUID The message unique GUID
             * @param type The type of message
             *
             */
            explicit Message(const std::string& message, const std::string& GUID = "", const Message::Type &type = Message::Type::User);

            /**
             * This constructor should be used when you want to initialize a message
             * with only a sender. This constructor also has an optional parameter that
             * is used as the GUID for the message. If it is not set here it will be
             * generated later.
             *
             * @brief Constructs a new `GroupMe::Message` object
             *
             * @param sender The user who sent this message as a `GroupMe::User` wrapped in a `std::shared_ptr`
             * @param GUID The message unique GUID
             * @param type The type of message
             *
             */
            explicit Message(const std::shared_ptr<GroupMe::User> &sender, const std::string& GUID = "", const Message::Type &type = Message::Type::User);

            /**
             * This constructor should be used when you want to initialize a message
             * with text and other stuff later. This constructor also has an optional
             * parameter that is used as the GUID for the message. If it is not set here
             * then it will be generated later.
             *
             * @brief Constructs a new `GroupMe::Message` object
             *
             * @param GUID The messages unique GUID
             * @param type The type of message
             *
             */
            explicit Message(const std::string& GUID = "", const Message::Type &type = Message::Type::User);

            /**
             * @brief Constructs a `GroupMe::Message` from a json.
             *
             * @param json A json that holds message data
             *
             * @param users The users that are the group that the message was sent in
             *
             * @return Message a new message
             *
             */
            static Message createFromJson(const nlohmann::json &json, const GroupMe::UserSet &user);

            /**
             * @brief Constructs an `nlohmann::json` object from this message
             *
             */
            nlohmann::json toJson() const;

            /**
             * @brief Gets the ID of the message
             *
             * @return std::string
             *
             */
            std::string getID() const;

            /**
             * @brief Sets the ID of the message
             *
             * @param messageID The ID to set
             *
             */
            void setID(const std::string& messageID);

            /**
             * @brief Gets when the message was created at
             *
             * @return unsigned long long int
             *
             */
            unsigned long long int getCreatedAt() const;

            /**
             * @brief Sets when the message was created at
             *
             * @param createdAt The new time to set
             *
             */
            void setCreatedAt(unsigned long long int createdAt);

            /**
             * @brief Gets the text attached to the message
             *
             * @return std::string
             *
             */
            std::string getText() const;

            /**
             * @brief Attaches text to the message
             *
             * @param messageText The new text to add
             *
             */
            void attach(const std::string& messageText);

            /**
             * @brief Attaches an attatchment to the message
             *
             * @param messageAttachment The new attatchment to add
             *
             */
            void attach(const std::shared_ptr<GroupMe::Attachment>& messageAttachment);

            /**
             * @brief Gets the attachments that have been attached to the message
             *
             * @return const std::vector<std::shared_ptr<GroupMe::Attatchment>>&
             *
             */
            const std::vector<std::shared_ptr<GroupMe::Attachment>>& getAttachments() const;

            /**
             * @brief Gets the user info for the sender of the message
             *
             * @return std::shared_ptr<const GroupMe::User>
             *
             */
            std::shared_ptr<const GroupMe::User> getSender() const;

            /**
             * @brief Sets the user info for the sender of the message
             *
             * @param sender The sender of the message
             *
             */
            void setSender(const std::shared_ptr<GroupMe::User>& sender);

            /**
             * @brief Adds a person to the favorited by list
             *
             * @param favoritedBy The user who favorited the message
             *
             */
            void addFavorited(const std::shared_ptr<GroupMe::User> &favoritedBy);

            /**
             * @brief Gets the type of message
             *
             *  - System Message
             *    - A user has left
             *  - A User Message
             *    - Normal message
             *
             *  @return Message::Type The type
             *
             */
            Message::Type getType() const;

            /**
             * @brief Sets the type of message
             *
             *  - System Message
             *    - A user has left
             *  - A User Message
             *    - Normal message
             *
             *  @param type The type
             *
             * @return void
             *
             */
            void setType(const Message::Type &type);

            /**
             * @brief Gets the date and time the message was pinned at
             *
             * @return unsigned long long int The date the message was pinned at
             *
             */
            unsigned long long int getPinnedAt() const;

            /**
             * @brief Sets the date and time the message was pinned at
             *
             * @param pinnedAt The date the message was pinned at
             *
             * @return void
             *
             */
            void setPinnedAt(unsigned long long int pinnedAt);

        private:
            std::string m_id;

            std::string m_guid;

            unsigned long long int m_createdAt;

            bool m_pinned;

            unsigned long long int m_pinnedAt;

            std::shared_ptr<GroupMe::User> m_pinnedBy;

            std::shared_ptr<GroupMe::User> m_sender;

            std::vector<std::shared_ptr<GroupMe::Attachment>> m_attachments;

            std::vector<std::shared_ptr<GroupMe::User>> m_favoritedBy;

            std::string m_text;

            Message::Type m_type;
    };

    struct MessageCompare {
        using is_transparent = void;

        inline size_t operator()(const Message &first, const Message &second) const {
            return static_cast<size_t>(first.getCreatedAt() < second.getCreatedAt());
        }

        inline size_t operator()(const Message &first, unsigned long long int createdAt) const {
            return static_cast<size_t>(first.getCreatedAt() < createdAt);
        }

        inline size_t operator()(unsigned long long int createdAt, const Message &message) const {
            return static_cast<size_t>(message.getCreatedAt() < createdAt);
        }
    };

    using MessageSet = std::set<Message, MessageCompare, std::allocator<Message>>;
}
