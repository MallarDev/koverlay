import QtQuick

Rectangle {
    id: root
    readonly property var settings: cfg.monitor
    readonly property int columnCount: Math.max(1, Math.min(settings.columns, settings.cards.length))
    width: settings.cards.length ? columnCount * settings.chartWidth + (columnCount - 1) * 20 + 32 : 340
    height: settings.cards.length ? Math.ceil(settings.cards.length / columnCount) * settings.chartHeight
        + (Math.ceil(settings.cards.length / columnCount) - 1) * 20 + 32 : 64
    radius: 12
    color: Qt.rgba(0.04, 0.05, 0.07, cfg.panelOpacity)

    Grid {
        x: 16
        y: 16
        columns: root.columnCount
        spacing: 20
        Repeater {
            model: root.settings.cards
            SensorCard {
                required property var modelData
                width: root.settings.chartWidth
                height: root.settings.chartHeight
                descriptor: modelData
                historySeconds: root.settings.historySeconds
                updateInterval: root.settings.updateInterval
                textSize: root.settings.fontSize
                textColor: cfg.textColor
                fontFamily: cfg.fontFamily || Qt.application.font.family
                active: overlayWindow.visible
            }
        }
    }

    Text {
        anchors.centerIn: parent
        visible: root.settings.cards.length === 0
        color: cfg.textColor
        text: "Enable a sensor in the [monitor] configuration."
    }
}
