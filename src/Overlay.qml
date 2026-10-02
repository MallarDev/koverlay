import QtQuick

Item {
    width: content.item ? content.item.width : 400
    height: content.item ? content.item.height : 80

    Loader {
        id: content
        source: cfg.monitorMode ? "MonitorOverlay.qml" : "TextOverlay.qml"
        onStatusChanged: {
            if (status === Loader.Error)
                console.error("KOverlay: could not load " + source + ". Check the KDE 6 QML runtime dependencies.")
        }
    }

    Text {
        anchors.fill: parent
        visible: content.status === Loader.Error
        color: "white"
        wrapMode: Text.WordWrap
        text: "KOverlay: missing QML modules. Check the terminal and install the KDE 6 sensors and KQuickCharts packages."
    }
}
