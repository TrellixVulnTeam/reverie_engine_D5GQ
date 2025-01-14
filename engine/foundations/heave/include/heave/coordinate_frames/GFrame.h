#ifndef GB_FRAME_H
#define GB_FRAME_H
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Standard Includes

// External

// Project


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Namespace Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace rev {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Enum for supported coordinate frames
/// @details
/// ECI is always fixed to J2000, X vernal equinox, Z north pole, Y cross product
/// ECEF fixed to Earth, X zero longitude, Z north pole, Y cross product
/// RIC Radial In-track Cross-Track coordinate frame, 
///    - Radial center of central body to object, 
///    - In-track is direction along velocity of satellite
///    - Cross-track is the cross product of Radial and In-track
enum CoordinateFrame {
    kAbstractFrame = -1,
    kGLWorld // GL world space
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** @class Frame
    @brief  Represents a coordinate system
*/
class Frame {
public:

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Constructors and Destructors
    /// @{       
    /// @brief Default constructor, creates an invalid frame
    Frame();
    virtual ~Frame();

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Operators
    /// @{ 


    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Properties
    /// @{ 

    /// @}

protected:
    //-----------------------------------------------------------------------------------------------------------------
    /// @name Protected methods
    /// @{


    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Protected members
    /// @{


    /// @}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // end namespacing

#endif 
