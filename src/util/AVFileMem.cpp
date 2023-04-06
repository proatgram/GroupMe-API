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

#include <cassert>
#include <util/AVFileMem.h>

using namespace GroupMe::Util;

AVFormat::AVFormat(const std::vector<uint8_t>& data, AVDictionary **options) :
    m_avFormatContext(avformat_alloc_context()),
    m_opaque(new Opaque),
    m_avioContext(avio_alloc_context(static_cast<uint8_t*>(av_malloc(AVFormat::AVIO_BUFF_SIZE)), AVFormat::AVIO_BUFF_SIZE, 0, static_cast<void*>(m_opaque), &AVFormat::read, nullptr, nullptr))
{
    m_opaque->data = Opaque::Vector(data);
    m_opaque->iterator = m_opaque->data.begin();

    m_avFormatContext->pb = m_avioContext;

    avformat_open_input(&m_avFormatContext, "video.dat", nullptr, options);
    avformat_find_stream_info(m_avFormatContext, nullptr);
}

AVFormat::AVFormat(const std::filesystem::path& path, AVDictionary **options) :
    m_avFormatContext(nullptr),
    m_opaque(nullptr),
    m_avioContext(nullptr)
{
    avformat_open_input(&m_avFormatContext, path.c_str(), nullptr, options);
    avformat_find_stream_info(m_avFormatContext, nullptr);
}

AVFormat::~AVFormat() {
    // Clears the data in the Opaque struct
    if(m_opaque != nullptr && m_avioContext != nullptr) {
        m_opaque->data.clear();
        m_opaque->iterator = m_opaque->data.begin();
        delete m_opaque;
        m_opaque = nullptr;
        m_avFormatContext->pb = nullptr;
        av_free(m_avioContext->buffer);
        avformat_free_context(m_avFormatContext);
        avio_context_free(&m_avioContext);
    }
    avformat_close_input(&m_avFormatContext);
    avformat_free_context(m_avFormatContext);
}

const AVFormatContext* AVFormat::operator->() {
    return m_avFormatContext;
}

int AVFormat::read(void* opaque, uint8_t* buf, int size) {
    assert(opaque);
    assert(buf);

    Opaque* octx = static_cast<Opaque*>(opaque);
    if (octx->iterator == octx->data.end()) {
        return 0;
    }

    int distance = static_cast<int>(std::distance(octx->iterator, octx->data.end()));
    int dataCount = std::min(size, distance);

    std::copy(octx->iterator, octx->iterator + dataCount, buf);

    octx->iterator += dataCount;
    return dataCount;
}

int AVFormat::openMemory(const std::vector<uint8_t>& data, AVDictionary **options) {
    if (m_avFormatContext != nullptr) {
        this->closeMemory();
    }
    m_opaque = new Opaque;

    m_opaque->data = Opaque::Vector(data);
    m_opaque->iterator = m_opaque->data.begin();

    m_avioContext = avio_alloc_context(static_cast<uint8_t*>(av_malloc(AVFormat::AVIO_BUFF_SIZE)), AVFormat::AVIO_BUFF_SIZE, 0, static_cast<void*>(m_opaque), &AVFormat::read, nullptr, nullptr);

    m_avFormatContext = avformat_alloc_context();

    m_avFormatContext->pb = m_avioContext;

    return avformat_open_input(&m_avFormatContext, "video.dat", nullptr, options);
}

int AVFormat::openMemory(std::vector<uint8_t>&& data, AVDictionary **options) {
    if (m_avFormatContext != nullptr) {
        this->closeMemory();
    }
    m_opaque = new Opaque;

    m_opaque->data = std::move(data);
    m_opaque->iterator = m_opaque->data.begin();

    m_avioContext = avio_alloc_context(static_cast<uint8_t*>(av_malloc(AVFormat::AVIO_BUFF_SIZE)), AVFormat::AVIO_BUFF_SIZE, 0, static_cast<void*>(m_opaque), &AVFormat::read, nullptr, nullptr);

    m_avFormatContext = avformat_alloc_context();

    m_avFormatContext->pb = m_avioContext;

    return avformat_open_input(&m_avFormatContext, "video.dat", nullptr, options);
}

void AVFormat::closeMemory() {
    // Clears the data in the Opaque struct
    m_opaque->data.clear();
    m_opaque->iterator = m_opaque->data.begin();
    
    delete m_opaque;
    m_opaque = nullptr;

    m_avFormatContext->pb = nullptr;

    avformat_free_context(m_avFormatContext);
    av_free(m_avioContext->buffer);
    avio_context_free(&m_avioContext);
}

const AVFormatContext* AVFormat::get() {
    return m_avFormatContext;
}
