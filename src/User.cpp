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

#include "User.h"

using namespace GroupMe;

User::User(const std::string& userID, const std::string& userNickname, const std::string& userProfileImageURL, const std::string& userPhoneNumber, const std::string& userEmail, const std::string& userGUID) :
    m_userID(userID),
    m_userNickname(userNickname),
    m_userProfileImageURL(userProfileImageURL),
    m_userPhoneNumber(userPhoneNumber),
    m_userEmail(userEmail),
    m_userGUID(userGUID),
    m_locale(),
    m_zipcode(),
    m_shareURL(),
    m_shareQRCodeURL(),
    m_createdAt(),
    m_updatedAt(),
    m_isSMS(false),
    m_isFacebookConnected(false),
    m_isTwitterConnected(false)
{

}

User::User() :
    m_userID(),
    m_userNickname(),
    m_userProfileImageURL(),
    m_userPhoneNumber(),
    m_userEmail(),
    m_locale(),
    m_zipcode(),
    m_shareURL(),
    m_shareQRCodeURL(),
    m_createdAt(),
    m_updatedAt(),
    m_isSMS(false),
    m_isFacebookConnected(false),
    m_isTwitterConnected(false)
{

}

std::string User::getID() const {
    return m_userID;
}

void User::setID(const std::string& userID) {
    m_userID = userID;
}

std::string User::getNickname() const {
    return m_userNickname;
}

void User::setNickname(const std::string& userNickname) {
    m_userNickname = userNickname;
}

std::string User::getProfileImageURL() const {
    return m_userProfileImageURL;
}

void User::setProfileImageURL(const std::string& userProfileImageURL) {
    m_userProfileImageURL = userProfileImageURL;
}

std::string User::getPhoneNumber() const {
    return m_userPhoneNumber;
}

void User::setPhoneNumber(const std::string& userPhoneNumber) {
    m_userPhoneNumber = userPhoneNumber;
}

std::string User::getEmail() const {
    return m_userEmail;
}

void User::setEmail(const std::string& userEmail) {
    m_userEmail = userEmail;
}

std::string User::getGUID() const {
    return m_userGUID;
}

void User::setGUID(const std::string& userGUID) {
    m_userGUID = userGUID;
}

std::string User::getLocale() const {
    return m_locale;
}

void User::setLocal(const std::string& locale) {
    m_locale = locale;
}

std::string User::getZipcode() const {
    return m_zipcode;
}

void User::setZipcode(const std::string& zipcode) {
    m_zipcode = zipcode;
}

std::string User::getShareURL() const {
    return m_shareURL;
}

void User::setShareURL(const std::string& shareURL) {
    m_shareURL = shareURL;
}

std::string User::getShareQRCodeURL() const {
    return m_shareQRCodeURL;
}

void User::setShareQRCodeURL(const std::string& shareQRCodeURL) {
    m_shareQRCodeURL = shareQRCodeURL;
}

unsigned int User::getCreatedAt() const {
    return m_createdAt;
}

void User::setCreatedAt(unsigned int createdAt) {
    m_createdAt = createdAt;
}

unsigned int User::getUpdatedAt() const {
    return m_updatedAt;
}

void User::setUpdatedAt(unsigned int updatedAt) {
    m_updatedAt = updatedAt;
}

bool User::usingSMS() const {
    return m_isSMS;
}

void User::setUsingSMS(bool usingSMS) {
    m_isSMS = usingSMS;
}

bool User::getFacebookConnected() const {
    return m_isFacebookConnected;
}

void User::setFacebookConnected(bool facebookConnected) {
    m_isFacebookConnected = facebookConnected;
}

bool User::getTwitterConnected() const {
    return m_isTwitterConnected;
}

void User::setTwitterConnected(bool twitterConnected) {
    m_isTwitterConnected = twitterConnected;
}

bool User::operator==(const User& user) const {
    if (this->getGUID() != user.getGUID()) {
        return false;
    }
    else if (this->getID() != user.getID()) {
        return false;
    }
    else if (this->getPhoneNumber() != user.getPhoneNumber()) {
        return false;
    }
    else if (this->getEmail() != user.getEmail()) {
        return false;
    }
    else if (this->getProfileImageURL() != user.getProfileImageURL()) {
        return false;
    }
    else if (this->getNickname() != user.getNickname()) {
        return false;
    }
    else {
        return true;
    }
}

bool User::operator!=(const User& user) const {
    // This legit just returns the oposite of the equals comparison operator
    return !(this->operator==(user));
}
