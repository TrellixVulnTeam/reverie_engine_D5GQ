#include "GByteArray.h"

namespace rev {

/////////////////////////////////////////////////////////////////////////////////////////////
ByteArray::ByteArray() :
    m_byteData(nullptr),
    m_dataFlags(ByteArrayFlag::kTakeOwnership)
{
}
/////////////////////////////////////////////////////////////////////////////////////////////
ByteArray::~ByteArray()
{
    if (m_byteData && m_dataFlags.testFlag(ByteArrayFlag::kTakeOwnership)) {
        delete[] m_byteData;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
} // End namespaces
