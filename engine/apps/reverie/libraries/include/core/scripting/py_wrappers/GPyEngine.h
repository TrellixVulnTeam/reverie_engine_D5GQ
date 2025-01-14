/////////////////////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef GB_PY_ENGINE_H
#define GB_PY_ENGINE_H

// External
#include "core/scripting/GPythonWrapper.h"

namespace rev {
/////////////////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// Class Definitions
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Class to encapsulate initialization of CoreEngine pybind wrapper
class PyCoreEngine {
public:

    /// @brief Wrap a behavior for Python
    static void PyInitialize(py::module_& m);

};

/////////////////////////////////////////////////////////////////////////////////////////////
} // End namespaces

#endif