#include "Controller.h"

#include <QDBusObjectPath>

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    // Create objects
    m_bluetooth = new bluetooth::Controller(this);
    m_audio = new audio::Controller(this);

    connect(m_bluetooth, &bluetooth::Controller::transportChanged, this, &Controller::onTransportChanged);
    connect(m_bluetooth, &bluetooth::Controller::volumeChanged, this, &Controller::onVolumeChanged);
}

Controller::~Controller()
{
}

void Controller::onTransportChanged(int fd, uint16_t blockSize)
{
    m_audio->setTransport(fd, blockSize, 44100);
}

void Controller::onVolumeChanged(float volume)
{
    m_audio->setVolume(volume);
}
