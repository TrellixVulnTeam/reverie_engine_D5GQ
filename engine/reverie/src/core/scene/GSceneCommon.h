#ifndef GB_SCENE_COMMON_H
#define GB_SCENE_COMMON_H
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Standard Includes

// Qt

// Internal
#include <core/containers/GFlags.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Namespace Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace rev {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Typedefs
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum class SceneBehaviorFlag {
    kEnabled = 1 << 0, // Whether or not the object is enabled
    kInvisible = 1 << 1, // Whether or not the object is visible
    kScriptGenerated = 1 << 2 // If the object is generated via script, right now (2/26/21) this just means Python 
};
MAKE_BITWISE(SceneBehaviorFlag);
MAKE_FLAGS(SceneBehaviorFlag, SceneBehaviorFlags);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // end namespacing

#endif 