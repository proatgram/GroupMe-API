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

#include "Picture.h"
#include "Video.h"
#include "File.h"

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

// TODO: There are events associated with system messages, for example
//  "event": {
//      "type": "membership.notifications.exited",
//      "data": {
//          "removed_user": {
//              "id": 71885510,
//              "nickname": "Brooklynn"
//          }
//      }
//  }
//
//  These are not well documented though
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

        if (!json["pinned_at"].is_null()) {
            message.setPinnedAt(json["pinned_at"]);
            
        }

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
                message.attach(std::make_shared<Picture>(web::uri(std::string(attachment["url"]))));
            }
            else if (attachment["type"] == "file") {
                message.attach(std::make_shared<File>(std::string(attachment["file_id"])));
            }
            else if (attachment["type"] == "video") {
                message.attach(std::make_shared<Video>(std::string(attachment["url"]), std::string(attachment["preview_url"])));
            }
        }
    }
    return message;
}

nlohmann::json Message::toJson() const {
    nlohmann::json json;
    
    json["message"]["source_guid"] = m_guid;

    if (!m_text.empty()) {
        json["message"]["text"] = m_text;
    }

    for (int i = 0; i < m_attachments.size(); i++) {
        json["message"]["attachments"][i] = m_attachments.at(i)->toJson();
    }

    return json;
}

std::string Message::getID() const {
    return m_id;
}

void Message::setID(const std::string& id) {
    m_id = id;
}

unsigned long long int Message::getCreatedAt() const {
    return m_createdAt;
}

void Message::setCreatedAt(unsigned long long int createdAt) {
    m_createdAt = createdAt;
}

std::string Message::getText() const {
    return m_text;
}

void Message::attach(const std::string& messageText) {
    m_text = messageText;
}

void Message::attach(const std::shared_ptr<GroupMe::Attachment>& messageAttachment) {
    m_attachments.push_back(messageAttachment);
}

const std::vector<std::shared_ptr<GroupMe::Attachment>>& Message::getAttachments() const {
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

unsigned long long int Message::getPinnedAt() const {
    return m_pinnedAt;
}

void Message::setPinnedAt(unsigned long long int pinnedAt) {
    m_pinnedAt = pinnedAt;
}
