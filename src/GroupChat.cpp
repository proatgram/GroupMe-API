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
    BasicChat(groupId, createdAt),
    m_accessToken(token),
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
    BasicChat(groupId),
    m_accessToken(token),
    m_endpointUrl("https://api.groupme.com/v3/groups"),
    m_client(m_endpointUrl)
{
    m_request.set_method(web::http::methods::GET);
    m_request.headers().add("X-Access-Token", m_accessToken);

    web::uri_builder builder(m_endpointUrl);

    builder.append_path(m_chatId);

    m_client = web::http::client::http_client(builder.to_uri());

    m_client.request(m_request).then([this](const web::http::http_response &response) -> void {
        auto group = nlohmann::json::parse(response.extract_string(true).get())["response"];

        m_chatId = group["id"];

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
            std::shared_ptr<User> user = std::make_shared<User>(
                member["user_id"],
                member["nickname"],
                member["image_url"],
                "",
                "",
                ""
            );
            m_groupMembers.insert(user);
            m_memberGroupId.emplace(user, member["id"]);
        }

        m_groupShareUrl = group["share_url"];

        m_groupCreator = *m_groupMembers.find(group["creator_user_id"]);
    }).wait();
}

std::string GroupChat::getGroupId() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_chatId;
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

pplx::task<bool> GroupChat::addGroupMember(const GroupMe::User &user) {
    return pplx::task<bool>([this, user]() {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_request.set_method(web::http::methods::POST);
        m_request.headers().add("X-Access-Token", m_accessToken);
        
        web::uri_builder builder(m_endpointUrl);
        builder.append_path(m_chatId);
        builder.append_path("members/add");

        m_client = web::http::client::http_client(builder.to_uri());

        nlohmann::json body;

        body["members"][0]["nickname"] = user.getNickname();
        body["members"][0]["user_id"] = user.getID();
        body["members"][0]["guid"] = user.getGUID();
        body["members"][0]["phone_number"] = user.getPhoneNumber();
        body["members"][0]["email"] = user.getEmail();

        m_request.set_body(body.dump());

        bool returnValue = false;

        std::string resultId;

        m_client.request(m_request).then([this, &returnValue, &resultId](const web::http::http_response &response) -> void {
            if (response.status_code() != web::http::status_codes::OK) {
                return;
            }
            resultId = nlohmann::json::parse(response.extract_string(true).get())["response"]["result_id"];
            returnValue = true;
        }).wait();

        if (returnValue) {
            m_request.set_method(web::http::methods::GET);

            builder = web::http::uri_builder(m_endpointUrl);
            builder.append_path(m_chatId);
            builder.append_path("members");
            builder.append_path(resultId);

            m_client = web::http::client::http_client(builder.to_uri());

            bool ready = false;
            bool resultsNotAvalliable = false;
            std::shared_ptr<User> sharedUser = std::make_shared<User>(user);
            while (!ready) {
                m_client.request(m_request).then([this, &ready, &resultsNotAvalliable, &sharedUser](const web::http::http_response &response) -> void {
                    if (response.status_code() == web::http::status_codes::NotFound) {
                        ready = true;
                        resultsNotAvalliable = true;
                        return;
                    }
                    if (response.status_code() == web::http::status_codes::ServiceUnavailable) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                        return;
                    }
                    m_memberGroupId.emplace(sharedUser, nlohmann::json::parse(response.extract_string(true).get())["response"]["id"]);
                    const auto [it, insert] = m_groupMembers.insert(sharedUser);
                    ready = true;
                }).wait();
            }
        }
        return returnValue;
    });
}

pplx::task<bool> GroupChat::addGroupMember(const std::shared_ptr<GroupMe::User> &user) {
    return pplx::task<bool>([this, user]() -> bool {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_request.set_method(web::http::methods::POST);
        m_request.headers().add("X-Access-Token", m_accessToken);
        
        web::uri_builder builder(m_endpointUrl);
        builder.append_path(m_chatId);
        builder.append_path("members/add");

        m_client = web::http::client::http_client(builder.to_uri());

        nlohmann::json body;

        body["members"][0]["nickname"] = user->getNickname();
        body["members"][0]["user_id"] = user->getID();
        body["members"][0]["guid"] = user->getGUID();
        body["members"][0]["phone_number"] = user->getPhoneNumber();
        body["members"][0]["email"] = user->getEmail();

        m_request.set_body(body.dump());

        bool returnValue = false;

        std::string resultId;

        m_client.request(m_request).then([this, &returnValue, &resultId](const web::http::http_response &response) -> void {
            if (response.status_code() != web::http::status_codes::OK) {
                return;
            }
            resultId = nlohmann::json::parse(response.extract_string(true).get())["response"]["result_id"];
            returnValue = true;
        }).wait();

        if (returnValue) {
            m_request.set_method(web::http::methods::GET);

            builder = web::http::uri_builder(m_endpointUrl);
            builder.append_path(m_chatId);
            builder.append_path("members");
            builder.append_path(resultId);

            m_client = web::http::client::http_client(builder.to_uri());

            bool ready = false;
            bool resultsNotAvalliable = false;
            while (!ready) {
                m_client.request(m_request).then([this, &ready, &resultsNotAvalliable, &user](const web::http::http_response &response) -> void {
                    if (response.status_code() == web::http::status_codes::NotFound) {
                        ready = true;
                        resultsNotAvalliable = true;
                        return;
                    }
                    if (response.status_code() == web::http::status_codes::ServiceUnavailable) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                        return;
                    }
                    m_memberGroupId.emplace(user, nlohmann::json::parse(response.extract_string(true).get())["response"]["id"]);
                    const auto [it, insert] = m_groupMembers.insert(user);
                    ready = true;
                }).wait();
            }
        }
        return returnValue;
    });
}

pplx::task<bool> GroupChat::removeGroupMember(const GroupMe::User &user) {
    return pplx::task<bool>([this, user]() -> bool {
            std::lock_guard<std::mutex> lock(m_mutex);

            std::shared_ptr<User> sharedUser = std::make_shared<User>(user);
            
            web::http::uri_builder builder(m_endpointUrl);

            builder.append_path(m_chatId);
            builder.append_path("members");
            builder.append_path(m_memberGroupId.at(sharedUser));
            builder.append_path("remove");

            m_request.set_method(web::http::methods::POST);

            m_client = web::http::client::http_client(builder.to_uri());

            bool returnValue = false;

            m_client.request(m_request).then([this, &returnValue, &sharedUser](const web::http::http_response &response) -> void {
                if (response.status_code() != web::http::status_codes::OK) {
                    m_groupMembers.erase(sharedUser);
                    m_memberGroupId.erase(sharedUser);
                    return;
                }
                returnValue = true;
            }).wait();

            return returnValue;
    });
}

pplx::task<bool> GroupChat::removeGroupMember(const std::shared_ptr<GroupMe::User> &user) {
    return pplx::task<bool>([this, user]() -> bool {
            std::lock_guard<std::mutex> lock(m_mutex);

            web::http::uri_builder builder(m_endpointUrl);

            builder.append_path(m_chatId);
            builder.append_path("members");
            builder.append_path(m_memberGroupId.at(user));
            builder.append_path("remove");

            m_request.set_method(web::http::methods::POST);

            m_client = web::http::client::http_client(builder.to_uri());

            bool returnValue = false;

            m_client.request(m_request).then([this, &returnValue, &user](const web::http::http_response &response) -> void {
                if (response.status_code() != web::http::status_codes::OK) {
                    m_groupMembers.erase(user);
                    m_memberGroupId.erase(user);
                    return;
                }
                returnValue = true;
            }).wait();

            return returnValue;
    });
}

pplx::task<bool> GroupChat::destroyGroup() {
    return pplx::task<bool>([this]() -> bool {
            std::lock_guard<std::mutex> lock(m_mutex);

            web::http::uri_builder builder(m_endpointUrl);

            builder.append_path(m_chatId);
            builder.append_path("destroy");

            m_request.set_method(web::http::methods::POST);

            m_client = web::http::client::http_client(builder.to_uri());

            bool returnValue = false;

            m_client.request(m_request).then([this, &returnValue](const web::http::http_response &response) -> void {
                if (response.status_code() != web::http::status_codes::OK) {
                    return;
                }
                returnValue = true;
            }).wait();
            return returnValue;
    });
}

pplx::task<bool> GroupChat::update() {
    return pplx::task<bool>([this]() {
        std::lock_guard<std::mutex> lock(m_mutex);
        nlohmann::json json;
        json["name"] = m_groupName;

        json["description"] = m_groupDescription;

        json["image_url"] = m_groupImageUrl;

        json["share"] = true;

        m_request.set_method(web::http::methods::POST);

        web::uri_builder builder(m_endpointUrl);

        builder.append_path(m_chatId);
        builder.append_path("update");

        m_client = web::http::client::http_client(builder.to_uri());

        m_request.set_body(json.dump());

        bool returnValue = false;

        m_client.request(m_request).then([this, &returnValue](const web::http::http_response &response) -> void {
            if (response.status_code() != web::http::status_codes::OK) {
                return;
            }
            returnValue = true;

            auto group = nlohmann::json::parse(response.extract_string(true).get())["response"];
                
            m_chatId = group["id"];

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

            m_groupShareUrl = group["share_url"];
            for (auto &member : group["members"]) {
                std::shared_ptr<User> user = std::make_shared<User>(
                    member["user_id"],
                    member["nickname"],
                    member["image_url"],
                    "",
                    "",
                    ""
                );
                if (const auto [it, insert] = m_groupMembers.insert(user); insert) {
                    m_memberGroupId.emplace(user, member["id"]);
                }
            }
            m_groupCreator = *m_groupMembers.find(group["creator_user_id"]);
        }).wait();
        return returnValue;
    });
}

pplx::task<bool> GroupChat::queryMessages(const Message &referenceMessage, BasicChat::QueryType queryType, unsigned int messageCount) {
    m_request.set_method(web::http::methods::GET);

    web::http::uri_builder builder(m_endpointUrl);
    
    switch (queryType) {
        case BasicChat::QueryType::Before: {
            return queryMessagesBefore(referenceMessage, messageCount);
        }
        case BasicChat::QueryType::After: {
            return queryMessagesAfter(referenceMessage, messageCount);
        }
        case BasicChat::QueryType::Since: {
            return queryMessagesSince(referenceMessage, messageCount);
        }
        default: {
            return pplx::task<bool>([]() -> bool { return false; });
        }
    }
}

pplx::task<bool> GroupChat::queryMessages(unsigned int messageCount) {
    web::http::uri_builder builder(m_endpointUrl);

    builder.append_path(m_chatId);
    builder.append_path("messages");
    
    builder.append_query("limit", messageCount);

    m_client = web::http::client::http_client(builder.to_uri());
    return pplx::task<bool>([this]() -> bool {
        bool returnValue = false;
        m_client.request(m_request).then([this, &returnValue](const web::http::http_response &response) -> void {
            if (response.status_code() != web::http::status_codes::OK) {
                return;
            }

            nlohmann::json json = nlohmann::json::parse(response.extract_string(true).get())["response"];

            for (const auto &message : json["messages"]) {
                m_messages.push_back(Message::createFromJson(message, m_groupMembers));
                std::cout << "Message: " << m_messages.at(m_messages.size() - 1).getText() << std::endl;
            }
            returnValue = true;
        }).wait();
        return returnValue;
    });
}

pplx::task<bool> GroupChat::queryMessagesBefore(const Message &beforeMessage, unsigned int messageCount) {
    web::http::uri_builder builder(m_endpointUrl);

    builder.append_path(m_chatId);
    builder.append_path("messages");
    
    builder.append_query("before_id", beforeMessage.getID());
    builder.append_query("limit", messageCount);

    m_client = web::http::client::http_client(builder.to_uri());
    return pplx::task<bool>([this]() -> bool {
        bool returnValue = false;
        m_client.request(m_request).then([this, &returnValue](const web::http::http_response &response) -> void {
            if (response.status_code() != web::http::status_codes::OK) {
                return;
            }

            nlohmann::json json = nlohmann::json::parse(response.extract_string(true).get())["response"];

            for (const auto &message : json["messages"]) {
                m_messages.push_back(Message::createFromJson(message, m_groupMembers));
            }
            returnValue = true;
        }).wait();
        return returnValue;
    });
}

pplx::task<bool> GroupChat::queryMessagesAfter(const Message &afterMessage, unsigned int messageCount) {
    web::http::uri_builder builder(m_endpointUrl);

    builder.append_path(m_chatId);
    builder.append_path("messages");
    
    builder.append_query("after_id", afterMessage.getID());
    builder.append_query("limit", messageCount);

    m_client = web::http::client::http_client(builder.to_uri());
    return pplx::task<bool>([this]() -> bool {
        bool returnValue = false;
        m_client.request(m_request).then([this, &returnValue](const web::http::http_response &response) -> void {
            if (response.status_code() != web::http::status_codes::OK) {
                return;
            }
            returnValue = true;
        }).wait();
        return returnValue;
    });
}

pplx::task<bool> GroupChat::queryMessagesSince(const Message &sinceMessage, unsigned int messageCount) {
    web::http::uri_builder builder(m_endpointUrl);

    builder.append_path(m_chatId);
    builder.append_path("messages");
    
    builder.append_query("since_id", sinceMessage.getID());
    builder.append_query("limit", messageCount);

    m_client = web::http::client::http_client(builder.to_uri());
    return pplx::task<bool>([this]() -> bool {
        bool returnValue = false;
        m_client.request(m_request).then([this, &returnValue](const web::http::http_response &response) -> void {
            if (response.status_code() != web::http::status_codes::OK) {
                return;
            }
            returnValue = true;
        }).wait();
        return returnValue;
    });
}
