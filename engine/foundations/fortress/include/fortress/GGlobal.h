#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rev {
    
/// @brief  For disabling a compiler warning that a variable is unused
/// @tparam ...Args 
/// @param ... 
template <typename... Args> 
inline void G_UNUSED(Args&&...) {}

}// End namespaces