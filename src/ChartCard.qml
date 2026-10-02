import QtQuick
import org.kde.quickcharts as Charts

Item {
    id: root
    property string title: ""
    property color accent: "#3daee9"
    property color textColor: "white"
    property string fontFamily: Qt.application.font.family
    property int textSize: 12
    property bool percent: true
    property bool active: true
    property bool available: false
    property real rawValue: NaN
    property real maximum: 100
    property string valueText: "—"
    property int historySeconds: 60
    property int updateInterval: 1000
    property var samples: []
    readonly property int historyLength: Math.max(2, Math.ceil(historySeconds * 1000 / updateInterval) + 1)
    readonly property real upperBound: maximum > 0 && isFinite(maximum) ? maximum : 1
    readonly property bool graphAvailable: available && maximum > 0
    width: 260
    height: 130

    /** Drop samples when their sensor, timing, or validity changes. */
    function clearHistory() { samples = [] }

    /** Store newest samples first; the chart puts time zero at the right edge. */
    function sample() {
        if (!active || !graphAvailable || !isFinite(rawValue)) {
            clearHistory()
            return
        }
        samples = [Math.max(0, Math.min(upperBound, rawValue))].concat(samples).slice(0, historyLength)
    }

    /** Format memory and disk axis labels with a single consistent binary unit. */
    function axisLabel(fraction) {
        if (percent) return Math.round(fraction * 100) + "%"
        if (!(maximum > 0)) return "—"
        const units = ["B", "KiB", "MiB", "GiB", "TiB", "PiB"]
        const power = Math.min(5, Math.max(0, Math.floor(Math.log(maximum) / Math.log(1024))))
        const value = maximum * fraction / Math.pow(1024, power)
        return (value === 0 ? "0" : value.toFixed(value < 10 ? 1 : 0)) + " " + units[power]
    }

    onAvailableChanged: { if (!available) clearHistory() }
    onActiveChanged: clearHistory()
    onUpdateIntervalChanged: clearHistory()
    onHistorySecondsChanged: clearHistory()

    Timer {
        running: root.active
        repeat: true
        interval: root.updateInterval
        onTriggered: root.sample()
    }

    Item {
        id: plot
        x: 62
        y: 8
        width: root.width - x
        height: root.height - 44
        Repeater {
            model: 5
            Item {
                required property int index
                width: plot.width
                y: index * plot.height / 4
                Rectangle {
                    width: parent.width
                    height: 1
                    color: root.textColor
                    opacity: 0.16
                }
                Text {
                    x: -62
                    y: -implicitHeight / 2
                    width: 55
                    horizontalAlignment: Text.AlignRight
                    text: root.axisLabel(1 - index / 4)
                    color: root.textColor
                    opacity: 0.65
                    font.pixelSize: root.textSize - 1
                    font.family: root.fontFamily
                }
            }
        }
        Charts.LineChart {
            anchors.fill: parent
            visible: root.graphAvailable
            clip: true
            direction: Charts.XYChart.ZeroAtEnd
            lineWidth: 2
            fillOpacity: 0.22
            interpolate: false
            xRange { from: 0; to: root.historyLength - 1; automatic: false }
            yRange { from: 0; to: root.upperBound; automatic: false }
            valueSources: Charts.ArraySource { array: root.samples }
            colorSource: Charts.SingleValueSource { value: root.accent }
        }
        Text {
            anchors.centerIn: parent
            visible: !root.graphAvailable
            text: root.available && root.maximum === 0 ? "Not configured" : "Unavailable"
            color: root.textColor
            opacity: 0.65
            font.pixelSize: root.textSize
            font.family: root.fontFamily
        }
    }
    Rectangle {
        x: 0
        y: root.height - 18
        width: 3
        height: 14
        radius: 1
        color: root.accent
    }
    Text {
        x: 10
        y: root.height - 20
        width: root.width * 0.4 - 10
        elide: Text.ElideRight
        text: root.title
        color: root.textColor
        font.pixelSize: root.textSize
        font.family: root.fontFamily
    }
    Text {
        x: root.width * 0.4
        y: root.height - 20
        width: root.width * 0.6
        elide: Text.ElideRight
        horizontalAlignment: Text.AlignRight
        text: root.valueText
        color: root.textColor
        font.pixelSize: root.textSize
        font.family: root.fontFamily
    }
}
