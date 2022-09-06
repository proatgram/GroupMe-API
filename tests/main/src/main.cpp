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

#include <cstdio>
#include <cstdlib>

#include "GroupMe_API.h"

int main(int argc, char** argv) {
    GroupMe::Authenticator auth("http://127.0.0.1:5678", "https://oauth.groupme.com/oauth/authorize?client_id=Tscatf1rOsIcqr7xRAQgCEoGU9xiwRgIDqPQPszFcjwHuZj6");

    GroupMe::Main test(auth.getToken());

    return EXIT_SUCCESS;
}
