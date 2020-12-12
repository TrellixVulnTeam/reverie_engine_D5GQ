#ifndef TEST_GB_PROTOCOL_H
#define TEST_GB_PROTOCOL_H

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "test_base.h"

#include "../../grand_blue/src/core/encoding/GbProtocol.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tests
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TestProtocol : public TestBase
{
    Q_OBJECT

public:

    TestProtocol();
    ~TestProtocol() {}

private slots:

    void protocolFields();
    void protocol();
    void protocolReadWrite();

private:
};


#endif