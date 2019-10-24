#include "IoModel.h"

#include "BleCentralAdapter.h"

IoModel* IoModel::s_instance = nullptr;

IoModel* IoModel::instance()
{
    return s_instance;
}

IoModel* IoModel::init(BleCentralAdapter* adapter)
{
    if (s_instance) {
        return s_instance;
    }

    s_instance = new IoModel(adapter);
    return s_instance;
}

IoModel::IoModel(BleCentralAdapter* adapter, QObject *parent) :
    QObject(parent),
    m_adapter(adapter)
{
    connect(m_adapter, &BleCentralAdapter::ioCapsReceived, this, &IoModel::onIoCapsReceived);
    connect(m_adapter, &BleCentralAdapter::ioConfReceived, this, &IoModel::onIoConfReceived);

    m_inputs = {
        { common::IoInterfaceType::Invalid, false, 0 }
    };

    m_outputs = {
        { common::IoInterfaceType::Invalid, true, 0 }
    };
}

QStringList IoModel::inputNames() const
{
    QStringList names;

    for (const auto& i : m_inputs) {
        names << toString(i);
    }

    return names;
}

QStringList IoModel::outputNames() const
{
    QStringList names;

    for (const auto& i : m_outputs) {
        names << toString(i);
    }

    return names;
}

int IoModel::activeInput() const
{
    return m_activeInput;
}

void IoModel::setActiveInput(int i)
{
    m_activeInput = i;
    emit activeInputChanged();
}

int IoModel::activeOutput() const
{
    return m_activeOutput;
}

void IoModel::setActiveOutput(int i)
{
    m_activeOutput = i;
    emit activeOutputChanged();
}

common::IoInterface IoModel::input()
{
    if (m_activeInput >= static_cast<int>(m_inputs.size())) {
        return { common::IoInterfaceType::Invalid, false, 0 };
    }

    return m_inputs.at(m_activeInput);
}

common::IoInterface IoModel::output()
{
    if (m_activeOutput >= static_cast<int>(m_outputs.size())) {
        return { common::IoInterfaceType::Invalid, true, 0 };
    }

    return m_outputs.at(m_activeOutput);
}

QString IoModel::toString(common::IoInterface interface)
{
    QString string;

    switch (interface.type) {
    case common::IoInterfaceType::Invalid:
        string = "<unknown>";
        break;
    case common::IoInterfaceType::Default:
        string = "Default";
        break;
    case common::IoInterfaceType::Analog:
        string = "Analog";
        break;
    case common::IoInterfaceType::Spdif:
        string = "SPDIF";
        break;
    case common::IoInterfaceType::Hdmi:
        string = "HDMI";
        break;
    case common::IoInterfaceType::Bluetooth:
        string = "Bluetooth";
        break;
    case common::IoInterfaceType::Airplay:
        string = "Airplay";
        break;
    }
    if (interface.index > 0) {
        string += (" " + QString::number(interface.index));
    }

    return string;
}

void IoModel::onIoCapsReceived(const std::vector<common::IoInterface>& inputs, const std::vector<common::IoInterface>& outputs)
{
    for (const auto& in : inputs) {
        if (in.index > 1) {
            for (uint8_t i = 0; i < in.index; ++i) {
                m_inputs.push_back( { in.type, false, uint8_t(i+1) } );
            }
        } else {
            m_inputs.push_back( { in.type, false, 0 } );
        }
    }

    for (const auto& out : outputs) {
        if (out.index > 1) {
            for (uint8_t i = 0; i < out.index; ++i) {
                m_outputs.push_back( { out.type, true, uint8_t(i+1) } );
            }
        } else {
            m_outputs.push_back( { out.type, true, 0 } );
        }
    }

    emit iosChanged();
}

void IoModel::onIoConfReceived(const common::IoInterface& input, const common::IoInterface& output)
{
    for (uint i = 0; i < m_inputs.size(); ++i) {
        if (m_inputs.at(i) == input) {
            setActiveInput(i);
        }
    }

    for (uint i = 0; i < m_outputs.size(); ++i) {
        if (m_outputs.at(i) == output) {
            setActiveOutput(i);
        }
    }
}