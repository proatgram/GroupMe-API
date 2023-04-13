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

#include "Message.h"

using namespace GroupMe;

Message::Message(const std::shared_ptr<GroupMe::User> &sender, const std::string& message, const std::string& GUID, const Type &type) :
    m_guid(GUID),
    m_createdAt(),
    m_pinned(),
    m_pinnedAt(),
    m_sender(sender),
    m_text(message),
    m_type(type)
{

}

Message::Message(const std::string& message, const std::string& GUID, const Type &type) :
    m_guid(GUID),
    m_createdAt(),
    m_pinned(),
    m_pinnedAt(),
    m_text(message),
    m_type(type)
{

}

Message::Message(const std::shared_ptr<GroupMe::User> &sender, const std::string& GUID, const Type &type) :
    m_guid(GUID),
    m_sender(sender),
    m_createdAt(),
    m_pinned(),
    m_pinnedAt(),
    m_type(type)
{

}

Message::Message(const std::string& GUID, const Type &type) :
    m_guid(GUID),
    m_createdAt(),
    m_pinned(),
    m_pinnedAt(),
    m_type(type)
{

}

Message Message::createFromJson(const nlohmann::json &json, const UserSet &users) {
    Message message;

    message.setID(json["id"]);
    message.setCreatedAt(json["created_at"]);

    if (!json["text"].is_null()) {
        message.attach(json["text"]);
    }

    if (json["system"]) {
        message.setSender(std::make_shared<User>("system", "GroupMe", "", "", "", ""));
        message.setType(Type::System);
    }
    else {
        message.setType(Type::User);
        UserSet::iterator sender = users.find(json["sender_id"]);

        if (sender != users.cend()) {
            message.setSender(*sender);
        }
        else {
            message.setSender(std::make_shared<User>(json["sender_id"], json["name"], json["avatar_url"], "", "", ""));
        }

        // Parses favorited by data
        for (const auto& user : json["favorited_by"]) {
            UserSet::iterator itUser = users.find(user);
            if (itUser != users.cend()) {
                message.addFavorited(*itUser);
            }
        }

        // Parses attachment data from the Json and constructs attachments accordingly
        for (const auto& attachment : json["attachments"]) {
            if (attachment["type"] == "image") {
                message.attach(Attachment(web::uri(std::string(attachment["url"])), Attachment::Types::Picture));
            }
            else if (attachment["type"] == "file") {
                message.attach(Attachment(std::string(attachment["file_id"]), Attachment::Types::File));
            }
            else if (attachment["type"] == "video") {
                message.attach(Attachment(std::string(attachment["url"]), Attachment::Types::Video));
            }
        }
    }
    return message;
}

std::string Message::getID() const {
    return m_id;
}

void Message::setID(const std::string& id) {
    m_id = id;
}

unsigned int Message::getCreatedAt() const {
    return m_createdAt;
}

void Message::setCreatedAt(unsigned int createdAt) {
    m_createdAt = createdAt;
}

std::string Message::getText() const {
    return m_text;
}

void Message::attach(const std::string& messageText) {
    m_text = messageText;
}

void Message::attach(const GroupMe::Attachment& messageAttachment) {
    m_attachments.push_back(messageAttachment);
}

const std::vector<GroupMe::Attachment>& Message::getAttachments() const {
    return m_attachments;
}

std::shared_ptr<const GroupMe::User> Message::getSender() const {
    return std::const_pointer_cast<const GroupMe::User>(m_sender);
}

void Message::setSender(const std::shared_ptr<GroupMe::User>& sender) {
    m_sender = sender;
}

void Message::addFavorited(const std::shared_ptr<GroupMe::User> &favoritedBy) {
    m_favoritedBy.push_back(favoritedBy);
}

Message::Type Message::getType() const {
    return m_type;
}

void Message::setType(const Message::Type &type) {
    m_type = type;
}
