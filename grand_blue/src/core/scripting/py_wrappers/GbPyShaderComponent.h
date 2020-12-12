/////////////////////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef GB_PY_SHADER_COMPONENT_H
#define GB_PY_SHADER_COMPONENT_H

// External
#include "GbPyComponent.h"

namespace Gb {
/////////////////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// Class definitions
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Class to encapsulate initialization of Light pybind wrapper
class PyShaderComponent: public PyComponent {
public:

    /// @brief Wrap a behavior for Python
    static void PyInitialize(py::module_& m);

};

/////////////////////////////////////////////////////////////////////////////////////////////
} // End namespaces

#endif