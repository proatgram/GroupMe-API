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

#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

namespace GroupMe {

    namespace Util {

        class InMemoryAVFormat {
            public:

                InMemoryAVFormat();

                int openMemory(const std::vector<uint8_t>& data, AVDictionary** options = nullptr);

                int openMemory(std::vector<uint8_t>&& data, AVDictionary** options = nullptr);

                void closeMemory();

                static void closeMemory(std::shared_ptr<AVFormatContext> context);

                std::shared_ptr<AVFormatContext> get();

            private:

                struct Opaque{
                    using Vector = std::vector<uint8_t>;
                    Vector data;
                    Vector::iterator iterator;
                };

                static int read(void* opaque, uint8_t* buf, int size);

                std::shared_ptr<AVFormatContext> m_context;

        };

    }

}
