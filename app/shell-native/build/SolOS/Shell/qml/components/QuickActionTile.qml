import QtQuick
import SolOS.Shell 1.0

SectionCard {
    property string actionTitle: ""
    property string actionSubtitle: ""
    property string actionDescription: ""

    title: actionTitle
    subtitle: actionSubtitle
    body: actionDescription
}
