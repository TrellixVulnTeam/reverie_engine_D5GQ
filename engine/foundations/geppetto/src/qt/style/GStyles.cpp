/*
###############################################################################
#                                                                             #
# The MIT License                                                             #
#                                                                             #
# Copyright (C) 2017 by Juergen Skrotzky (JorgenVikingGod@gmail.com)          #
#               >> https://github.com/Jorgen-VikingGod                        #
#                                                                             #
# Sources: https://github.com/Jorgen-VikingGod/Qt-Frameless-Window-DarkStyle  #
#                                                                             #
###############################################################################
*/

#include "geppetto/qt/style/GStyles.h"

namespace rev {

DarkStyle::DarkStyle() :  DarkStyle(styleBase()) 
{
}

DarkStyle::DarkStyle(QStyle *style) : 
    QProxyStyle(style) 
{
}

void DarkStyle::polish(QPalette &palette) {
  // Called internally by Qt on setStyle
  // modify palette to dark
  palette.setColor(QPalette::Window, QColor(53, 53, 53));
  palette.setColor(QPalette::WindowText, Qt::white);
  palette.setColor(QPalette::Disabled, QPalette::WindowText,
                   QColor(127, 127, 127));
  palette.setColor(QPalette::Base, QColor(42, 42, 42));
  palette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
  palette.setColor(QPalette::ToolTipBase, Qt::white);
  palette.setColor(QPalette::ToolTipText, QColor(53, 53, 53));
  palette.setColor(QPalette::Text, Qt::white);
  palette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
  palette.setColor(QPalette::Dark, QColor(35, 35, 35));
  palette.setColor(QPalette::Shadow, QColor(20, 20, 20));
  palette.setColor(QPalette::Button, QColor(53, 53, 53));
  palette.setColor(QPalette::Midlight, QColor(81, 81, 81)); // Added by Dante
  palette.setColor(QPalette::Light, QColor(100, 100, 100)); // Added by Dante
  palette.setColor(QPalette::ButtonText, Qt::white);
  palette.setColor(QPalette::Disabled, QPalette::ButtonText,
                   QColor(127, 127, 127));
  palette.setColor(QPalette::BrightText, Qt::red);
  palette.setColor(QPalette::Link, QColor(42, 130, 218));
  //palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
  palette.setColor(QPalette::Highlight, QColor(32, 110, 188));
  //palette.setColor(QPalette::Highlight, QColor(22, 90, 158));
  palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
  palette.setColor(QPalette::HighlightedText, Qt::white);
  palette.setColor(QPalette::Disabled, QPalette::HighlightedText,
                   QColor(127, 127, 127));
}

void DarkStyle::polish(QApplication *app) {
  // Called internally by Qt on setStyle
  if (!app) return;

  // increase font size for better reading,
  // setPointSize was reduced from +2 because when applied this way in Qt5, the
  // font is larger than intended for some reason
  QFont defaultFont = QApplication::font();
  defaultFont.setPointSize(defaultFont.pointSize() + 1);
  app->setFont(defaultFont);

  // loadstylesheet
  QFile qfDarkstyle(QStringLiteral(":/styles/darkstyle/darkstyle.qss"));
  if (qfDarkstyle.open(QIODevice::ReadOnly | QIODevice::Text)) {
    // set stylesheet
    QString qsStylesheet = QString::fromLatin1(qfDarkstyle.readAll());
    app->setStyleSheet(qsStylesheet);
    qfDarkstyle.close();
  }
}

QStyle *DarkStyle::styleBase(QStyle *style) const {
    // Get base fusion style
    static QStyle *base =
        !style ? QStyleFactory::create(QStringLiteral("Fusion")) : style;
    return base;
}



} // rev