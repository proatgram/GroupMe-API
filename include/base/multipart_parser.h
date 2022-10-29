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

namespace web{
namespace http{

class MultipartParser
{
public:
  MultipartParser();
  inline const std::string &body_content()
  {
    return body_content_;
  }
  inline const std::string &boundary()
  {
    return boundary_;
  }
  inline void AddParameter(const std::string &name, const std::string &value)
  {
    params_.push_back(std::move(std::pair<std::string, std::string>(name, value)));
  }
  inline void AddFile(const std::filesystem::path &file)
  {
    files_.push_back(std::move(std::pair<std::filesystem::path, std::string>(file, file.filename())));
  }
  inline void AddFile(const std::vector<uint8_t>& data, const std::string name) {
    files_raw_.push_back(std::move(std::pair<std::vector<uint8_t>, std::string>(std::move(data), name)));
  }
  inline void AddFile(const std::string& data, const std::string name) {
    files_raw_.push_back(std::move(std::pair<std::vector<uint8_t>, std::string>(std::move(std::vector<uint8_t>(data.begin(), data.end())), name)));
  }
  const std::string &GenBodyContent();

private:
  void _get_file_name_type(const std::string &file_path, std::string *filenae, std::string *content_type);
private:
  static const std::string boundary_prefix_;
  static const std::string rand_chars_;
  std::string boundary_;
  std::string body_content_;
  std::vector<std::pair<std::string, std::string> > params_;
  std::vector<std::pair<std::filesystem::path, std::string> > files_;
  std::vector<std::pair<std::vector<uint8_t> , std::string>> files_raw_;
};

} //namespace web::http
} //namespace web
