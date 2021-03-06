import QtQuick 2.12
import QtQuick.Controls 2.2
import QtQuick.Controls.impl 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3

import Qt.labs.qmlmodels 1.0

import Cornrow.DeviceModel 1.0
import Cornrow.IoModel 1.0
import Cornrow.FilterModel 1.0
import Cornrow.PresetModel 1.0

import ZpClient 1.0
import ZpService 1.0

Dialog {
    id: myDialog
    visible: true
    background: Rectangle { color: "transparent" }
    padding: 0  // Make the dialog full screen
    closePolicy: Popup.NoAutoClose
    Component.onCompleted: {
        DeviceModel.startDiscovering()
    }
    
    CoroBusyIndicator {
        id: busyIndicator
        y: myDialog.availableHeight/12  // @TODO(mawe): this is placed by trial and error. Can be improved.
        
        CoroBusyIndicator {
            strokeColor: Material.color(Material.Pink)
            fillColor: "transparent"
            innerRadius: 20
            outerRadius: 48
            strokeWidth: 0.67
            inactiveOpacity: 0.67
        }
    }
    
    ColumnLayout {
        y: myDialog.availableHeight/3
        width: myDialog.availableWidth
        height: myDialog.availableHeight-y
        
        Label {
            id: statusLabel
            text: DeviceModel.statusLabel;
            font.capitalization: Font.SmallCaps
            font.pixelSize: 20
            font.weight: Font.DemiBold
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 16
        }
        
        Label {
            id: deviceLabel
            text: DeviceModel.statusText
            font.capitalization: Font.AllUppercase
            font.pixelSize: 10
            opacity: 0.75
            leftPadding: 16
        }
        
        ListView {
            clip: true
            Layout.fillHeight: true
            Layout.fillWidth: true
            model: DeviceModel.services
            enabled: DeviceModel.status === ZpClientState.Discovering ||
                     DeviceModel.status === ZpClientState.Idle
            delegate: ListItemNew {
                icon.source: modelData.type === ZpService.BluetoothLe ? "qrc:/icons/bluetooth.svg" : "qrc:/icons/wifi.svg"
                primaryText: modelData.name
                onClicked: {
                    FilterModel.setService(modelData)
                    CornrowIoModel.setService(modelData)
                    CornrowPresetModel.setService(modelData)
                    DeviceModel.connectToService(modelData)
                }
                //subText: ip
                //subTextFontSize: 12
                //showDivider: true
                /*
                rightItem: IconLabel {
                    icon.source: "qrc:/icons/chevron_right.svg"
                    icon.color: Material.foreground 
                    anchors.centerIn: rightItem
                }
                */
            }
        }
    } // ColumnLayout

    /*
    Column {
        id: statusScreen
        spacing: 16
        anchors.centerIn: parent
        width: myDialog.width

        Label {
            id: statusText
            horizontalAlignment: Text.AlignHCenter
            maximumLineCount: 2
            text: DeviceModel.statusText;
            width: parent.width - 2*spacing
            height: 48
            clip: true
            wrapMode: Text.Wrap
            font.pixelSize: 16
            font.weight: Font.Light
            anchors.horizontalCenter: parent.horizontalCenter
        }

    } // Column
    */

    footer: DialogButtonBox {
        opacity: /*DeviceModel.status != DeviceModel.Discovering &&*/
                 DeviceModel.status !== ZpClientState.Connected

        Button {
            text: DeviceModel.status === ZpClientState.Connecting ? "Abort and rescan" : "Rescan"
            flat: true
            onPressed: {
                DeviceModel.startDiscovering()
            }
        }
        Button {
            text: "Demo"
            flat: true
            visible: DeviceModel.status !== ZpClientState.Connecting
            onPressed: {
                DeviceModel.startDemo()
                CornrowIoModel.startDemo()
                CornrowPresetModel.startDemo()
            }
        }
    }
}
