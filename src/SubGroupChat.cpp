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

#include "SubGroupChat.h"

using namespace GroupMe;

SubGroupChat::SubGroupChat(const std::string &token, const std::string &groupId, const std::string &parentId, const std::string &name, const std::string &description, const web::uri &imageUrl, unsigned long long int createdAt, unsigned long long int updatedAt, const std::shared_ptr<UserSet> &members) :
    BasicGroupChat(token, groupId, name, description, imageUrl, createdAt, updatedAt),
    m_parentChatId(parentId)
{
    m_members = members;
}

// TODO: Use User Central Repository
SubGroupChat::SubGroupChat(const std::string &token, const std::string &groupId, const std::string &parentId, const std::shared_ptr<UserSet> &members) :
    BasicGroupChat(token, groupId),
    m_parentChatId(parentId)
{
    web::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());
    uri_builder.append_path(m_parentChatId);
    uri_builder.append_path(SUBGROUP_ENDPOINT_QUERY.data());
    uri_builder.append_path(m_chatId);

    web::http::http_response response = m_client.request(web::http::methods::GET, uri_builder.to_string()).get();

    nlohmann::json response_json = nlohmann::json::parse(response.extract_string(true).get()).at("response");

    m_name = response_json.at("topic");
    m_groupDescription = response_json.at("description");
    m_groupImageUrl = (response_json.at("avatar_url").is_null() ? "" : response_json.at("avatar_url").get<std::string>());
    m_createdAt = response_json.at("created_at");
    m_updatedAt = response_json.at("updated_at");
    m_members = members;
}

std::string SubGroupChat::getParentId() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_parentChatId;
}

std::shared_ptr<SubGroupChat> SubGroupChat::createFromJson(const nlohmann::json &json, const std::shared_ptr<UserSet> &members, const std::string &accessToken) {
    std::shared_ptr<SubGroupChat> subgroup = std::make_shared<SubGroupChat>();

    subgroup->m_parentChatId = std::to_string(json.at("parent_id").get<unsigned long long int>());
    subgroup->m_chatId = std::to_string(json.at("id").get<unsigned long long int>());
    subgroup->m_name = json.at("topic");
    subgroup->m_groupDescription = json.at("description");
    subgroup->m_groupImageUrl = (json.at("avatar_url").is_null() ? "" : json.at("avatar_url").get<std::string>());

    subgroup->m_createdAt = json.at("created_at");
    subgroup->m_updatedAt = json.at("updated_at");
    subgroup->m_members = members;

    subgroup->m_accessToken = accessToken;

    return subgroup;
}

pplx::task<BasicChat::Result> SubGroupChat::queryMessages(unsigned int messageCount) {
    return pplx::task<BasicChat::Result>([=]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::http::uri_builder uri_builder;

        uri_builder.append_path(GROUP_ENDPOINT_QUERY.data());
        uri_builder.append_path(m_chatId);
        uri_builder.append_path("messages");
        
        uri_builder.append_query("limit", messageCount);
        uri_builder.append_query("token", m_accessToken);

        BasicChat::Result returnValue = BasicChat::Result::Failure;
        m_client.request(web::http::methods::GET, uri_builder.to_string()).then([this, &returnValue](const web::http::http_response &response) -> void {
            if (response.status_code() != web::http::status_codes::OK) {
                return;
            }

            nlohmann::json json = nlohmann::json::parse(response.extract_string(true).get()).at("response");

            for (const auto &message : json.at("messages")) {
                // TODO: Change Message::createFromJson to use Central Repository
                m_messages.insert(Message::createFromJson(message, *m_members));
            }
            returnValue = BasicChat::Result::Success;
        }).wait();
        return returnValue;
    });
}
pplx::task<BasicChat::Result> SubGroupChat::queryMessagesBefore(const Message &beforeMessage, unsigned int messageCount) {
    return pplx::task<BasicChat::Result>([=]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::http::uri_builder uri_builder;

        uri_builder.append_path(GROUP_ENDPOINT_QUERY.data());
        uri_builder.append_path(m_chatId);
        uri_builder.append_path("messages");
        
        uri_builder.append_query("before_id", beforeMessage.getID());
        uri_builder.append_query("limit", messageCount);
        uri_builder.append_query("token", m_accessToken);

        BasicChat::Result returnValue = BasicChat::Result::Failure;
        m_client.request(web::http::methods::GET, uri_builder.to_string()).then([this, &returnValue](const web::http::http_response &response) -> void {
            if (response.status_code() != web::http::status_codes::OK) {
                return;
            }

            nlohmann::json json = nlohmann::json::parse(response.extract_string(true).get()).at("response");

            for (const auto &message : json.at("messages")) {
                m_messages.insert(Message::createFromJson(message, *m_members));
            }
            returnValue = BasicChat::Result::Success;
        }).wait();
        return returnValue;
    });
}

pplx::task<BasicChat::Result> SubGroupChat::queryMessagesAfter(const Message &afterMessage, unsigned int messageCount) {
    return pplx::task<BasicChat::Result>([=]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::http::uri_builder uri_builder;

        uri_builder.append_path(GROUP_ENDPOINT_QUERY.data());
        uri_builder.append_path(m_chatId);
        uri_builder.append_path("messages");
        
        uri_builder.append_query("after_id", afterMessage.getID());
        uri_builder.append_query("limit", messageCount);
        uri_builder.append_query("token", m_accessToken);

        BasicChat::Result returnValue = BasicChat::Result::Failure;
        m_client.request(web::http::methods::GET, uri_builder.to_string()).then([this, &returnValue](const web::http::http_response &response) -> void {
            if (response.status_code() != web::http::status_codes::OK) {
                return;
            }
            
            nlohmann::json json = nlohmann::json::parse(response.extract_string(true).get()).at("response");

            for (const auto &message : json.at("messages")) {
                m_messages.insert(Message::createFromJson(message, *m_members));
            }

            returnValue = BasicChat::Result::Success;
        }).wait();
        return returnValue;
    });
}

pplx::task<BasicChat::Result> SubGroupChat::queryMessagesSince(const Message &sinceMessage, unsigned int messageCount) {
    return pplx::task<BasicChat::Result>([=]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::http::uri_builder uri_builder;

        uri_builder.append_path(GROUP_ENDPOINT_QUERY.data());
        uri_builder.append_path(m_chatId);
        uri_builder.append_path("messages");
        
        uri_builder.append_query("since_id", sinceMessage.getID());
        uri_builder.append_query("limit", messageCount);
        uri_builder.append_query("token", m_accessToken);

        BasicChat::Result returnValue = BasicChat::Result::Failure;
        m_client.request(web::http::methods::GET, uri_builder.to_string()).then([this, &returnValue](const web::http::http_response &response) -> void {
            if (response.status_code() != web::http::status_codes::OK) {
                return;
            }

            nlohmann::json json = nlohmann::json::parse(response.extract_string(true).get()).at("response");

            for (const auto &message : json["messages"]) {
                Message msg(Message::createFromJson(message, *m_members));
                m_messages.insert(Message::createFromJson(message, *m_members));
            }

            returnValue = BasicChat::Result::Success;
        }).wait();
        return returnValue;
    });
}

pplx::task<BasicChat::Result> SubGroupChat::sendMessage(const Message &message) {
    return pplx::task<BasicChat::Result>([this, message = message.toJson()]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());

        uri_builder.append_path(m_chatId);
        uri_builder.append_path("messages");

        uri_builder.append_query("token", m_accessToken);

        web::http::http_response response = m_client.request(web::http::methods::POST, uri_builder.to_string(), message.dump(), "application/json").get();

        if (response.status_code() == web::http::status_codes::Created) {
            return BasicChat::Result::Success;
        }

        return BasicChat::Result::Failure;
        
    });
}

pplx::task<BasicChat::Result> SubGroupChat::update() {
    return pplx::task<BasicChat::Result>([this]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::http::uri_builder uri_builder;

        uri_builder.append_path(GROUP_ENDPOINT_QUERY.data());
        uri_builder.append_path(m_parentChatId);
        uri_builder.append_path(SUBGROUP_ENDPOINT_QUERY.data());
        uri_builder.append_path(m_chatId);
        uri_builder.append_query("token", m_accessToken);

        nlohmann::json body{
            {"topic", m_name},
            {"description", m_groupDescription},
            {"avatar_url", m_groupImageUrl},
        };

        BasicChat::Result returnValue = BasicChat::Result::Failure;

        m_client.request(web::http::methods::PUT, uri_builder.to_string(), body.dump(), "application/json").then([this, &returnValue](const web::http::http_response &response) -> void {
            
            if (response.status_code() != web::http::status_codes::OK) {
                return;
            }
            returnValue = BasicChat::Result::Success;

            nlohmann::json group = nlohmann::json::parse(response.extract_string(true).get()).at("response");
                
            m_chatId = std::to_string(group.at("id").get<unsigned long long int>());

            m_name = group.at("topic");
            
            m_groupDescription = group.at("description");

            m_createdAt = group.at("created_at");
            m_updatedAt = group.at("updated_at");

            for (auto &member : group["members"]) {
                std::shared_ptr<User> user = std::make_shared<User>(User::createFromJson(member, m_chatId));
            }
        }).wait();
        return returnValue;
    });
}

