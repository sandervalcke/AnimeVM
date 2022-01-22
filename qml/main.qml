import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Window {
    id: root

    visible: true
    width: 1224
    height: 837
    title: qsTr("Anime View Manager")

    Rectangle {
        anchors.fill: parent
        color: "#222222"
    }

    RowLayout {
        anchors { fill: parent }

        ListView {
            Layout.preferredWidth: 250
            Layout.fillHeight: true

            model: cpp_series

            delegate: series

            RoundButton {
                text: "+"
                anchors { bottom: parent.bottom; right: parent.right; margins: 10 }
                onClicked: cpp_model.addSeries();
            }
        }

        ListView {
            model: cpp_model.viewSessionModel

            Layout.preferredWidth: 200
            Layout.fillHeight: true

            delegate: Rectangle {
                width:  parent.width
                height: 20

                color: area.containsMouse ? "#444444" : "transparent"

                MouseArea {
                    id: area
                    hoverEnabled: true
                    anchors.fill: parent
                    onClicked: cpp_model.series.OnViewSessionSelected(index);
                }

                Text {
                    text: cpp_model.viewSessionModel.data(cpp_model.viewSessionModel.index(index, 2), 0);
                    leftPadding: 5
                    color: "#eeeeee"
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    font.underline: index === cpp_model.series.activeViewSession
                }
            }

            RoundButton {
                text: "+"
                anchors { bottom: parent.bottom; right: parent.right; margins: 10 }
                onClicked: cpp_model.addViewSession();
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true

                color: "#333333"

                GridView {
                    id: grid

                    anchors { fill: parent; margins: 5 }
                    state: "smallTiles"

                    model: cpp_model.history

                    boundsBehavior: Flickable.StopAtBounds

                    cellWidth:  40
                    cellHeight: 40

                    states: [
                        State {
                            name: "largeTiles"

                            PropertyChanges { target: grid; cellWidth: 80; cellHeight: 70 }
                        }

                    ]

                    delegate: Loader {
                        readonly property int index: model.index
                        sourceComponent: grid.state === "smallTiles" ? smallTileComponent : largeTileComponent
                    }
                }
            }

            RowLayout {
                Layout.maximumHeight: 25

                Button {
                    Layout.fillHeight: true
                    text: "Toggle view"
                    flat: true
                    onClicked: {
                        if (grid.state === "smallTiles")
                            grid.state = "largeTiles";
                        else
                            grid.state = "smallTiles";
                    }
                }

                Button {
                    Layout.fillHeight: true
                    text: "View next"
                    flat: true
                    onClicked: cpp_model.MarkNextEpisodeViewed();
                }
            }
        }
    }

    Component {
        id: smallTileComponent

        Item {
            width: grid.cellWidth;
            height: grid.cellHeight

            Rectangle {
                color: "#dddddd"
                anchors { fill: parent; margins: 2 }
                radius: 3
            }

            Text {
                anchors.fill: parent
                text: cpp_model.history.data(cpp_model.history.index(index, 2), 0) ?? ""

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment:   Text.AlignVCenter
            }
        }
    }

    Component {
        id: largeTileComponent

        Item {
            width: grid.cellWidth;
            height: grid.cellHeight

            Rectangle {
                color: "#eeeeee"
                anchors { fill: parent; margins: 2 }
                radius: 3
            }

            ColumnLayout {
                anchors { fill: parent; margins: 5 }
                spacing: 1

                Text {
                    Layout.fillWidth:   true
                    Layout.fillHeight:  true
                    Layout.bottomMargin: 5
                    text: cpp_model.history.data(cpp_model.history.index(index, 2), 0) ?? ""

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment:   Text.AlignVCenter
                    font.bold: true
                }

                Text {
                    visible: grid.state === "largeTiles"
                    Layout.fillWidth: true
                    text: {
                        let date = new Date(cpp_model.history.data(cpp_model.history.index(index, 3), 0));
                        Qt.formatDate(date, "dd-MM-yyyy");
                    }

                    verticalAlignment: Text.AlignVCenter
                }

                Text {
                    visible: grid.state === "largeTiles"
                    Layout.fillWidth: true
                    text: {
                        let date = new Date(cpp_model.history.data(cpp_model.history.index(index, 3), 0));
                        Qt.formatDateTime(date, "h:mm");
                    }

                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }

    Component {
        id: series

        Rectangle {
            width:  parent.width
            height: 20
            anchors { left: parent.left; leftMargin: 5 }

            color: area.containsMouse ? "#444444" : "transparent"

            MouseArea {
                id: area
                hoverEnabled: true
                anchors.fill: parent
                onClicked: cpp_model.OnSeriesSelected(cpp_series.index(index, 0))
            }

            Text {
                //                text: index
                leftPadding: 5
                color: "#eeeeee"
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                text: cpp_series.data(cpp_series.index(index, 1), 0);
                font.underline: index === cpp_model.currentRow;
            }
        }
    }
}
