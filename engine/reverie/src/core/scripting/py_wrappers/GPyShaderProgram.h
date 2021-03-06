/////////////////////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef GB_PY_SHADER_PROGRAM_H
#define GB_PY_SHADER_PROGRAM_H

// External
#include "../GPythonWrapper.h"

namespace rev {
/////////////////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
/////////////////////////////////////////////////////////////////////////////////////////////
struct Uniform;

/////////////////////////////////////////////////////////////////////////////////////////////
// Class Definitions
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Class to encapsulate initialization of a shader program pybind wrapper
class PyShaderProgram{
public:

    /// @brief Wrap a behavior for Python
    static void PyInitialize(py::module_& m);

    static PyObject* uniformToPyObject(const Uniform& uniform);
};

/////////////////////////////////////////////////////////////////////////////////////////////
} // End namespaces

#endif