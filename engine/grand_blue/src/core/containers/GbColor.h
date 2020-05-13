/////////////////////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef GB_COLOR_H
#define GB_COLOR_H

// QT
#include <QColor>

// Internal
#include "../geometry/GbVector.h"

namespace Gb {


/////////////////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// Class definitions
/////////////////////////////////////////////////////////////////////////////////////////////


/// @brief Color class
class Color : public QColor {

public:
	//--------------------------------------------------------------------------------------------
	/// @name Constructors/Destructor
	/// @{
    Color();
    Color(const std::vector<real_g>& vec);
    Color(const std::vector<int>& vec);
    Color(const Vector3g& vec);
    Color(const Vector4g& vec);
    Color(int r, int g, int b, int a = 255);
    Color(const QColor& color);
	~Color();
	/// @}

	//--------------------------------------------------------------------------------------------
	/// @name Public Methods
	/// @{

    std::vector<int> toVector4i() const;

    Vector4g toVector4g() const;

    Vector3g toVector3g() const;

	/// @}
protected:

};


/////////////////////////////////////////////////////////////////////////////////////////////
} // End namespaces

#endif