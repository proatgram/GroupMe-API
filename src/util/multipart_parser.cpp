/*
NOTICE: This code, along with the respective header file (multipart_parser.h)
is not owned by me (Timothy Hutchins) and is licensed under the MIT License.

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

#include <cstdlib>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <future>

#include "util/multipart_parser.h"

namespace web::http {
    const std::string MultipartParser::m_boundaryPrefix("----CppRestSdkClient");
    const std::string MultipartParser::m_randChars("0123456789" "abcdefghijklmnopqrstuvwxyz" "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    MultipartParser::MultipartParser():
        m_boundary(m_boundaryPrefix)
    {
        int times = 0;
        unsigned long len = m_randChars.size();
        while(times < 16) {
            int idx = rand() % len;
            m_boundary.push_back(m_randChars[idx]);
            times++;
        }
    }

    const std::string& MultipartParser::getBodyContent() {
        return m_bodyContent;
    }

    const std::string& MultipartParser::getBoundary() {
        return m_boundary;
    }

    void MultipartParser::addParameter(const std::string& name, const std::string& value) {
            m_params.push_back(std::move(std::pair<std::string, std::string>(name, value)));
    }

    void MultipartParser::addFile(const std::string& data, const std::string& name) {
            m_filesRaw.push_back(std::move(std::pair<std::vector<uint8_t>, std::string>(std::move(std::vector<uint8_t>(data.begin(), data.end())), name)));
    }

    void MultipartParser::addFile(const std::vector<uint8_t>& data, const std::string& name) {
            m_filesRaw.push_back(std::move(std::pair<std::vector<uint8_t>, std::string>(data, name)));
    }

    void MultipartParser::addFile(const std::filesystem::path& file) {
            m_files.push_back(std::move(std::pair<std::filesystem::path, std::string>(file, file.filename())));
    }

    const std::string &MultipartParser::generateBody() {
        std::vector<std::future<std::string> > futures;
        m_bodyContent.clear();
        for(auto &file:m_files) {
            std::future<std::string> content_futures = std::async(std::launch::async, [&file]() {
                std::ifstream ifile(file.first, std::ios::binary | std::ios::ate);
                std::streamsize size = ifile.tellg();
                ifile.seekg(0, std::ios::beg);
                char *buff = new char[size];
                ifile.read(buff, size);
                ifile.close();
                std::string ret(buff, size);
                delete[] buff;
                return ret;
            });
            futures.push_back(std::move(content_futures));
        }

        for(auto &param : m_params) {
            m_bodyContent += "\r\n--";
            m_bodyContent += m_boundary;
            m_bodyContent += "\r\nContent-Disposition: form-data; name=\"";
            m_bodyContent += param.first;
            m_bodyContent += "\"\r\n\r\n";
            m_bodyContent += param.second;
        }

        for(size_t i = 0; i < m_files.size(); ++i) {
            std::string fileContent = futures[i].get();
            auto [filename, contentType] = getFileNameTypes(m_files[i].second);
            m_bodyContent += "\r\n--";
            m_bodyContent += m_boundary;
            m_bodyContent += "\r\nContent-Disposition: form-data; name=\"";
            m_bodyContent += "file";
            m_bodyContent += "\"; filename=\"";
            m_bodyContent += filename;
            m_bodyContent += "\"\r\nContent-Type: ";
            m_bodyContent += contentType;
            m_bodyContent += "\r\n\r\n";
            m_bodyContent += fileContent;
        }
        for (auto& curFile : m_filesRaw) {
            std::string fileContent;
            fileContent.insert(fileContent.begin(), curFile.first.begin(), curFile.first.end());
            auto [filename, contentType] = getFileNameTypes(std::string(curFile.second));
            m_bodyContent += "\r\n--";
            m_bodyContent += m_boundary;
            m_bodyContent += "\r\nContent-Disposition: form-data; name=\"";
            m_bodyContent += "file";
            m_bodyContent += "\"; filename=\"";
            m_bodyContent += filename;
            m_bodyContent += "\"\r\nContent-Type: ";
            m_bodyContent += contentType;
            m_bodyContent += "\r\n\r\n";
            m_bodyContent += fileContent;
        }
        m_bodyContent += "\r\n--";
        m_bodyContent += m_boundary;
        m_bodyContent += "--\r\n";
        return m_bodyContent;
    }

    std::pair<std::string, std::string> MultipartParser::getFileNameTypes(const std::string &filePath) {
        size_t lastSplitter = filePath.find_last_of("/\\");
        std::string filename = filePath.substr(lastSplitter + 1);
        std::string contentType;
        size_t dotPos = filename.find_last_of('.');
        if (dotPos == std::string::npos) {
            contentType = "application/octet-stream";
            return std::make_pair(filename, contentType);
        }
        
        std::string ext = filename.substr(dotPos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == "jpg" || ext == "jpeg") {
            contentType = "image/jpeg";
            return std::make_pair(filename, contentType);
        }
        if (ext == "txt" || ext == "log") {
            contentType = "text/plain";
            return std::make_pair(filename, contentType);
        }
        contentType = "application/octet-stream";
        return std::make_pair(filename, contentType);
    }
} //namespace web
