import QtQuick
import QtQuick.Layouts

import SolOS.Shell 1.0

SectionCard {
    property string appName: ""
    property string appSubtitle: ""
    property string appDescription: ""

    title: appName
    subtitle: appSubtitle
    body: appDescription
}
