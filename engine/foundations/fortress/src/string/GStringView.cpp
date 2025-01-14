#include "fortress/string/GStringView.h"

namespace rev {

GStringView::GStringView():
    m_cStr(nullptr),
    m_len(0)
{
}

GStringView::GStringView(const char * c_str):
    GStringView(c_str, c_str? strlen(c_str): 0)
{
}

GStringView::GStringView(const char * c_str, size_t len)
{
    if (!c_str) {
        m_len = 0;
        m_cStr = nullptr;
    }
    else{
        m_len = len;
        m_cStr = c_str;
    }
}

GStringView::GStringView(const GString & other):
    GStringView(other.c_str())
{
}

GStringView::~GStringView()
{
}


} // End namespaces
