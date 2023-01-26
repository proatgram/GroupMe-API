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

#include <util/AVFileMem.h>

using namespace GroupMe::Util;

InMemoryAVFormat::InMemoryAVFormat() :
    m_context(avformat_alloc_context())
{

}

int InMemoryAVFormat::read(void* opaque, uint8_t* buf, int size) {
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

int InMemoryAVFormat::openMemory(const std::vector<uint8_t>& data, AVDictionary** options) {
    const size_t avioBufferSize = 4096;
    uint8_t* avioBuffer = static_cast<uint8_t*>(av_malloc(avioBufferSize));

    Opaque* opaque = new Opaque;

    opaque->data = Opaque::Vector(data);
    opaque->iterator = opaque->data.begin();

    void* op = static_cast<void*>(opaque);

    AVIOContext* avioContext = avio_alloc_context(avioBuffer, avioBufferSize, 0, op, &InMemoryAVFormat::read, nullptr, nullptr);

    m_context->pb = avioContext;

    AVFormatContext* con = m_context.get();

    return avformat_open_input(&con, "mem", NULL, options);
}

// TODO: Make passed in vector parameter a hollow object.
int InMemoryAVFormat::openMemory(std::vector<uint8_t>&& data, AVDictionary** options) {
    std::cerr << "Function not safe to use. Function not finished." << std::endl;

    const size_t avioBufferSize = 4096;
    uint8_t* avioBuffer = static_cast<uint8_t*>(av_malloc(avioBufferSize));

    Opaque* opaque = new Opaque;

    opaque->data = data;
    opaque->iterator = opaque->data.begin();

    void* op = static_cast<void*>(opaque);

    AVIOContext* avioContext = avio_alloc_context(avioBuffer, avioBufferSize, 0, op, &InMemoryAVFormat::read, nullptr, nullptr);

    m_context->pb = avioContext;

    AVFormatContext* con = m_context.get();

    return avformat_open_input(&con, "mem", NULL, options);
}

void InMemoryAVFormat::closeMemory() {
    static_cast<Opaque*>(m_context.get()->pb->opaque)->data.clear();
    static_cast<Opaque*>(m_context.get()->pb->opaque)->iterator = static_cast<Opaque*>(m_context.get()->pb->opaque)->data.begin();
    delete static_cast<Opaque*>(m_context.get()->pb->opaque);
    m_context.get()->pb->opaque = nullptr;
    avio_close(m_context->pb);
    m_context.reset();
}

void InMemoryAVFormat::closeMemory(std::shared_ptr<AVFormatContext> context) {
    static_cast<Opaque*>(context.get()->pb->opaque)->data.clear();
    static_cast<Opaque*>(context.get()->pb->opaque)->iterator = static_cast<Opaque*>(context.get()->pb->opaque)->data.begin();
    delete static_cast<Opaque*>(context.get()->pb->opaque);
    context.get()->pb->opaque = nullptr;
    avio_close(context->pb);
    context.reset();
}

std::shared_ptr<AVFormatContext> InMemoryAVFormat::get() {
    return m_context;
}
