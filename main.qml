import QtQuick 2.9
import QtQuick.Window 2.2
import QtMultimedia 5.5
import com.xdien.classes 1.0
Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    Camera
    {

        id:camera
        focus {
            focusMode: CameraFocus.FocusContinuous
            focusPointMode: CameraFocus.FocusPointAuto
        }
//        captureMode: Camera.CaptureViewfinder

    }
    VideoOutput
    {
        id: videoOutput
        source: camera
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 240
        autoOrientation: true
        fillMode: VideoOutput.PreserveAspectCrop
        filters: [ testFilter ]
        MouseArea {
            anchors.fill: parent
            onClicked: {
                camera.focus.customFocusPoint = Qt.point(mouse.x / width,  mouse.y / height);
                camera.focus.focusMode = CameraFocus.FocusMacro;
                camera.focus.focusPointMode = CameraFocus.FocusPointCustom;
            }
        }

        Rectangle
        {
            id: smile
//            source: "qrc:/smile.png"
            visible: false
            color: "red"
            opacity: 0.5
        }
    }
    CvDetectFilter
    {
        id: testFilter
        onObjectDetected:
        {
            var r = videoOutput.mapNormalizedRectToItem(Qt.rect(x, y, w, h));
            smile.x = r.x;
            smile.y = r.y;
            smile.width = r.width;
            smile.height = r.height;
            smile.visible = true;

        }
    }


}
