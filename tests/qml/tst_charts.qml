import QtQuick
import QtTest
import "../../src" as Overlay

TestCase {
    id: test
    name: "MonitorCharts"
    when: windowShown
    width: 700
    height: 600

    Component { id: chartComponent; Overlay.ChartCard {} }
    Component { id: monitorComponent; Overlay.MonitorOverlay {} }
    Component { id: textComponent; Overlay.TextOverlay {} }

    function test_historyAndMissingReadings() {
        const chart = createTemporaryObject(chartComponent, test, {
            active: true, available: true, rawValue: 12, updateInterval: 60000, historySeconds: 120
        })
        verify(chart !== null)
        chart.sample()
        chart.rawValue = 23
        chart.sample()
        compare(chart.samples[0], 23)
        compare(chart.samples[1], 12)
        for (let i = 0; i < 5; ++i) chart.sample()
        compare(chart.samples.length, 3)
        chart.available = false
        compare(chart.samples.length, 0)
        chart.sample()
        compare(chart.samples.length, 0)
        chart.available = true
        chart.rawValue = 0
        chart.sample()
        compare(chart.samples[0], 0)
        chart.active = false
        compare(chart.samples.length, 0)
    }

    function test_memoryScaleAndZeroCapacity() {
        const chart = createTemporaryObject(chartComponent, test, {
            active: false, percent: false, maximum: 32 * 1024 * 1024 * 1024
        })
        compare(chart.axisLabel(1), "32 GiB")
        compare(chart.axisLabel(0.5), "16 GiB")
        chart.maximum = 0
        compare(chart.graphAvailable, false)
        verify(isFinite(chart.upperBound))
    }

    function test_realKdeImportsAndLayouts() {
        const monitor = createTemporaryObject(monitorComponent, test)
        verify(monitor !== null)
        compare(monitor.width, 572)
        compare(monitor.height, 462)
        waitForRendering(monitor)
        const text = createTemporaryObject(textComponent, test)
        verify(text !== null)
        verify(text.width > 0)
    }
}
