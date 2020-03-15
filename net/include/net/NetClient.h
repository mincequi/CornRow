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

#pragma once

// @TODO(mawe): remove bluetooth dependency
#include <QBluetoothDeviceInfo>
#include <QHostAddress>
#include <QTimer>

#include <common/Types.h>

class QZeroConf;
class QZeroConfServiceData;
typedef QSharedPointer<QZeroConfServiceData> QZeroConfService;

namespace net
{

struct NetDevice : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(DeviceType type MEMBER type)

public:
	// We cannot use an alias here
    //using DeviceType = common::CtrlInterfaceType;
    
    enum class DeviceType {
		Invalid = 0,
		BluetoothLe = 0x1,
		TcpIp = 0x2
    };
    // We need this for QML engine
    Q_ENUM(DeviceType)
    
    QString name;
    DeviceType type = DeviceType::Invalid;

    QHostAddress address;
    uint16_t port = 0;
    
    QBluetoothDeviceInfo bluetoothDeviceInfo;
};
using NetDevicePtr = QSharedPointer<net::NetDevice>;

class NetClient : public QObject
{
    Q_OBJECT

public:
    explicit NetClient(QObject *parent = nullptr);
    ~NetClient();

    void startDiscovering();
	void stopDiscovering();

signals:
    void deviceDiscovered(NetDevicePtr device);

private:
    void onServiceAdded(QZeroConfService);

    QZeroConf* m_zeroConf = nullptr;
};

} // namespace net

Q_DECLARE_METATYPE(net::NetDevicePtr)