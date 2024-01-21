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

#include <list>
#include <memory>
#include <thread>
#include <chrono>

#include "GroupChat.h"
#include "BasicGroupChat.h"

using namespace GroupMe;

GroupChat::GroupChat(const std::string &token, const std::string &groupId, const std::string &name, Visibility visibility, const std::string &description, const web::uri &imageUrl, const std::shared_ptr<User> &creator, unsigned long long int createdAt, unsigned long long int updatedAt) :
    BasicGroupChat(token, groupId, name, description, imageUrl, createdAt, updatedAt),
    m_groupVisibility(visibility),
    m_groupCreator(creator)
{

}


GroupChat::GroupChat(const std::string &token, const std::string &groupId) :
    BasicGroupChat(token, groupId),
    m_groupVisibility(Visibility::Hidden)
{
    web::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());

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

        m_name = group.at("name");
        
        if (group.at("type") == "private") {
            m_groupType = GroupType::Private;
        }
        else if (group.at("type") == "public") {
            m_groupType = GroupType::Public;
        }

        m_groupDescription = group.at("description");

        m_createdAt = group.at("created_at");
        m_updatedAt = group.at("updated_at");

        for (auto &member : group.at("members")) {
            std::shared_ptr<User> user = std::make_shared<User>(User::createFromJson(member, m_chatId));
            m_members->insert(user);
        }

        m_groupShareUrl = group.at("share_url");

        UserSet::iterator creator = m_members->find(group.at("creator_user_id"));
        if (creator != m_members->cend()) {
            m_groupCreator = *creator;
        }
        else {
            // If for some reason it can't find the creator in the members list
            // (It should, but just to handle any problem that can arise) we create it
            for (const auto &member : group.at("members")) {
                if (member.at("user_id") == group.at("creator_user_id")) {
                    m_groupCreator = std::make_shared<User>(User::createFromJson(member, m_chatId));
                    m_members->insert(m_groupCreator);
                }
            }
        }
    }).wait();

    // Query subgroups
    uri_builder.append_path("subgroups");
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

        nlohmann::json subgroups = nlohmann::json::parse(response.extract_string(true).get()).at("response");

        if (subgroups.empty()) {
            return;
        }
        for (const auto &subgroup : subgroups) {
            m_subgroups.push_back(SubGroupChat::createFromJson(subgroup, m_members, m_accessToken));
        }

    }).wait();
}

std::string GroupChat::getShareUrl() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupShareUrl;
}

pplx::task<BasicChat::Result> GroupChat::addMembers(const std::vector<User> &users) {
    return pplx::task<BasicChat::Result>([this, users]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());
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
            web::http::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());
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
                    const auto [it, insert] = m_members->insert(sharedUser);
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

        web::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());
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
            web::http::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());
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
                    const auto [it, insert] = m_members->insert(sharedUser);
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

        web::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());
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
                    const auto [it, insert] = m_members->insert(sharedUser);
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

        web::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());
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
            web::http::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());
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
                    const auto [it, insert] = m_members->insert(sharedUser);
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

            web::http::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());

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

                m_members->erase(sharedUser);
                returnValue = BasicChat::Result::Success;
            }).wait();

            return returnValue;
    });
}

pplx::task<BasicChat::Result> GroupChat::removeMember(const std::shared_ptr<GroupMe::User> &user) {
    return pplx::task<BasicChat::Result>([this, user]() -> BasicChat::Result {
            std::lock_guard<std::mutex> lock(m_mutex);

            web::http::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());

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

                m_members->erase(user);
                returnValue = BasicChat::Result::Success;
            }).wait();

            return returnValue;
    });
}

pplx::task<BasicChat::Result> GroupChat::destroyGroup() {
    return pplx::task<BasicChat::Result>([this]() -> BasicChat::Result {
            std::lock_guard<std::mutex> lock(m_mutex);

            web::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());

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
                UserSet::iterator newOwnerIt = m_members->find(responseJson.at("results").at(0).at("owner_id"));
                if (newOwnerIt != m_members->end()) {
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
                UserSet::iterator newOwnerIt = m_members->find(responseJson.at("results").at(0).at("owner_id"));
                if (newOwnerIt != m_members->end()) {
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

void GroupChat::setVisibility(GroupMe::GroupChat::Visibility visibility) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_groupVisibility = visibility;
}

GroupChat::Visibility GroupChat::getVisibility() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupVisibility;
}

void GroupChat::setGroupPermissions(const GroupChat::GroupPermissions &permissions) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_groupPermissions = permissions;
}

GroupChat::GroupPermissions GroupChat::getGroupPermissions() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupPermissions;
}

void GroupChat::setDeletionPermissions(const GroupChat::DeletionPermissions &permissions) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_messageDeletionMode = permissions;
}

GroupChat::DeletionPermissions GroupChat::getDeletionPermissions() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_messageDeletionMode;
}

void GroupChat::setJoinability(const GroupChat::Joinability &memberType) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_groupJoinability = memberType;
}

GroupChat::Joinability GroupChat::getJoinability() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groupJoinability;
}

void GroupChat::setRequiresApproval(bool requiresApproval) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_requiresApproval = requiresApproval;
}

bool GroupChat::getRequiresApproval() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_requiresApproval;
}

void GroupChat::setShowJoinQuestion(bool showJoinQuestion) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_showJoinQuestion = showJoinQuestion;
}

bool GroupChat::getShowJoinQuestion() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_showJoinQuestion;
}

void GroupChat::setJoinQuestion(const std::string &question) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_joinQuestion = question;
}

const std::string& GroupChat::getJoinQuestion() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_joinQuestion;
}

std::shared_ptr<const GroupMe::User> GroupChat::getCreator() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return std::const_pointer_cast<const User>(m_groupCreator);
}

const std::list<std::shared_ptr<SubGroupChat>>& GroupChat::getSubGroups() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_subgroups;
}

pplx::task<BasicChat::Result> GroupChat::createSubGroup(const std::string &topic, const std::string &description) {
    return pplx::task<BasicChat::Result>([=]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());

        uri_builder.append_path(m_chatId);
        uri_builder.append_path("subgroups");

        uri_builder.append_query("token", m_accessToken);

        const nlohmann::json body = {
            {"topic", topic},
            {"description", description}
        };

        web::http::http_response response = m_client.request(web::http::methods::POST, uri_builder.to_string(), body.dump(), "application/json").get();
        switch (response.status_code()) {
            case web::http::status_codes::BadRequest: {
                return BasicChat::Result::Failure;
            }
            case web::http::status_codes::NotFound: {
                return BasicChat::Result::NotFound;
            }
            case web::http::status_codes::Unauthorized: {
                return BasicChat::Result::RequesterIsNotOwner;
            }
        }

        const nlohmann::json json = nlohmann::json::parse(response.extract_string(true).get()).at("response");

        const std::string subGroupId = std::to_string(json.at("id").get<unsigned long long int>());

        m_subgroups.push_back(std::make_unique<SubGroupChat>(m_accessToken, subGroupId, m_chatId, topic, description, (json.at("avatar_url").is_null() ? "" : json.at("avatar_url").get<std::string>()), json.at("created_at"), json.at("updated_at"), m_members));

        return BasicChat::Result::Success;
    });
}

pplx::task<std::variant<std::list<std::shared_ptr<GroupMe::SubGroupChat>>::iterator, BasicChat::Result>> 
GroupChat::destroySubGroup(const std::string &subGroupId) {
    return pplx::task<std::variant<std::list<std::shared_ptr<GroupMe::SubGroupChat>>::iterator, BasicChat::Result>>([=]() 
            -> std::variant<std::list<std::shared_ptr<GroupMe::SubGroupChat>>::iterator, BasicChat::Result> {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());

        uri_builder.append_path(m_chatId);
        uri_builder.append_path("subgroups");
        uri_builder.append_path(subGroupId);

        uri_builder.append_query("token", m_accessToken);

        web::http::http_response response = m_client.request(web::http::methods::DEL, uri_builder.to_string()).get();

        switch (response.status_code()) {
            case web::http::status_codes::BadRequest: {
                return BasicChat::Result::Failure;
            }
            case web::http::status_codes::NotFound: {
                return BasicChat::Result::NotFound;
            }
            case web::http::status_codes::Unauthorized: {
                return BasicChat::Result::RequesterIsNotOwner;
            }
        }

        for (std::list<std::shared_ptr<SubGroupChat>>::iterator subchat = m_subgroups.begin(); subchat != m_subgroups.end(); ++subchat) {
            if (subchat->get()->getId() == subGroupId) {
               return m_subgroups.erase(subchat++);
            }
        }
        return BasicChat::Result::NotFound;
    });
}

pplx::task<std::variant<std::list<std::shared_ptr<GroupMe::SubGroupChat>>::iterator, BasicChat::Result>> 
GroupChat::destroySubGroup(const SubGroupChat &subGroupChat) {
    return pplx::task<std::variant<std::list<std::shared_ptr<GroupMe::SubGroupChat>>::iterator, BasicChat::Result>>([=]() 
          -> std::variant<std::list<std::shared_ptr<GroupMe::SubGroupChat>>::iterator, BasicChat::Result> {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());

        uri_builder.append_path(m_chatId);
        uri_builder.append_path("subgroups");
        uri_builder.append_path(subGroupChat.getId());

        uri_builder.append_query("token", m_accessToken);

        web::http::http_response response = m_client.request(web::http::methods::DEL, uri_builder.to_string()).get();

        switch (response.status_code()) {
            case web::http::status_codes::BadRequest: {
                return BasicChat::Result::Failure;
            }
            case web::http::status_codes::NotFound: {
                return BasicChat::Result::NotFound;
            }
            case web::http::status_codes::Unauthorized: {
                return BasicChat::Result::RequesterIsNotOwner;
            }
        }

        for (std::list<std::shared_ptr<SubGroupChat>>::iterator subchat = m_subgroups.begin(); subchat != m_subgroups.end(); ++subchat) {
            if (subchat->get()->getId() == subGroupChat.getId()) {
                return m_subgroups.erase(subchat++);
            }
        }
        return BasicChat::Result::NotFound;
    });
}

pplx::task<std::variant<std::list<std::shared_ptr<GroupMe::SubGroupChat>>::iterator, BasicChat::Result>> 
GroupChat::destroySubGroup(const std::shared_ptr<SubGroupChat> &subGroupChat) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return pplx::task<std::variant<std::list<std::shared_ptr<GroupMe::SubGroupChat>>::iterator, BasicChat::Result>>([=]() 
          -> std::variant<std::list<std::shared_ptr<GroupMe::SubGroupChat>>::iterator, BasicChat::Result> {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());

        uri_builder.append_path(m_chatId);
        uri_builder.append_path("subgroups");
        uri_builder.append_path(subGroupChat->getId());

        uri_builder.append_query("token", m_accessToken);

        web::http::http_response response = m_client.request(web::http::methods::DEL, uri_builder.to_string()).get();

        switch (response.status_code()) {
            case web::http::status_codes::BadRequest: {
                return BasicChat::Result::Failure;
            }
            case web::http::status_codes::NotFound: {
                return BasicChat::Result::NotFound;
            }
            case web::http::status_codes::Unauthorized: {
                return BasicChat::Result::RequesterIsNotOwner;
            }
        }

        
        for (std::list<std::shared_ptr<SubGroupChat>>::iterator subchat = m_subgroups.begin(); subchat != m_subgroups.end(); ++subchat) {
            if (*subchat == subGroupChat) {
                return m_subgroups.erase(subchat++);
            }
        }
        return BasicChat::Result::NotFound;
    });
}

pplx::task<BasicChat::Result> GroupChat::queryMessages(unsigned int messageCount) {
    return pplx::task<BasicChat::Result>([=]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());

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

pplx::task<BasicChat::Result> GroupChat::queryMessagesBefore(const Message &beforeMessage, unsigned int messageCount) {
    return pplx::task<BasicChat::Result>([=]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());

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

pplx::task<BasicChat::Result> GroupChat::queryMessagesAfter(const Message &afterMessage, unsigned int messageCount) {
    return pplx::task<BasicChat::Result>([=]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());

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

pplx::task<BasicChat::Result> GroupChat::queryMessagesSince(const Message &sinceMessage, unsigned int messageCount) {
    return pplx::task<BasicChat::Result>([=]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());

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
                m_messages.insert(Message::createFromJson(message, *m_members));
            }

            returnValue = BasicChat::Result::Success;
        }).wait();
        return returnValue;
    });
}

pplx::task<BasicChat::Result> GroupChat::sendMessage(const Message &message) {
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

pplx::task<BasicChat::Result> GroupChat::update() {
    return pplx::task<BasicChat::Result>([this]() -> BasicChat::Result {
        std::lock_guard<std::mutex> lock(m_mutex);

        web::http::uri_builder uri_builder(GROUP_ENDPOINT_QUERY.data());

        uri_builder.append_path(m_chatId);
        uri_builder.append_path("update");
        uri_builder.append_query("token", m_accessToken);

        auto deletion_permissions = nlohmann::json::array();

        switch (m_messageDeletionMode) {
            case DeletionPermissions::Both: {
                deletion_permissions.push_back("admin");
                deletion_permissions.push_back("sender");
                break;
            }
            case DeletionPermissions::AdminOnly: {
                deletion_permissions.push_back("admin");
                break;
            }
        }

        nlohmann::json body {
            {"name", m_name},
            {"description", m_groupDescription},
            {"image_url", m_groupImageUrl},
            {"share", true},
            {"requires_approval", m_groupJoinability == Joinability::ApprovedMembers},
            {"visibility", m_groupVisibility == Visibility::Hidden ? "hidden" : "searchable"},
            {"message_deletion_mode", deletion_permissions},
            {"description", m_groupDescription},
            {"type", m_groupType == GroupType::Private ? "private" : "public"}
        };
        if (m_groupJoinability == Joinability::ApprovedMembers) {
            body.push_back({"show_join_question", m_showJoinQuestion});
            if (m_showJoinQuestion) {
                body.push_back({"join_question", {{"text", m_joinQuestion}, {"type", "join_reason/questions/text"}}});
            }
        }
        std::cout << body.dump(4) << std::endl;

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
                m_groupType = GroupType::Private;
            }
            else if (group.at("type") == "public") {
                m_groupType = GroupType::Public;
            }

            m_groupDescription = group.at("description");

            m_createdAt = group.at("created_at");
            m_updatedAt = group.at("updated_at");

            m_groupShareUrl = group["share_url"];
            for (auto &member : group["members"]) {
                std::shared_ptr<User> user = std::make_shared<User>(User::createFromJson(member, m_chatId));
            }
            m_groupCreator = *m_members->find(group.at("creator_user_id"));
        }).wait();
        return returnValue;
    });
}

