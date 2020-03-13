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
                    onClicked: cpp_model.series.OnViewSessionSelected(cpp_model.viewSessionModel.index(index, 0))
                }

                Text {
                    text: cpp_model.viewSessionModel.data(cpp_model.viewSessionModel.index(index, 2), 0);
                    leftPadding: 5
                    color: "#eeeeee"
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            GridView {
                id: grid

                model: cpp_model.history

                Layout.fillWidth: true
                Layout.fillHeight: true

                cellWidth:  40
                cellHeight: 40

                delegate: Item {
                    width: grid.cellWidth;
                    height: grid.cellHeight

                    Rectangle {
                        color: "#dddddd"
                        anchors { fill: parent; margins: 2 }
                    }

                    Text {
                        anchors { fill: parent }
                        text: {
                            var number = cpp_model.history.data(cpp_model.history.index(index, 2), 0);
                            if (number){
                                number;
                            } else {
                                "";
                            }
                        }

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment:   Text.AlignVCenter
                    }
                }
            }

            Button {
                Layout.preferredHeight: 20
                text: "View next"
                onClicked: cpp_model.MarkNextEpisodeViewed();
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
            }
        }
    }
}
