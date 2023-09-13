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
    web::uri_builder uri_builder;

    uri_builder.append_path(m_chatId);

    uri_builder.append_query("token", m_accessToken);

    m_client.request(web::http::methods::GET, uri_builder.to_string()).then([this](const web::http::http_response &response) -> void {
        switch (response.status_code()) {
            case web::http::status_codes::BadRequest: {
                std::string errors;
                const nlohmann::json errorsArray(nlohmann::json::parse(response.extract_string(true).get()).at("meta").at("errors"));
                for (const auto &error : errorsArray) {
                    errors.append(error);
                }
                throw web::http::http_exception("400: " + errors);
            }
            case web::http::status_codes::NotFound: {
                throw web::http::http_exception("404: Not Found");
            }
            case web::http::status_codes::Unauthorized: {
                throw web::http::http_exception("401: Unauthorized");
            }
        }

        nlohmann::json group = nlohmann::json::parse(response.extract_string(true).get()).at("response");

        m_chatId = group.at("id");

        m_groupName = group.at("name");
        
        if (group.at("type") == "private") {
            m_groupVisibility = VisibilityType::Private;
        }
        else if (group.at("type") == "public") {
            m_groupVisibility = VisibilityType::Public;
        }

        m_groupDescription = group.at("description");

        m_createdAt = group.at("created_at");
        m_updatedAt = group.at("updated_at");

        for (auto &member : group.at("members")) {
            std::shared_ptr<User> user = std::make_shared<User>(User::createFromJson(member, m_chatId));
            m_groupMembers.insert(user);
        }

        m_groupShareUrl = group.at("share_url");

        UserSet::iterator creator = m_groupMembers.find(group.at("creator_user_id"));
        if (creator != m_groupMembers.cend()) {
            m_groupCreator = *creator;
        }
        else {
            // If for some reason it can't find the creator in the members list
            // (It should, but just to handle any problem that can arise) we create it
            for (const auto &member : group.at("members")) {
                if (member.at("user_id") == group.at("creator_user_id")) {
                    m_groupCreator = std::make_shared<User>(User::createFromJson(member, m_chatId));
                    m_groupMembers.insert(m_groupCreator);
                }
            }
        }
    }).wait();
}

std::string GroupChat::getId() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_chatId;
}

std::string GroupChat::getName() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupName;
}

void GroupChat::setName(const std::string &name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_groupName = name;
}

std::string GroupChat::getDescription() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupDescription;
}

void GroupChat::setDescription(const std::string &description) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_groupDescription = description;
}

std::string GroupChat::getImageUrl() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupImageUrl;
}

void GroupChat::setImage(GroupMe::Picture &image) {
    m_task = pplx::task<void>([&image, this]() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_groupImageUrl = image.upload().get();
    });
}

void GroupChat::setImage(const web::uri &url) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_groupImageUrl = url.to_string();
}

std::shared_ptr<const GroupMe::User> GroupChat::getCreator() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return std::const_pointer_cast<const User>(m_groupCreator);
}

std::string GroupChat::getShareUrl() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupShareUrl;
}

GroupChat::VisibilityType GroupChat::getVisibility() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupVisibility;
}

void GroupChat::setVisibility(GroupMe::GroupChat::VisibilityType visibility) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_groupVisibility = visibility;
}

unsigned long long int GroupChat::getUpdatedAt() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_updatedAt;
}

const GroupMe::UserSet& GroupChat::getMembers() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupMembers;
}

pplx::task<BasicChat::Result> GroupChat::addMembers(const std::vector<User> &users) {
    return pplx::task<BasicChat::Result>([this, users]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::uri_builder uri_builder;
        uri_builder.append_path(m_chatId);
        uri_builder.append_path("members/add");
        uri_builder.append_query("token", m_accessToken);

        nlohmann::json body;

        for (unsigned int var_size = 0; var_size < users.size(); var_size++) {
            body["members"][var_size] = {
                {"nickname", users.at(var_size).getNickname()},
                {"user_id", users.at(var_size).getID()},
                {"guid", users.at(var_size).getGUID()},
                {"phone_number", users.at(var_size).getPhoneNumber()},
                {"email", users.at(var_size).getEmail()}
            };
        }

        BasicChat::Result returnValue = BasicChat::Result::Failure;

        std::string resultId;

        m_client.request(web::http::methods::POST, uri_builder.to_string(), "application/json").then([this, &returnValue, &resultId](const web::http::http_response &response) -> void {
            if (response.status_code() != web::http::status_codes::OK) {
                return;
            }
            resultId = nlohmann::json::parse(response.extract_string(true).get()).at("response").at("result_id");
            returnValue = BasicChat::Result::Success;
        }).wait();

        if (returnValue != BasicChat::Result::Failure) {
            uri_builder = web::http::uri_builder();
            uri_builder.append_path(m_chatId);
            uri_builder.append_path("members");
            uri_builder.append_path(resultId);
            uri_builder.append_query("token", m_accessToken);

            bool ready = false;
            bool resultsNotAvalliable = false;
            while (returnValue != BasicChat::Result::ResultsExpired || returnValue != BasicChat::Result::Success) {
                m_client.request(web::http::methods::GET, uri_builder.to_string()).then([this, &returnValue](const web::http::http_response &response) -> void {
                    switch (response.status_code()) {
                        case web::http::status_codes::NotFound: {
                            returnValue = BasicChat::Result::ResultsExpired;
                        }
                        case web::http::status_codes::ServiceUnavailable: {
                            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                            return;
                        }
                        default: {
                            returnValue = BasicChat::Result::Failure;
                        }
                    }
                    nlohmann::json userJson = nlohmann::json::parse(response.extract_string(true).get()).at("response").at("members").at(0);
                    std::shared_ptr<User> sharedUser(std::make_shared<User>(User::createFromJson(userJson, m_chatId)));
                    const auto [it, insert] = m_groupMembers.insert(sharedUser);
                    returnValue = BasicChat::Result::Success;
                }).wait();
            }
        }
        return returnValue;
    });
}

pplx::task<BasicChat::Result> GroupChat::addMembers(const std::vector<std::shared_ptr<User>> &users) {
    return pplx::task<BasicChat::Result>([this, users]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::uri_builder uri_builder;
        uri_builder.append_path(m_chatId);
        uri_builder.append_path("members/add");
        uri_builder.append_query("token", m_accessToken);

        nlohmann::json body;

        for (unsigned int var_size = 0; var_size < users.size(); var_size++) {
            body["members"][var_size] = {
                {"nickname", users.at(var_size)->getNickname()},
                {"user_id", users.at(var_size)->getID()},
                {"guid", users.at(var_size)->getGUID()},
                {"phone_number", users.at(var_size)->getPhoneNumber()},
                {"email", users.at(var_size)->getEmail()}
            };
        }

        BasicChat::Result returnValue = BasicChat::Result::Failure;

        std::string resultId;

        m_client.request(web::http::methods::POST, uri_builder.to_string(), body.dump(), "application/json").then([this, &returnValue, &resultId](const web::http::http_response &response) -> void {
            if (response.status_code() != web::http::status_codes::OK) {
                return;
            }
            resultId = nlohmann::json::parse(response.extract_string(true).get()).at("response").at("result_id");
            returnValue = BasicChat::Result::Success;
        }).wait();

        if (returnValue != BasicChat::Result::Failure) {
            uri_builder = web::http::uri_builder();
            uri_builder.append_path(m_chatId);
            uri_builder.append_path("members");
            uri_builder.append_path(resultId);
            uri_builder.append_query("token", m_accessToken);

            bool ready = false;
            bool resultsNotAvalliable = false;
            while (returnValue != BasicChat::Result::ResultsExpired || returnValue != BasicChat::Result::Success) {
                m_client.request(web::http::methods::GET, uri_builder.to_string()).then([this, &returnValue](const web::http::http_response &response) -> void {
                    switch (response.status_code()) {
                        case web::http::status_codes::NotFound: {
                            returnValue = BasicChat::Result::ResultsExpired;
                        }
                        case web::http::status_codes::ServiceUnavailable: {
                            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                            return;
                        }
                        default: {
                            returnValue = BasicChat::Result::Failure;
                        }
                    }
                    nlohmann::json userJson = nlohmann::json::parse(response.extract_string(true).get()).at("response").at("members").at(0);
                    std::shared_ptr<User> sharedUser(std::make_shared<User>(User::createFromJson(userJson, m_chatId)));
                    const auto [it, insert] = m_groupMembers.insert(sharedUser);
                    returnValue = BasicChat::Result::Success;
                }).wait();
            }
        }
        return returnValue;
    });
}

pplx::task<BasicChat::Result> GroupChat::addMembers(const User &users) {
    return pplx::task<BasicChat::Result>([this, users]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::uri_builder uri_builder;
        uri_builder.append_path(m_chatId);
        uri_builder.append_path("members/add");
        uri_builder.append_query("token", m_accessToken);

        nlohmann::json body;

        body["members"][0] = {
            {"nickname", users.getNickname()},
            {"user_id", users.getID()},
            {"guid", users.getGUID()},
            {"phone_number", users.getPhoneNumber()},
            {"email", users.getEmail()}
        };

        BasicChat::Result returnValue = BasicChat::Result::Failure;

        std::string resultId;

        m_client.request(web::http::methods::POST, uri_builder.to_string(), body.dump(), "application/json").then([this, &returnValue, &resultId](const web::http::http_response &response) -> void {
            if (response.status_code() != web::http::status_codes::OK) {
                return;
            }
            resultId = nlohmann::json::parse(response.extract_string(true).get()).at("response").at("result_id");
            returnValue = BasicChat::Result::Success;
        }).wait();

        if (returnValue != BasicChat::Result::Failure) {
            web::http::uri_builder uri_builder; 
            uri_builder.append_path(m_chatId);
            uri_builder.append_path("members");
            uri_builder.append_path(resultId);
            uri_builder.append_query("token", m_accessToken);

            bool ready = false;
            bool resultsNotAvalliable = false;
            while (returnValue != BasicChat::Result::ResultsExpired || returnValue != BasicChat::Result::Success) {
                m_client.request(web::http::methods::GET, uri_builder.to_string()).then([this, &returnValue](const web::http::http_response &response) -> void {
                    switch (response.status_code()) {
                        case web::http::status_codes::NotFound: {
                            returnValue = BasicChat::Result::ResultsExpired;
                        }
                        case web::http::status_codes::ServiceUnavailable: {
                            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                            return;
                        }
                        default: {
                            returnValue = BasicChat::Result::Failure;
                        }
                    }
                    nlohmann::json userJson = nlohmann::json::parse(response.extract_string(true).get()).at("response").at("members").at(0);
                    std::shared_ptr<User> sharedUser(std::make_shared<User>(User::createFromJson(userJson, m_chatId)));
                    const auto [it, insert] = m_groupMembers.insert(sharedUser);
                    returnValue = BasicChat::Result::Success;
                }).wait();
            }
        }
        return returnValue;
    });
}

pplx::task<BasicChat::Result> GroupChat::addMembers(const std::shared_ptr<User> &users) {
    return pplx::task<BasicChat::Result>([this, users]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::uri_builder uri_builder;
        uri_builder.append_path(m_chatId);
        uri_builder.append_path("members/add");
        uri_builder.append_query("token", m_accessToken);

        nlohmann::json body;

        body["members"][0] = {
            {"nickname", users->getNickname()},
            {"user_id", users->getID()},
            {"guid", users->getGUID()},
            {"phone_number", users->getPhoneNumber()},
            {"email", users->getEmail()}
        };

        BasicChat::Result returnValue = BasicChat::Result::Failure;

        std::string resultId;

        m_client.request(web::http::methods::POST, uri_builder.to_string(), body.dump(), "application/json").then([this, &returnValue, &resultId](const web::http::http_response &response) -> void {
            if (response.status_code() != web::http::status_codes::OK) {
                return;
            }
            resultId = nlohmann::json::parse(response.extract_string(true).get()).at("response").at("result_id");
            returnValue = BasicChat::Result::Success;
        }).wait();

        if (returnValue != BasicChat::Result::Failure) {
            web::http::uri_builder uri_builder;
            uri_builder.append_path(m_chatId);
            uri_builder.append_path("members");
            uri_builder.append_path(resultId);
            uri_builder.append_query("token", m_accessToken);

            bool ready = false;
            bool resultsNotAvalliable = false;
            while (returnValue != BasicChat::Result::ResultsExpired || returnValue != BasicChat::Result::Success) {
                m_client.request(web::http::methods::GET, uri_builder.to_string()).then([this, &returnValue](const web::http::http_response &response) -> void {
                    switch (response.status_code()) {
                        case web::http::status_codes::NotFound: {
                            returnValue = BasicChat::Result::ResultsExpired;
                        }
                        case web::http::status_codes::ServiceUnavailable: {
                            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                            return;
                        }
                        default: {
                            returnValue = BasicChat::Result::Failure;
                        }
                    }
                    nlohmann::json userJson = nlohmann::json::parse(response.extract_string(true).get()).at("response").at("members").at(0);
                    std::shared_ptr<User> sharedUser(std::make_shared<User>(User::createFromJson(userJson, m_chatId)));
                    const auto [it, insert] = m_groupMembers.insert(sharedUser);
                    returnValue = BasicChat::Result::Success;
                }).wait();
            }
        }
        return returnValue;
    });
}

pplx::task<BasicChat::Result> GroupChat::removeMember(const GroupMe::User &user) {
    return pplx::task<BasicChat::Result>([this, user]() -> BasicChat::Result {
            std::lock_guard<std::mutex> lock(m_mutex);

            std::shared_ptr<User> sharedUser = std::make_shared<User>(user);
            
            web::http::uri_builder uri_builder;

            uri_builder.append_path(m_chatId);
            uri_builder.append_path("members");
            try {
                uri_builder.append_path(user.getUserGroupId(m_chatId).value());
            }
            catch (const std::exception &e) {
                return BasicChat::Result::NotFound;
            }
            uri_builder.append_path("remove");
            uri_builder.append_query("token", m_accessToken);

            BasicChat::Result returnValue = BasicChat::Result::Failure;

            m_client.request(web::http::methods::POST, uri_builder.to_string()).then([this, &returnValue, &sharedUser](const web::http::http_response &response) -> void {
                switch (response.status_code()) {
                    case web::http::status_codes::OK: {
                        returnValue = BasicChat::Result::Success;
                        return;
                    }
                    case web::http::status_codes::NotFound: {
                        returnValue = BasicChat::Result::NotFound;
                        return;
                    }
                    default: {
                        return;
                    }
                }

                m_groupMembers.erase(sharedUser);
                returnValue = BasicChat::Result::Success;
            }).wait();

            return returnValue;
    });
}

pplx::task<BasicChat::Result> GroupChat::removeMember(const std::shared_ptr<GroupMe::User> &user) {
    return pplx::task<BasicChat::Result>([this, user]() -> BasicChat::Result {
            std::lock_guard<std::mutex> lock(m_mutex);

            web::http::uri_builder uri_builder(m_endpointUrl);

            uri_builder.append_path(m_chatId);
            uri_builder.append_path("members");
            try {
                uri_builder.append_path(user->getUserGroupId(m_chatId).value());
            }
            catch (const std::exception &e) {
                return BasicChat::Result::NotFound;
            }
            uri_builder.append_path("remove");
            uri_builder.append_query("token", m_accessToken);

            BasicChat::Result returnValue = BasicChat::Result::Failure;

            m_client.request(web::http::methods::POST, uri_builder.to_string()).then([this, &returnValue, &user](const web::http::http_response &response) -> void {
                switch (response.status_code()) {
                    case web::http::status_codes::NotFound: {
                        returnValue = BasicChat::Result::NotFound;
                        return;
                    }
                    default: {
                        return;
                    }
                }

                m_groupMembers.erase(user);
                returnValue = BasicChat::Result::Success;
            }).wait();

            return returnValue;
    });
}

pplx::task<BasicChat::Result> GroupChat::destroyGroup() {
    return pplx::task<BasicChat::Result>([this]() -> BasicChat::Result {
            std::lock_guard<std::mutex> lock(m_mutex);

            web::http::uri_builder uri_builder;

            uri_builder.append_path(m_chatId);
            uri_builder.append_path("destroy");
            uri_builder.append_query("token", m_accessToken);

            BasicChat::Result returnValue = BasicChat::Result::Failure;

            m_client.request(web::http::methods::POST, uri_builder.to_string()).then([this, &returnValue](const web::http::http_response &response) -> void {
                if (response.status_code() != web::http::status_codes::OK) {
                    return;
                }
                returnValue = BasicChat::Result::Success;
            }).wait();
            return returnValue;
    });
}

pplx::task<BasicChat::Result> GroupChat::changeGroupOwner(const User &user) {
    return pplx::task<BasicChat::Result>([this, user]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        nlohmann::json body;

        body["requests"][0] = {
            {"group_id", m_chatId},
            {"ownder_id", user.getID()}
        };
        
        BasicChat::Result returnValue = BasicChat::Result::Failure;

        m_client.request(web::http::methods::POST, "/change_owners", body.dump(), "application/json").then([this, &returnValue](const web::http::http_response &response) -> void {
                nlohmann::json responseJson = nlohmann::json::parse(response.extract_string(true).get()).at("response");
                std::string responseCode = responseJson.at("results").at(0).at("status");
            if (responseCode == "200") {
                UserSet::iterator newOwnerIt = m_groupMembers.find(responseJson.at("results").at(0).at("owner_id"));
                if (newOwnerIt != getMembers().end()) {
                    returnValue = BasicChat::Result::Success;
                    m_groupCreator = *newOwnerIt;
                }
                else {
                    returnValue = BasicChat::Result::NotFound;
                }
            }
            else if (responseCode == "400") {
                returnValue = BasicChat::Result::RequesterIsNewOwner;
            }
            else if (responseCode == "403") {
                returnValue = BasicChat::Result::RequesterIsNotOwner;
            }
            else if (responseCode == "404") {
                returnValue = BasicChat::Result::NotFound;
            }
            else if (responseCode == "405") {
                returnValue = BasicChat::Result::MissingData;
            }
        }).wait();
        return returnValue;
    });
}

pplx::task<BasicChat::Result> GroupChat::changeGroupOwner(const std::shared_ptr<User> &user) {
    return pplx::task<BasicChat::Result>([this, user]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        nlohmann::json body;
        
        body["requests"][0] = {
            {"group_id", m_chatId},
            {"ownder_id", user->getID()}
        };

        BasicChat::Result returnValue = BasicChat::Result::Failure;

        m_client.request(web::http::methods::POST, "/change_owners", body.dump(), "application/json").then([this, &returnValue](const web::http::http_response &response) -> void {
                nlohmann::json responseJson = nlohmann::json::parse(response.extract_string(true).get()).at("response");
                std::string responseCode = responseJson.at("results").at(0).at("status");
            if (responseCode == "200") {
                UserSet::iterator newOwnerIt = m_groupMembers.find(responseJson.at("results").at(0).at("owner_id"));
                if (newOwnerIt != m_groupMembers.end()) {
                    returnValue = BasicChat::Result::Success;
                    m_groupCreator = *newOwnerIt;
                }
                else {
                    returnValue = BasicChat::Result::NotFound;
                }
            }
            else if (responseCode == "400") {
                returnValue = BasicChat::Result::RequesterIsNewOwner;
            }
            else if (responseCode == "403") {
                returnValue = BasicChat::Result::RequesterIsNotOwner;
            }
            else if (responseCode == "404") {
                returnValue = BasicChat::Result::NotFound;
            }
            else if (responseCode == "405") {
                returnValue = BasicChat::Result::MissingData;
            }
        }).wait();
        return returnValue;
    });
}

pplx::task<BasicChat::Result> GroupChat::update() {
    return pplx::task<BasicChat::Result>([this]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::http::uri_builder uri_builder;

        uri_builder.append_path(m_chatId);
        uri_builder.append_path("update");
        uri_builder.append_query("token", m_accessToken);

        nlohmann::json body{
            {"name", m_groupName},
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

            m_groupName = group.at("name");
            
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
            m_groupCreator = *m_groupMembers.find(group.at("creator_user_id"));
        }).wait();
        return returnValue;
    });
}

const std::map<unsigned long long int, GroupMe::Message>& GroupChat::getMessages() const {
    return std::as_const(m_messages);
}

pplx::task<BasicChat::Result> GroupChat::queryMessages(const Message &referenceMessage, BasicChat::QueryType queryType, unsigned int messageCount) {
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

pplx::task<BasicChat::Result> GroupChat::queryMessages(unsigned int messageCount) {
    return pplx::task<BasicChat::Result>([=]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::http::uri_builder uri_builder;

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
                Message msg(Message::createFromJson(message, m_groupMembers));
                m_messages.insert({msg.getCreatedAt(), msg});
            }
            returnValue = BasicChat::Result::Success;
        }).wait();
        return returnValue;
    });
}

pplx::task<BasicChat::Result> GroupChat::queryMessagesBefore(const Message &beforeMessage, unsigned int messageCount) {
    return pplx::task<BasicChat::Result>([=]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::http::uri_builder uri_builder;

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
                Message msg(Message::createFromJson(message, m_groupMembers));
                m_messages.insert({msg.getCreatedAt(), msg});
            }
            returnValue = BasicChat::Result::Success;
        }).wait();
        return returnValue;
    });
}

pplx::task<BasicChat::Result> GroupChat::queryMessagesAfter(const Message &afterMessage, unsigned int messageCount) {
    return pplx::task<BasicChat::Result>([=]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::http::uri_builder uri_builder;

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
                Message msg(Message::createFromJson(message, m_groupMembers));
                m_messages.insert({msg.getCreatedAt(), msg});
            }

            returnValue = BasicChat::Result::Success;
        }).wait();
        return returnValue;
    });
}

pplx::task<BasicChat::Result> GroupChat::queryMessagesSince(const Message &sinceMessage, unsigned int messageCount) {
    return pplx::task<BasicChat::Result>([=]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::http::uri_builder uri_builder;

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
                Message msg(Message::createFromJson(message, m_groupMembers));
                m_messages.insert({msg.getCreatedAt(), msg});
            }

            returnValue = BasicChat::Result::Success;
        }).wait();
        return returnValue;
    });
}
