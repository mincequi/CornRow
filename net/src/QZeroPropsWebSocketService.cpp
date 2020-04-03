/*
 * Copyright (C) 2020 Manuel Weichselbaumer <mincequi@web.de>
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

#include "QZeroPropsWebSocketService.h"

#include <msgpack.h>

#include <QUuid>

namespace QZeroProps
{

QZeroPropsWebSocketService::QZeroPropsWebSocketService(QZeroPropsService* _q)
    : QZeroPropsServicePrivate(_q)
{
    // Setup socket
    q->connect(&m_socket, &QWebSocket::binaryMessageReceived, [this](const QByteArray &message) {
        onReceive(message);
    });
    q->connect(&m_socket, &QWebSocket::connected, [this]() {
        emit q->stateChanged(QZeroPropsClient::State::Connected);
    });
    q->connect(&m_socket, &QWebSocket::disconnected, [this]() {
        emit q->stateChanged(QZeroPropsClient::State::Disconnected);
    });
    q->connect(&m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), [this](QAbstractSocket::SocketError) {
        emit q->stateChanged(QZeroPropsClient::State::Error, m_socket.errorString());
    });
    q->connect(&m_socket, &QWebSocket::pong, [](quint64 elapsedTime, const QByteArray &payload) {
        qDebug() << "pong> elapsedTime:" << elapsedTime << ", payload.size: " << payload.size();
    });
    q->connect(&m_socket, &QWebSocket::stateChanged, [this](QAbstractSocket::SocketState state) {
        onStateChanged(state);
    });

    MsgPack::registerPacker(QVariant::Type::Uuid, QVariant::Type::Uuid, [](const QVariant& variant) -> QByteArray {
        return variant.toUuid().toRfc4122();
    });
    MsgPack::registerUnpacker(QVariant::Type::Uuid, [](const QByteArray& buffer) -> QVariant {
        return QUuid::fromRfc4122(buffer);
    });
}

QZeroPropsWebSocketService::~QZeroPropsWebSocketService()
{
}

void QZeroPropsWebSocketService::connect()
{
    QUrl url;
    url.setScheme("ws");
    url.setHost(address.toString());
    url.setPort(port);
    m_socket.open(url);
}

void QZeroPropsWebSocketService::disconnect()
{
    m_socket.abort();
}

void QZeroPropsWebSocketService::onStateChanged(QAbstractSocket::SocketState state)
{
    switch (state) {
    case QAbstractSocket::UnconnectedState:
        break;
    case QAbstractSocket::HostLookupState:
    case QAbstractSocket::ConnectingState:
        emit q->stateChanged(QZeroPropsClient::State::Connecting);
        break;
    case QAbstractSocket::ConnectedState:
        emit q->stateChanged(QZeroPropsClient::State::Connected);
        break;
    case QAbstractSocket::BoundState:
    case QAbstractSocket::ClosingState:
        break;
    default:
        break;
    }
}

void QZeroPropsWebSocketService::onReceive(const QByteArray& message)
{
    if (message.front() != static_cast<char>(0x81)) {
        qWarning() << "Illegal data:" << message.front();
        return;
    }

    QVariantList vlist = MsgPack::unpack(message.mid(1)).toList();
    if (vlist.size() != 2 ||
            (vlist.at(0).type() != QVariant::Type::UInt && vlist.at(0).type() != QVariant::Type::Uuid) ||
            vlist.at(1).type() != QVariant::Type::ByteArray) {
        qWarning() << "Illegal data:" << vlist;
        return;
    }

    auto value = vlist.last().toByteArray();

    properties.insert(vlist.first(), value);
    emit q->propertyChanged(vlist.first(), value);
}

void QZeroPropsWebSocketService::doSend(const QVariant& uuid, const QByteArray& value)
{
    // If we are not connected, we do not send
    if (m_socket.state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Socket not connected";
        return;
    }

    QByteArray block;
    block += static_cast<char>(0x81);   // Map with one element
    block += MsgPack::pack(uuid);
    block += MsgPack::pack(value);
    const auto bytes = m_socket.sendBinaryMessage(block);
    qDebug() << "Sent" << bytes << "bytes";

    m_socket.flush();
}

} // namespace QZeroProps
