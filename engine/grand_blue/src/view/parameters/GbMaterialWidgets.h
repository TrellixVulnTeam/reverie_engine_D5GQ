#ifndef GB_MATERIAL_WIDGETS_H 
#define GB_MATERIAL_WIDGETS_H
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Project
#include "GbParameterWidgets.h"
#include "../../core/mixins/GbRenderable.h"
#include "../tree/GbTreeWidget.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Namespace Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Gb {

class ResourceHandle;

namespace View {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Class Declarations
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class LoadTextureWidget
class LoadTextureWidget : public ParameterWidget {
public:
    //-----------------------------------------------------------------------------------------------------------------
    /// @name Constructors and Destructors
    /// @{
    LoadTextureWidget(CoreEngine* core, QWidget* parent = nullptr);
    virtual ~LoadTextureWidget();

    /// @}
    //-----------------------------------------------------------------------------------------------------------------
    /// @name Public Methods
    /// @{

    virtual void update() override {
    }

    /// @}

protected:
    friend class LoadTextureCommand;

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Protected Methods
    /// @{

    virtual void initializeWidgets() override;
    virtual void initializeConnections() override;
    virtual void layoutWidgets() override;

    /// @}


    //-----------------------------------------------------------------------------------------------------------------
    /// @name Protected Members
    /// @{

    FileLoadWidget* m_fileLoadWidget;
    QDialogButtonBox* m_confirmButtons;

    QString m_fileName;
    Uuid m_textureHandleID = Uuid(false); // null UUID

    /// @}

};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // View
} // Gb

#endif