#include "GbIcon.h"

namespace Gb {

/////////////////////////////////////////////////////////////////////////////////////////////
Icon::Icon(CanvasComponent* canvas) :
    Label(canvas, kIcon, 40, "free-solid-900", u8"\uf041"),
    m_iconName("map-marker")
{
    //QString folder("C:/Users/dante/Documents/Projects/grand-blue-engine/grand_blue/resources/images/bitmaps/fonts/");
    //getFontFace()->saveBitmap(m_fontSize,
    //    folder + "free-solid-900.png");
}

/////////////////////////////////////////////////////////////////////////////////////////////
Icon::~Icon()
{
}
/////////////////////////////////////////////////////////////////////////////////////////////
void Icon::setFontAwesomeIcon(const QString & iconName)
{
    m_iconName = iconName;
    setText(FontManager::faUnicodeCharacter(iconName));
}
/////////////////////////////////////////////////////////////////////////////////////////////
QJsonValue Icon::asJson() const
{
    QJsonObject object = Label::asJson().toObject();
    return object;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void Icon::loadFromJson(const QJsonValue& json, const SerializationContext& context)
{
    Q_UNUSED(context)

    Label::loadFromJson(json);
    //QJsonObject object = json.toObject();
}

/////////////////////////////////////////////////////////////////////////////////////////////
} // End namespaces
