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
#include <filesystem>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/log.h>
}

namespace GroupMe::Util {

    /**
     * This class is a utility class to open video information using
     * libavformat from in memory instead of a file.
     *
     * @brief A utility class to open information for a video
     *
     */
    class AVFormat {
        public:

            /**
             * @brief Constructs a new `GroupMe::Util::AVFormat` object
             *
             * @param data A vector of data
             *
             * @param options Options for the AVFormat
             *
             */
            explicit AVFormat(const std::vector<uint8_t>& data, AVDictionary **options = nullptr);

            AVFormat();

            ~AVFormat();

            AVFormat(const AVFormat& other);

            AVFormat(AVFormat&& other) noexcept;

            explicit AVFormat(const std::filesystem::path& path, AVDictionary **options = nullptr);

            AVFormat& operator=(const AVFormat& other);

            AVFormat& operator=(AVFormat&& other) noexcept;

            const AVFormatContext* operator->();
            /**
             * @brief Opens a video from a vector
             *
             * @param data A vector of data
             *
             * @param options Options for the AVFormat
             *
             * @returns int
             *
             */
            int openMemory(const std::vector<uint8_t>& data, AVDictionary** options = nullptr);

            /**
             * @brief Opens a video from a vector
             *
             * @param data A vector of data
             *
             * @param options Options for the AVFormat
             *
             * @returns int
             *
             */
            int openMemory(std::vector<uint8_t>&& data, AVDictionary** options = nullptr);

            /**
             * @brief Closes the in memory context
             *
             */
            void closeMemory();

           /**
             * @brief Gets the AVFormatContext. **THIS SHOULD NOT BE COPIED**
             *
             * @return std::shared_ptr<AVFormatContext>
             *
             */
            const AVFormatContext* get();

            static constexpr unsigned short int AVIO_BUFF_SIZE = 4096;

        private:
            struct Opaque{
                using Vector = std::vector<uint8_t>;
                Vector data;
                Vector::iterator iterator;
            };

            static int read(void* opaque, uint8_t* buf, int size);

            AVFormatContext *m_avFormatContext;

            Opaque *m_opaque;

            AVIOContext *m_avioContext;
    };

}
