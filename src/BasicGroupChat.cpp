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

#include <cpprest/uri_builder.h>
#include <libavutil/pixfmt.h>

#include "BasicGroupChat.h"

using namespace GroupMe;

BasicGroupChat::BasicGroupChat(const std::string &token, const std::string &groupId, const std::string &name, VisibilityType type, const std::string &description, const web::uri &imageUrl, const std::shared_ptr<User> &creator, unsigned long long int createdAt, unsigned long long int updatedAt, const web::uri &shareUrl) :
    BasicChat(groupId, token, createdAt, name),
    m_groupDescription(description),
    m_groupImageUrl(imageUrl.to_string()),
    m_groupShareUrl(shareUrl.to_string()),
    m_groupVisibility(type),
    m_updatedAt(updatedAt),
    m_groupCreator(creator)
{

}

BasicGroupChat::BasicGroupChat(const std::string &token, const std::string &groupId) :
    BasicChat(groupId, token),
    m_groupVisibility(),
    m_updatedAt()
{

}

std::string BasicGroupChat::getDescription() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupDescription;
}

void BasicGroupChat::setDescription(const std::string &description) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_groupDescription = description;
}

std::string BasicGroupChat::getImageUrl() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupImageUrl;
}

void BasicGroupChat::setImage(GroupMe::Picture &image) {
    m_task = pplx::task<void>([&image, this]() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_groupImageUrl = image.upload().get();
    });
}

void BasicGroupChat::setImage(const web::uri &url) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_groupImageUrl = url.to_string();
}

std::shared_ptr<const GroupMe::User> BasicGroupChat::getCreator() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return std::const_pointer_cast<const User>(m_groupCreator);
}

BasicGroupChat::VisibilityType BasicGroupChat::getVisibility() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupVisibility;
}

unsigned long long int BasicGroupChat::getUpdatedAt() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_updatedAt;
}

pplx::task<BasicChat::Result> BasicGroupChat::update() {
    return pplx::task<BasicChat::Result>([this]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::http::uri_builder uri_builder;

        uri_builder.append_path(m_chatId);
        uri_builder.append_path("update");
        uri_builder.append_query("token", m_accessToken);

        nlohmann::json body{
            {"name", m_name},
            {"description", m_groupDescription},
            {"image_url", m_groupImageUrl},
            {"share", true}
        };

        BasicChat::Result returnValue = BasicChat::Result::Failure;

        m_client.request(web::http::methods::POST, uri_builder.to_string(), body.dump(), "application/json").then([this, &returnValue](const web::http::http_response &response) -> void {
            
            if (response.status_code() != web::http::status_codes::OK) {
                return;
            }
            returnValue = BasicChat::Result::Success;

            nlohmann::json group = nlohmann::json::parse(response.extract_string(true).get()).at("response");
                
            m_chatId = group.at("id");

            m_name = group.at("name");
            
            if (group.at("type") == "private") {
                m_groupVisibility = VisibilityType::Private;
            }
            else if (group.at("type") == "public") {
                m_groupVisibility = VisibilityType::Public;
            }

            m_groupDescription = group.at("description");

            m_createdAt = group.at("created_at");
            m_updatedAt = group.at("updated_at");

            m_groupShareUrl = group["share_url"];
            for (auto &member : group["members"]) {
                std::shared_ptr<User> user = std::make_shared<User>(User::createFromJson(member, m_chatId));
            }
            m_groupCreator = *m_members.find(group.at("creator_user_id"));
        }).wait();
        return returnValue;
    });
}

const std::map<unsigned long long int, GroupMe::Message>& BasicGroupChat::getMessages() const {
    return std::as_const(m_messages);
}

pplx::task<BasicChat::Result> BasicGroupChat::queryMessages(const Message &referenceMessage, BasicChat::QueryType queryType, unsigned int messageCount) {
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
            return pplx::task<BasicChat::Result>([]() -> BasicChat::Result { return BasicChat::Result::InvalidQueryType; });
        }
    }
}

