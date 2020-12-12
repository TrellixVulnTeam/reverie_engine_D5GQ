#include "GbJsonReader.h"

#include <QJsonDocument>

namespace Gb {
/////////////////////////////////////////////////////////////////////////////////////////////
GString JsonReader::ToGString(const QJsonValue & json, bool verbose)
{
    QJsonDocument doc(json.toObject());
    QJsonDocument::JsonFormat format;
    if (verbose) {
        format = QJsonDocument::Indented;
    }
    else {
        format = QJsonDocument::Compact;
    }
    GString strJson(doc.toJson(format).constData());
    return strJson;
}
/////////////////////////////////////////////////////////////////////////////////////////////
QString JsonReader::ToQString(const QJsonValue & json, bool verbose)
{
    QJsonDocument doc(json.toObject());
    QJsonDocument::JsonFormat format;
    if (verbose) {
        format = QJsonDocument::Indented;
    }
    else {
        format = QJsonDocument::Compact;
    }
    QString strJson(doc.toJson(format));
    return strJson;
}
/////////////////////////////////////////////////////////////////////////////////////////////
QJsonDocument JsonReader::ToJsonDocument(const QString & str)
{
    return QJsonDocument::fromJson(str.toUtf8());
}
/////////////////////////////////////////////////////////////////////////////////////////////
QJsonObject JsonReader::ToJsonObject(const QString & str)
{
    return ToJsonDocument(str).object();
}
/////////////////////////////////////////////////////////////////////////////////////////////
JsonReader::JsonReader(const QString & filepath):
    FileReader(filepath)
{
}
/////////////////////////////////////////////////////////////////////////////////////////////
JsonReader::~JsonReader()
{
}
/////////////////////////////////////////////////////////////////////////////////////////////
QVariantMap JsonReader::getContentsAsVariantMap()
{
    // Get variant map
    QVariantMap dataMap;
    if (fileExists()) {
        dataMap = getContentsAsJsonObject().toVariantMap();
    }

    return dataMap;
}
/////////////////////////////////////////////////////////////////////////////////////////////
QJsonObject JsonReader::getContentsAsJsonObject()
{
    QJsonObject object;
    if (fileExists()) {

        // Get document
        QJsonDocument doc = QJsonDocument::fromJson(getContentsAsByteArray());

        // Get object
         object = doc.object();
    }

    return object;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// End namespaces
}
