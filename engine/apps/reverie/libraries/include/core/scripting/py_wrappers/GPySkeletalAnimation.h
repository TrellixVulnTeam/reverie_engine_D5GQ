/////////////////////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef GB_PY_SKELETAL_ANIMATION_H
#define GB_PY_SKELETAL_ANIMATION_H

// External
#include "GPyComponent.h"

namespace rev {
/////////////////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// Class definitions
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Class to encapsulate initialization of SkeletalAnimation pybind wrapper
class PySkeletalAnimation: public PyComponent{
public:

    /// @brief Wrap a behavior for Python
    static void PyInitialize(py::module_& m);

};

/////////////////////////////////////////////////////////////////////////////////////////////
} // End namespaces

#endif