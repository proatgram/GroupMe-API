/*
NOTICE: This code, along with the respective source file (multipart_parser.cpp)
is not owned by me (Timothy Hutchins) and is Licensed under the MIT License.

------
MIT License

Copyright (c) 2020 YangangYe

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------

https://github.com/AndsonYe/MultipartEncoder
*/

#pragma once

#include <vector>
#include <string>
#include <tuple>
#include <filesystem>
#include <iostream>

namespace web::http{

    class MultipartParser {
        public:
            MultipartParser();

            const std::string &getBodyContent();
            
            const std::string &getBoundary();

            void addParameter(const std::string &name, const std::string &value);
            
            void addFile(const std::filesystem::path &file);

            void addFile(const std::vector<uint8_t> &data, const std::string &name);

            void addFile(const std::string& data, const std::string& name);
            
            const std::string &generateBody();

        private:
            static std::pair<std::string, std::string> getFileNameTypes(const std::string &filePath);

            static const std::string m_boundaryPrefix;

            static const std::string m_randChars;

            std::string m_boundary;

            std::string m_bodyContent;

            std::vector<std::pair<std::string, std::string>> m_params;

            std::vector<std::pair<std::filesystem::path, std::string>> m_files;

            std::vector<std::pair<std::vector<uint8_t>, std::string>> m_filesRaw;
    };
} //namespace web
