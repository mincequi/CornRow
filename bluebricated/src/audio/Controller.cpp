/*
 * Copyright (C) 2018 Manuel Weichselbaumer <mincequi@web.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Controller.h"

#include <gstreamermm-dsp.h>

#include "../../cornrowd/src/audio/CoroPipeline.h"
#include "../../cornrowd/src/audio/FileDescriptorSource.h"
#include "../../cornrowd/src/audio/Pipeline.h"

namespace audio
{

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    // Init gstreamermm-dsp
    coro::init();

    m_coroPipeline = new CoroPipeline();
}

Controller::~Controller()
{
    delete m_coroPipeline;
}

void Controller::setTransport(int fd, uint16_t blockSize, int rate)
{
    // Stop pipeline (in any case).
    //m_pipeline->stop();
    if (m_fdSource) {
        delete m_fdSource;
        m_fdSource = nullptr;
    }

    if (fd < 0) {
        return;
    }

    m_fd = fd;
    m_blockSize = blockSize;
    m_rate = rate;

    m_fdSource = new FileDescriptorSource(fd, blockSize, m_coroPipeline);
}

void Controller::setVolume(float volume)
{
    // @TODO(mawe): enable volume on CoroPipeline
    //m_coroPipeline->setVolume(volume);
}

} // namespace audio
