import QtQuick
import org.kde.ksysguard.sensors as Sensors

ChartCard {
    id: root
    required property var descriptor
    property double lastReadingAt: 0
    property bool fresh: false

    title: descriptor.label
    accent: descriptor.accent
    percent: descriptor.percent
    rawValue: sensor.value === undefined ? NaN : Number(sensor.value)
    maximum: percent ? 100 : sensor.maximum
    available: fresh && sensor.status === Sensors.Sensor.Ready && isFinite(rawValue)
    valueText: available ? sensor.formattedValue : "—"

    onDescriptorChanged: {
        lastReadingAt = 0
        fresh = false
        clearHistory()
    }
    onActiveChanged: {
        lastReadingAt = 0
        fresh = false
        clearHistory()
    }

    Sensors.Sensor {
        id: sensor
        sensorId: root.descriptor.sensorId
        enabled: root.active
        onValueChanged: {
            root.lastReadingAt = Date.now()
            root.fresh = true
        }
    }

    Timer {
        interval: 1000
        repeat: true
        running: root.active
        onTriggered: root.fresh = root.lastReadingAt > 0
            && Date.now() - root.lastReadingAt < Math.max(5000, sensor.updateInterval * 3)
    }
}
