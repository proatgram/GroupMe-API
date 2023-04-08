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

#include "GroupChat.h"
#include <cpprest/uri_builder.h>
#include <libavutil/pixfmt.h>

using namespace GroupMe;

GroupChat::GroupChat(const std::string &token, const std::string &groupId, const std::string &name, VisibilityType type, const std::string &description, const web::uri &imageUrl, const std::shared_ptr<User> &creator, unsigned long long int createdAt, unsigned long long int updatedAt, const web::uri &shareUrl) :
    Chat(createdAt),
    m_accessToken(token),
    m_groupId(groupId),
    m_groupName(name),
    m_groupDescription(description),
    m_groupImageUrl(imageUrl.to_string()),
    m_groupShareUrl(shareUrl.to_string()),
    m_groupVisibility(type),
    m_updatedAt(updatedAt),
    m_groupCreator(creator),
    m_endpointUrl("https://api.groupme.com/v3/groups"),
    m_client(m_endpointUrl)
{

}

GroupChat::GroupChat(const std::string &token, const std::string &groupId) :
    m_accessToken(token),
    m_groupId(groupId),
    m_endpointUrl("https://api.groupme.com/v3/groups"),
    m_client(m_endpointUrl)
{
    m_request.set_method(web::http::methods::GET);
    m_request.headers().add("X-Access-Token", m_accessToken);

    web::uri_builder builder(m_endpointUrl);

    builder.append_path(m_groupId);

    m_client = web::http::client::http_client(builder.to_uri());

    m_client.request(m_request).then([this](const web::http::http_response &response) -> void {
        auto group = nlohmann::json::parse(response.extract_string(true).get())["response"];

        m_groupId = group["id"];

        m_groupName = group["name"];
        
        if (group["type"] == "private") {
            m_groupVisibility = VisibilityType::Private;
        }
        else if (group["type"] == "public") {
            m_groupVisibility = VisibilityType::Public;
        }

        m_groupDescription = group["description"];

        m_createdAt = group["created_at"];
        m_updatedAt = group["updated_at"];

        
        for (auto &member : group["members"]) {
            m_groupMembers.insert(std::make_shared<User>(
                member["user_id"],
                member["nickname"],
                member["image_url"],
                "",
                "",
                ""
            ));
        }

        m_groupShareUrl = group["share_url"];

        m_groupCreator = *m_groupMembers.find(group["creator_user_id"]);
    }).wait();
}

std::string GroupChat::getGroupId() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupId;
}

std::string GroupChat::getGroupName() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupName;
}

void GroupChat::setGroupName(const std::string &name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_groupName = name;
}

std::string GroupChat::getGroupDescription() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupDescription;
}

void GroupChat::setGroupDescription(const std::string &description) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_groupDescription = description;
}

std::string GroupChat::getGroupImageUrl() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupImageUrl;
}

void GroupChat::setGroupImage(GroupMe::Picture &image) {
    m_task = pplx::task<void>([&image, this]() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_groupImageUrl = image.upload().get();
    });
}

void GroupChat::setGroupImage(const web::uri &url) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_groupImageUrl = url.to_string();
}

std::shared_ptr<const GroupMe::User> GroupChat::getCreator() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return std::const_pointer_cast<const User>(m_groupCreator);
}

std::string GroupChat::getGroupShareUrl() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupShareUrl;
}

GroupChat::VisibilityType GroupChat::getGroupVisibility() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupVisibility;
}

void GroupChat::setGroupVisibility(GroupMe::GroupChat::VisibilityType visibility) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_groupVisibility = visibility;
}

unsigned long long int GroupChat::getUpdatedAt() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_updatedAt;
}

const GroupMe::UserSet& GroupChat::getGroupMembers() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupMembers;
}

bool GroupChat::addGroupMember(const GroupMe::User &user) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto [pos, insert] = m_groupMembers.insert(std::make_shared<User>(user));
    m_task = pplx::task<void>([this, &user]() {
        m_request.set_method(web::http::methods::POST);
        m_request.headers().add("X-Access-Token", m_accessToken);
        
        web::uri_builder builder(m_endpointUrl);
        builder.append_path(m_groupId);
        builder.append_path("members/add");

        m_request.set_request_uri(builder.to_uri());

        nlohmann::json body;

        body["members"][0]["nickname"] = user.getNickname();
        body["members"][0]["user_id"] = user.getID();
        body["members"][0]["guid"] = user.getGUID();
        body["members"][0]["phone_number"] = user.getPhoneNumber();
        body["members"][0]["email"] = user.getEmail();

        m_request.set_body(body.dump());
    });
}

bool GroupChat::update() const {
    std::lock_guard<std::mutex> lock(m_mutex);

}

bool GroupChat::queryMessages(const Message &referenceMessage, Chat::QueryType queryType, unsigned int messageCount) {
    switch (queryType) {
        case Chat::QueryType::Before: {
            return queryMessagesBefore(referenceMessage, messageCount);
        }
        case Chat::QueryType::After: {
            return queryMessagesAfter(referenceMessage, messageCount);
        }
        case Chat::QueryType::Since: {
            return queryMessagesSince(referenceMessage, messageCount);
        }
    }
}

bool GroupChat::queryMessagesBefore(const Message &beforeMessage, unsigned int messageCount) {

}

bool GroupChat::queryMessagesAfter(const Message &afterMessage, unsigned int messageCount) {

}

bool GroupChat::queryMessagesSince(const Message &sinceMessage, unsigned int messageCount) {

}
