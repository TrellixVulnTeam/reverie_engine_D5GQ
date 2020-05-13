/////////////////////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef GB_IMAGE_RESOURCE_H
#define GB_IMAGE_RESOURCE_H

// QT
#include <QFile>
#include <QImage>

// Internal
#include "GbResource.h"

namespace Gb {

/////////////////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
/////////////////////////////////////////////////////////////////////////////////////////////
class ResourceCache;
class ShaderProgram;
class Texture;
class CubeTexture;

/////////////////////////////////////////////////////////////////////////////////////////////
// Class definitions
/////////////////////////////////////////////////////////////////////////////////////////////

/// @class ResourceHandle
/// @brief Class representing a resource
class Image: public Resource{
public:
	//--------------------------------------------------------------------------------------------
	/// @name Constructors/Destructor
	/// @{
    Image();
    Image(const uchar *data, int width, int height, QImage::Format format);
    Image(const QString& filepath, QImage::Format format = QImage::Format_Invalid);
    Image(const Image& image);
    Image(const QImage& image);
    ~Image() {}
	/// @}

    //--------------------------------------------------------------------------------------------
    /// @name Operators
    /// @{

    Image& operator=(const Image& image);

    /// @}
    //--------------------------------------------------------------------------------------------
    /// @name Properties
    /// @{

    QImage::Format format() const { return m_image.format(); }

    /// @}
	//--------------------------------------------------------------------------------------------
	/// @name Public Methods
	/// @{

    /// @brief Whether or not the image is valid
    bool isNull() const { return m_image.isNull(); }

    /// @brief Return bits of the image
    uchar* buffer() { return m_image.bits(); }
    const uchar* buffer() const { return m_image.bits(); }

    /// @brief Return size of the image
    QSize size() const { return m_image.size(); }

    /// @brief Load .tga image
    /// See: https://forum.qt.io/topic/101971/qimage-and-tga-support-in-c
    /// https://forum.qt.io/topic/74712/qimage-from-tga-with-alpha/11
    QImage loadTga(const char* filePath, bool &success) const;

    /// @brief Perform on removal of this texture resource
    void onRemoval(ResourceCache* cache = nullptr) override;

    /// @brief Size of the image in megabytes
    size_t sizeInMegaBytes() const;

    /// @brief Save the image to a file
    bool save(const QString& filename, const char* format = 0, int quality = -1);

	/// @}
protected:
    //--------------------------------------------------------------------------------------------
    /// @name Friends
    /// @{

    friend class Texture;
    friend class CubeTexture;
    friend class ResourceCache;

    /// @}

    //--------------------------------------------------------------------------------------------
    /// @name Private Methods
    /// @{


    /// @}

    //--------------------------------------------------------------------------------------------
    /// @name Private Members
    /// @{

    /// @brief The QImage used by this image
    QImage m_image;

    /// @}

};


/////////////////////////////////////////////////////////////////////////////////////////////
} // End namespaces

#endif