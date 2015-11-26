/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_LAYER_HPP__
#define __FWRENDEROGRE_LAYER_HPP__

#include <fwCore/BaseObject.hpp>

#include <fwCom/HasSignals.hpp>
#include <fwCom/HasSlots.hpp>
#include <fwCom/Slot.hpp>

#include <fwRenderOgre/IRenderWindowInteractorManager.hpp>
#include <fwRenderOgre/interactor/IMovementInteractor.hpp>
#include <fwRenderOgre/interactor/IPickerInteractor.hpp>
#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/compositor/CompositorChainManager.hpp>
#include <fwRenderOgre/compositor/DefaultCompositor.hpp>
#include <fwRenderOgre/compositor/SaoCompositorChainManager.hpp>

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreViewport.h>

#include <vector>

#include "fwRenderOgre/config.hpp"

fwCorePredeclare( (fwRenderOgre)(SRender) )

namespace fwRenderOgre
{

/**
 * @class Layer
 * Allows to render multiple scenes in the same render window with viewports
 */
class FWRENDEROGRE_CLASS_API Layer : public ::fwCore::BaseObject,
                                     public ::fwCom::HasSignals,
                                     public ::fwCom::HasSlots

{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (Layer)(::fwRenderOgre::Layer), (()), new Layer);
    fwCoreAllowSharedFromThis();

    /**@name Signals API
     * @{
     */

    FWRENDEROGRE_API static const ::fwCom::Signals::SignalKeyType s_INIT_LAYER_SIG;
    typedef ::fwCom::Signal<void (::fwRenderOgre::Layer::sptr)> InitLayerSignalType;

    /** @} */

    /**
     * @name Slots API
     * @{
     */
    typedef ::fwCom::Slot< void (::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo) > InteractionSlotType;
    typedef ::fwCom::Slot< void () > DestroySlotType;

    /// Slot: Request the picker to do a ray cast according to the passed position
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_INTERACTION_SLOT;

    /// Slot: Request the deletion of the scene manager
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_DESTROY_SLOT;

    /// Slot: Request the reset of camera
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_RESET_CAMERA_SLOT;

    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_USE_CELSHADING_SLOT;


    /** @} */

    FWRENDEROGRE_API Layer();
    FWRENDEROGRE_API virtual ~Layer();

    /**
     * @brief setRenderWindow
     * Set the render window containing this layer
     */
    FWRENDEROGRE_API void setRenderWindow(::Ogre::RenderWindow* renderWindow);
    /**
     * @brief setID
     * Set the associated scene manager ID of this viewport
     */
    FWRENDEROGRE_API void setID(const std::string& id);
    FWRENDEROGRE_API const std::string& getID() const;

    /// Get the scene manager associated to this viewport
    FWRENDEROGRE_API ::Ogre::SceneManager* getSceneManager() const;

    /// Create the scene
    FWRENDEROGRE_API void createScene();

    /// Add a disabled compositor name to the CompositorChainManager
    FWRENDEROGRE_API void addAvailableCompositor(std::string compositorName);

    // Removes all available compositors from the CompositorChainManager
    FWRENDEROGRE_API void clearAvailableCompositors();

    /// Enables/Disables a compositor according to the isEnabled flag
    FWRENDEROGRE_API void updateCompositorState(std::string compositorName, bool isEnabled);

    /// Place and align camera's focal with the boundingBox
    FWRENDEROGRE_API void resetCameraCoordinates() const;

    /// Reset the camera clipping range (near and far)
    FWRENDEROGRE_API void resetCameraClippingRange() const;

    /// Reset the camera clipping range (near and far)
    FWRENDEROGRE_API void resetCameraClippingRange(const ::Ogre::AxisAlignedBox& worldCoordBoundingBox) const;

    /**
     * @brief Returns Ogre interactor.
     */
    FWRENDEROGRE_API virtual ::fwRenderOgre::interactor::IInteractor::sptr getInteractor();

    /**
     * @brief set Ogre Movement interactor.
     */
    FWRENDEROGRE_API virtual void setMoveInteractor(::fwRenderOgre::interactor::IMovementInteractor::sptr interactor);

    /**
     * @brief set Ogre Select interactor.
     */
    FWRENDEROGRE_API virtual void setSelectInteractor(::fwRenderOgre::interactor::IPickerInteractor::sptr interactor);

    /**
     * @brief get Ogre Movement interactor.
     */
    FWRENDEROGRE_API virtual ::fwRenderOgre::interactor::IMovementInteractor::sptr getMoveInteractor();

    /**
     * @brief get Ogre Select interactor.
     */
    FWRENDEROGRE_API virtual ::fwRenderOgre::interactor::IPickerInteractor::sptr getSelectInteractor();

    /// Returns m_depth
    FWRENDEROGRE_API int getDepth() const;
    /// Sets m_depth
    FWRENDEROGRE_API void setDepth(int depth);

    /// Returns m_oneLight
    FWRENDEROGRE_API void setOneLight(bool oneLight);

    /// Sets the worker for slots
    FWRENDEROGRE_API void setWorker( const ::fwThread::Worker::sptr& _worker);

    /// Sets the render service
    FWRENDEROGRE_API void setRenderService( const SPTR(::fwRenderOgre::SRender)& _service );

    FWRENDEROGRE_API void doRayCast(int x, int y, int width, int height);

    FWRENDEROGRE_API ::fwRenderOgre::interactor::IInteractor getInteractor(std::string type);

    /// Requests render
    FWRENDEROGRE_API void requestRender();

    /// Sets background color : specific to background Layer
    FWRENDEROGRE_API void setBackgroundColor(std::string topColor, std::string botColor);

    /// Sets background scale : specific to background Layer
    FWRENDEROGRE_API void setBackgroundScale(float topScale, float botScale);

    /// Sets if this layer need a layer's 3D scene
    FWRENDEROGRE_API void setDefaultCompositorEnabled(bool hasDefaultCompositor, std::string transparencyTechnique = "",
                                                      std::string useCelShading = "", std::string nbPeel = "");

    /// Sets if this layer has a configured compositor chain
    FWRENDEROGRE_API void setCompositorChainEnabled(bool hasDefaultCompositorChain, std::string compositorChain);

    /// Gets if this layer needs a layer's 3D scene
    FWRENDEROGRE_API bool isDefaultCompositorEnabled();

    /// Gets if there is an XML configured compositor chain
    FWRENDEROGRE_API bool isCompositorChainEnabled();

    /// Checks if this layer has a default compositor
    FWRENDEROGRE_API ::fwRenderOgre::DefaultCompositor::sptr getDefaultCompositor();

    // farid
    // return the sao manager for this layer

    FWRENDEROGRE_API ::fwRenderOgre::SaoCompositorChainManager::sptr getSaoManager();


    FWRENDEROGRE_API CompositorChainManager::CompositorChainType getCompositorChain();

    FWRENDEROGRE_API std::string getFinalChainCompositorName() const;

    FWRENDEROGRE_API ::Ogre::Viewport* getViewport() const;

    FWRENDEROGRE_API bool isSceneCreated() const;

private:
    /// Slot: Interact with the scene
    void interaction(::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo);

    /// Slot: Destroy the scene
    void destroy();

    /// Compute bounding box of the scene
    ::Ogre::AxisAlignedBox computeCameraParameters() const;

    /// Setups default compositor for a layer's 3D scene
    void setupDefaultCompositor();

    /// For a list of semicolon-separated words, returns a vector of these words
    std::vector< std::string > trimSemicolons(std::string input);

    /// Ogre scene manager of this viewport
    ::Ogre::SceneManager* m_sceneManager;

    /// Ogre render window containing this viewport
    ::Ogre::RenderWindow* m_renderWindow;

    /// Ogre viewport representing this layer
    ::Ogre::Viewport* m_viewport;

    /// This boolean enables default compositor's widgets (gui displays before scene creation)
    bool m_hasDefaultCompositor;

    /// Indicates if a compositor chain is attached to the layer
    bool m_hasCompositorChain;

    /// Indicates if the scene has been created
    bool m_sceneCreated;

    /// If there is a configured compositor chain, this attribute stores its raw string
    std::string m_rawCompositorChain;

    /// Ogre default compositor for this layer
    ::fwRenderOgre::DefaultCompositor::sptr m_defaultCompositor;


    /// Ogre default compositor default transparency technique
    transparencyTechnique m_defaultCompositorTransaprencyTechnique;

    /// Ogre default compositor default cel shading behavior
    bool m_defaultCompositorUseCelShading;

    int m_nbPeel;

    /// Manages the list of available compositors.
    /// The names are associated to a boolean value which indicates whether the compositor is enabled or not
    CompositorChainManager m_compositorChainManager;

    // Farid

    /// Ogre sao compositor manager for this layer
    ::fwRenderOgre::SaoCompositorChainManager::sptr m_saoManager;


    /// Z Depth of this viewport
    int m_depth;

    /// This layer has one light only
    bool m_oneLight;

    /// Top background color : specific to background Layer
    std::string m_topColor;
    /// Bottom background color : specific to background Layer
    std::string m_bottomColor;

    /// Top background scale : specific to background Layer
    float m_topScale;
    /// Bottom background scale : specific to background Layer
    float m_bottomScale;

    /// Camera
    ::Ogre::Camera* m_camera;

    /// Ogre movement interactor
    ::fwRenderOgre::interactor::IMovementInteractor::sptr m_moveInteractor;

    /// Ogre picker interactor
    ::fwRenderOgre::interactor::IPickerInteractor::sptr m_selectInteractor;

    ///Connection service, needed for slot/signal association
    ::fwServices::helper::SigSlotConnection::sptr m_connections;

    /// Render service which this layer is attached
    WPTR(::fwRenderOgre::SRender) m_renderService;
};

}

#endif // __FWRENDEROGRE_LAYER_HPP__
