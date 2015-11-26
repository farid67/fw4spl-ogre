/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRenderOgre/Layer.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwDataTools/Color.hpp>

#include <fwRenderOgre/interactor/TrackballInteractor.hpp>
#include <fwRenderOgre/SRender.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwThread/Worker.hpp>

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreColourValue.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreLight.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreVector3.h>
#include <OGRE/OgreException.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreRectangle2D.h>
#include <OGRE/Overlay/OgreOverlay.h>
#include <OGRE/Overlay/OgreOverlayContainer.h>
#include <OGRE/Overlay/OgreOverlayManager.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>

#include <stack>

namespace fwRenderOgre
{

const ::fwCom::Signals::SignalKeyType Layer::s_INIT_LAYER_SIG = "layerInitialized";

const ::fwCom::Slots::SlotKeyType Layer::s_INTERACTION_SLOT    = "interaction";
const ::fwCom::Slots::SlotKeyType Layer::s_DESTROY_SLOT        = "destroy";
const ::fwCom::Slots::SlotKeyType Layer::s_RESET_CAMERA_SLOT   = "resetCamera";
const ::fwCom::Slots::SlotKeyType Layer::s_USE_CELSHADING_SLOT = "useCelShading";

//-----------------------------------------------------------------------------

Layer::Layer() :
    m_sceneManager(nullptr),
    m_renderWindow(nullptr),
    m_viewport(nullptr),
    m_hasDefaultCompositor(false),
    m_hasCompositorChain(false),
    m_sceneCreated(false),
    m_rawCompositorChain(""),
    m_defaultCompositor(nullptr),
    m_defaultCompositorTransaprencyTechnique(DEFAULT),
    m_defaultCompositorUseCelShading(false),
    m_nbPeel(8),
    m_compositorChainManager(),
    m_saoManager(nullptr),
    m_depth(1),
    m_oneLight(false),
    m_topColor("#333333"),
    m_bottomColor("#333333"),
    m_topScale(0.f),
    m_bottomScale(1.f),
    m_camera(nullptr)
{
    newSignal<InitLayerSignalType>(s_INIT_LAYER_SIG);

    newSlot(s_INTERACTION_SLOT, &Layer::interaction, this);
    newSlot(s_DESTROY_SLOT, &Layer::destroy, this);
    newSlot(s_RESET_CAMERA_SLOT, &Layer::resetCameraCoordinates, this);

    //Create the connection slot object
    m_connections = ::fwServices::helper::SigSlotConnection::New();
}

//-----------------------------------------------------------------------------

Layer::~Layer()
{
}

//-----------------------------------------------------------------------------

void Layer::setRenderWindow(::Ogre::RenderWindow* renderWindow)
{
    m_renderWindow = renderWindow;
}

//-----------------------------------------------------------------------------

void Layer::setID(const std::string& id)
{
    m_sceneManager = ::fwRenderOgre::Utils::getOgreRoot()->createSceneManager(::Ogre::ST_GENERIC, id);
    m_sceneManager->addRenderQueueListener( ::fwRenderOgre::Utils::getOverlaySystem() );
}

//-----------------------------------------------------------------------------

const std::string& Layer::getID() const
{
    return m_sceneManager->getName();
}

//-----------------------------------------------------------------------------

::Ogre::SceneManager* Layer::getSceneManager() const
{
    return m_sceneManager;
}

//-----------------------------------------------------------------------------

void Layer::createScene()
{
    SLM_ASSERT("Scene manager must be initialized", m_sceneManager);
    SLM_ASSERT("Render window must be initialized", m_renderWindow);

    m_sceneManager->setAmbientLight(::Ogre::ColourValue(0.8f,0.8f,0.8f));

    // Create the camera
    m_camera = m_sceneManager->createCamera("PlayerCam");
    m_camera->setNearClipDistance(1);

    m_viewport = m_renderWindow->addViewport(m_camera, m_depth);

    m_compositorChainManager.setOgreViewport(m_viewport);

    // Farid -> set the viewport for sao Chain Manager

    m_saoManager = fwRenderOgre::SaoCompositorChainManager::New();
    m_saoManager->setOgreViewport(m_viewport);


    if (m_depth != 0)
    {
        m_viewport->setClearEveryFrame(true, ::Ogre::FBT_DEPTH);
    } // Set the background material
    else
    {
        // FIXME : background isn't show when using compositor with a clear pass
        // We must blend the input previous in each compositor
        ::Ogre::MaterialPtr defaultMaterial = ::Ogre::MaterialManager::getSingleton().getByName("DefaultBackground");
        ::Ogre::MaterialPtr material        = ::Ogre::MaterialManager::getSingleton().create(
            this->getID() + "backgroundMat", ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        defaultMaterial.get()->copyDetailsTo(material);

        std::uint8_t color[4];
        ::Ogre::Pass* pass                           = material->getTechnique(0)->getPass(0);
        ::Ogre::GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();

        ::fwDataTools::Color::hexaStringToRGBA(m_topColor, color);
        ::Ogre::ColourValue ogreTopColor(color[0] / 255.f,
                                         color[1] / 255.f,
                                         color[2] / 255.f,
                                         1.f);
        params->setNamedConstant("topColour", ogreTopColor);

        ::fwDataTools::Color::hexaStringToRGBA(m_bottomColor, color);
        ::Ogre::ColourValue ogreBotColor(color[0] / 255.f,
                                         color[1] / 255.f,
                                         color[2] / 255.f,
                                         1.f);
        params->setNamedConstant("bottomColour", ogreBotColor);

        params->setNamedConstant("topScale", m_topScale);
        params->setNamedConstant("bottomScale", m_bottomScale);

        // Create background rectangle covering the whole screen
        ::Ogre::Rectangle2D* rect = new ::Ogre::Rectangle2D();
        rect->setCorners(-1.0, 1.0, 1.0, -1.0);
        rect->setMaterial(this->getID() + "backgroundMat");

        // Render the background before everything else
        rect->setRenderQueueGroup(::Ogre::RENDER_QUEUE_BACKGROUND);

        // Use infinite AAB to always stay visible
        ::Ogre::AxisAlignedBox aabInf;
        aabInf.setInfinite();
        rect->setBoundingBox(aabInf);

        // Attach background to the scene
        ::Ogre::SceneNode* node = m_sceneManager->getRootSceneNode()->createChildSceneNode("Background");
        node->attachObject(rect);
    }

    // Alter the camera aspect ratio to match the viewport
    m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / ::Ogre::Real(m_viewport->getActualHeight()));

    // We need two lights currently because of the meshes data we use
    // Most of them do not have normals - when we generate them we rely on the point order to
    // compute the normal orientation. Some are clockwise, others are anti-clockwise
    // Thus we need to take that into account, even if it would be better to fix data
    // A light pass is not free in a forward renderer !

    ::Ogre::Light* light = m_sceneManager->createLight("MainLight");
    light->setType(::Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(::Ogre::Vector3(0,0,1));
    light->setDiffuseColour(::Ogre::ColourValue());
    light->setSpecularColour(::Ogre::ColourValue());

    // Creating Camera Scene Node
    ::Ogre::SceneNode* cameraNode = m_sceneManager->getRootSceneNode()->createChildSceneNode("CameraNode");
    cameraNode->setPosition(Ogre::Vector3(0,0,5));
    cameraNode->lookAt(Ogre::Vector3(0,0,1), ::Ogre::Node::TS_WORLD);

    // Attach Camera and Headlight to fit vtk light
    cameraNode->attachObject(m_camera);
    cameraNode->attachObject(light);

    if(!m_oneLight)
    {
        ::Ogre::Light* secondlight = m_sceneManager->createLight("SecondLight");
        secondlight->setType(::Ogre::Light::LT_DIRECTIONAL);
        secondlight->setDirection(::Ogre::Vector3(0,0,-1));
        secondlight->setDiffuseColour(::Ogre::ColourValue());
        secondlight->setSpecularColour(::Ogre::ColourValue());
        cameraNode->attachObject(secondlight);
    }

    // If there is any interactor adaptor in xml, m_moveInteractor will be overwritten by InteractorStyle adaptor
    ::fwRenderOgre::interactor::IMovementInteractor::sptr interactor =
        ::fwRenderOgre::interactor::IMovementInteractor::dynamicCast (
            ::fwRenderOgre::interactorFactory::New("::fwRenderOgre::interactor::TrackballInteractor"));

    interactor->setSceneID(m_sceneManager->getName());

    // Set compositor
    //this->addCompositor(m_compositorName);
    //this->setCompositorEnabled(m_compositorName);

    if(m_hasDefaultCompositor)
    {
        // Farid -> to suppress
        std::cout << "Scene has Default Compositor " << std::endl;
        this->setupDefaultCompositor();
    }

    if(m_hasCompositorChain)
    {
        // Farid -> to suppress
        std::cout << "Scene has Compositor chain " << std::endl;
        m_compositorChainManager.setCompositorChain(this->trimSemicolons(m_rawCompositorChain));
    }

    this->setMoveInteractor(interactor);

    m_sceneCreated = true;

    this->signal<InitLayerSignalType>(s_INIT_LAYER_SIG)->asyncEmit(this->getSptr());

}

// ----------------------------------------------------------------------------

void Layer::addAvailableCompositor(std::string compositorName)
{
    m_compositorChainManager.addAvailableCompositor(compositorName);
}

// ----------------------------------------------------------------------------

void Layer::clearAvailableCompositors()
{
    m_renderService.lock()->makeCurrent();
    m_compositorChainManager.clearCompositorChain();
    m_renderService.lock()->requestRender();
}

// ----------------------------------------------------------------------------

void Layer::updateCompositorState(std::string compositorName, bool isEnabled)
{
    std::cout << "The following compositor try to be enable " << compositorName << std::endl;
    m_renderService.lock()->makeCurrent();
    m_compositorChainManager.updateCompositorState(compositorName, isEnabled);
    m_renderService.lock()->requestRender();
}

// ----------------------------------------------------------------------------

void Layer::interaction(::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo info)
{
    switch(info.interactionType)
    {
        case ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::MOUSEMOVE:
        {
            m_moveInteractor->mouseMoveEvent(info.dx, info.dy);
            break;
        }
        case ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::HORIZONTALMOVE:
        {
            m_moveInteractor->horizontalMoveEvent(info.x, info.dx);
            break;
        }
        case ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::VERTICALMOVE:
        {
            m_moveInteractor->verticalMoveEvent(info.y, info.dy);
            break;
        }
        case ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::WHEELMOVE:
        {
            m_moveInteractor->wheelEvent(info.delta, info.x, info.y);
            break;
        }
        case ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::RESIZE:
        {
            m_moveInteractor->resizeEvent(info.x, info.y);
            break;
        }
        case ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::KEYPRESS:
        {
            m_moveInteractor->keyPressEvent(info.key);
            break;
        }
    }
}

// ----------------------------------------------------------------------------

void Layer::destroy()
{
    ::fwRenderOgre::Utils::getOgreRoot()->destroySceneManager(m_sceneManager);
    m_sceneManager = nullptr;
    m_camera       = nullptr;
}

// ----------------------------------------------------------------------------

int Layer::getDepth() const
{
    return m_depth;
}

// ----------------------------------------------------------------------------

void Layer::setDepth(int depth)
{
    m_depth = depth;
}

// ----------------------------------------------------------------------------

void Layer::setOneLight(bool oneLight)
{
    m_oneLight = oneLight;
}

// ----------------------------------------------------------------------------

void Layer::setWorker( const ::fwThread::Worker::sptr& _worker)
{
    ::fwCom::HasSlots::m_slots.setWorker(_worker);
}

//------------------------------------------------------------------------------

void Layer::setRenderService( const ::fwRenderOgre::SRender::sptr& _service)
{
    SLM_ASSERT("service not instanced", _service);

    m_renderService = _service;
}

// ----------------------------------------------------------------------------

::fwRenderOgre::interactor::IInteractor::sptr Layer::getInteractor()
{
    return m_moveInteractor;
}

// ----------------------------------------------------------------------------

void Layer::setMoveInteractor(::fwRenderOgre::interactor::IMovementInteractor::sptr interactor)
{
    m_connections->disconnect();

    m_moveInteractor = interactor;

    m_connections->connect(interactor, ::fwRenderOgre::interactor::IMovementInteractor::s_RESET_CAMERA_SIG,
                           this->getSptr(), s_RESET_CAMERA_SLOT);
}

// ----------------------------------------------------------------------------

void Layer::setSelectInteractor(::fwRenderOgre::interactor::IPickerInteractor::sptr interactor)
{
    m_selectInteractor = interactor;
}

// ----------------------------------------------------------------------------

::fwRenderOgre::interactor::IMovementInteractor::sptr Layer::getMoveInteractor()
{

    return m_moveInteractor;

}

// ----------------------------------------------------------------------------

::fwRenderOgre::interactor::IPickerInteractor::sptr Layer::getSelectInteractor()
{
    return m_selectInteractor;
}

//-----------------------------------------------------------------------------

::Ogre::AxisAlignedBox Layer::computeCameraParameters() const
{
    // The bounding box in which all the object's bounding boxes will be merged
    ::Ogre::AxisAlignedBox worldCoordBoundingBox;

    // Getting this render service scene manager
    ::Ogre::SceneNode *rootSceneNode = this->getSceneManager()->getRootSceneNode();

    // Needed to recompute world bounding boxes
    rootSceneNode->_update(true, false);

    // This stack will be used for iterate through the scene nodes graph
    ::std::stack< const ::Ogre::SceneNode* > childrenStack;
    childrenStack.push(rootSceneNode);

    while(!childrenStack.empty())
    {
        const ::Ogre::SceneNode *tempSceneNode = childrenStack.top();
        childrenStack.pop();

        // Retrieves an iterator pointing to the attached movable objects of the current scene node
        ::Ogre::SceneNode::ConstObjectIterator entitiesIt = tempSceneNode->getAttachedObjectIterator();
        while(entitiesIt.hasMoreElements())
        {
            // First, we must cast the MovableObject* into an Entity*
            const ::Ogre::Entity* e = dynamic_cast< ::Ogre::Entity* > (entitiesIt.getNext());

            if(e)
            {
                // The current entity's bounding box is merged into the "world" bounding box
                worldCoordBoundingBox.merge(e->getWorldBoundingBox());
            }
        }

        // Retrieves an iterator pointing to the child nodes of the current scene node
        ::Ogre::SceneNode::ConstChildNodeIterator childNodesIt = tempSceneNode->getChildIterator();
        while(childNodesIt.hasMoreElements())
        {
            // First, we must cast the Node* into a SceneNode*
            const ::Ogre::SceneNode *childNode = dynamic_cast< ::Ogre::SceneNode* >(childNodesIt.getNext());
            if(childNode)
            {
                // Push the current node into the stack in order to continue iteration
                childrenStack.push(childNode);
            }
        }
    }
    return worldCoordBoundingBox;
}

//-----------------------------------------------------------------------------

void Layer::resetCameraCoordinates() const
{
    ::Ogre::AxisAlignedBox worldCoordBoundingBox = computeCameraParameters();

    if(m_camera && m_camera->getProjectionType() == ::Ogre::PT_PERSPECTIVE)
    {
        // Check if bounding box is valid, otherwise, do nothing.
        if( worldCoordBoundingBox == ::Ogre::AxisAlignedBox::EXTENT_NULL ||
            worldCoordBoundingBox == ::Ogre::AxisAlignedBox::EXTENT_INFINITE)
        {
            ::Ogre::SceneNode* camNode = m_camera->getParentSceneNode();

            camNode->setPosition(0.f, 0.f, 0.f);
        }
        else
        {
            ::Ogre::SceneNode* camNode = m_camera->getParentSceneNode();
            // Arbitrary coefficient
            ::Ogre::Real boundingBoxLength = worldCoordBoundingBox.getSize().length() > 0 ?
                                             worldCoordBoundingBox.getSize().length() : 0;

            float coeffZoom = boundingBoxLength * 3;
            OSLM_DEBUG("Zoom coefficient : " << coeffZoom);

            // Set the direction of the camera
            ::Ogre::Quaternion quat   = camNode->getOrientation();
            ::Ogre::Vector3 direction = quat.zAxis();

            // Set the position of the camera
            camNode->setPosition((worldCoordBoundingBox.getCenter() + coeffZoom * direction ) );

            // Update interactor's mouse scale
            m_moveInteractor->setMouseScale( coeffZoom );

            resetCameraClippingRange(worldCoordBoundingBox);
        }


        m_renderService.lock()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void Layer::resetCameraClippingRange() const
{
    resetCameraClippingRange(computeCameraParameters());
}

//-----------------------------------------------------------------------------

void Layer::resetCameraClippingRange(const ::Ogre::AxisAlignedBox& worldCoordBoundingBox) const
{
    if(m_camera && m_camera->getProjectionType() == ::Ogre::PT_PERSPECTIVE)
    {
        // Check if bounding box is valid, otherwise, do nothing.
        if( worldCoordBoundingBox == ::Ogre::AxisAlignedBox::EXTENT_NULL ||
            worldCoordBoundingBox == ::Ogre::AxisAlignedBox::EXTENT_INFINITE)
        {
            return;
        }

        ::Ogre::SceneNode* camNode = m_camera->getParentSceneNode();

        // Set the direction of the camera
        ::Ogre::Quaternion quat   = camNode->getOrientation();
        ::Ogre::Vector3 direction = quat.zAxis();
        ::Ogre::Vector3 position  = camNode->getPosition();

        // Set near and far plan
        ::Ogre::Vector3 minimum = worldCoordBoundingBox.getMinimum();
        ::Ogre::Vector3 maximum = worldCoordBoundingBox.getMaximum();

        float a, b, c, d;
        a = -direction.x;
        b = -direction.y;
        c = -direction.z;
        d = -(a*position.x + b*position.y + c*position.z);

        // Max near and min far
        float maxNear = a*minimum.x + b*minimum.y + c*minimum.z + d;
        float minFar  = 1e-18f;
        float dist;

        float corners[6];
        corners[0] = minimum.x;
        corners[1] = maximum.x;
        corners[2] = minimum.y;
        corners[3] = maximum.y;
        corners[4] = minimum.z;
        corners[5] = maximum.z;
        // Find the closest / farthest bounding box vertex
        for (int k = 0; k < 2; k++ )
        {
            for (int j = 0; j < 2; j++ )
            {
                for (int i = 0; i < 2; i++ )
                {
                    dist    = a*corners[i] + b*corners[2+j] + c*corners[4+k] + d;
                    maxNear = (dist<maxNear) ? dist : maxNear;
                    minFar  = (dist>minFar) ? dist : minFar;
                }
            }
        }


        // Give ourselves a little breathing room
        maxNear = 0.99f*maxNear - (minFar - maxNear)*0.5f;
        minFar  = 1.01f*minFar + (minFar - maxNear)*0.5f;

        // Do not let the range behind the camera throw off the calculation.
        if (maxNear < 0.1f)
        {
            maxNear = 0.1f;
        }

        // Make sure near is not bigger than far
        maxNear = (maxNear >= minFar) ? (0.01f*minFar) : (maxNear);

        m_camera->setNearClipDistance( 1 );
        m_camera->setFarClipDistance( 10000 );
    }
}

//-----------------------------------------------------------------------------

void Layer::doRayCast(int x, int y, int width, int height)
{
    if(m_selectInteractor)
    {
        OSLM_ASSERT("SelectInteractor Isn't initialized, add the adaptor to your xml file.",m_selectInteractor);

        if(!m_selectInteractor->isPickerInitialized())
        {
            m_selectInteractor->initPicker();
        }

        m_selectInteractor->mouseClickEvent(x, y, width, height);
    }
}

//-----------------------------------------------------------------------------

void Layer::requestRender()
{
    m_renderService.lock()->requestRender();
}

//-----------------------------------------------------------------------------

void Layer::setBackgroundColor(std::string topColor, std::string botColor)
{
    m_topColor    = topColor;
    m_bottomColor = botColor;
}

//-----------------------------------------------------------------------------

void Layer::setBackgroundScale(float topScale, float botScale)
{
    m_topScale    = topScale;
    m_bottomScale = botScale;
}

//-------------------------------------------------------------------------------------

void Layer::setDefaultCompositorEnabled(bool hasDefaultCompositor, std::string transparencyTechnique,
                                        std::string useCelShading, std::string nbPeel)
{
    m_hasDefaultCompositor = hasDefaultCompositor;
    if(transparencyTechnique != "")
    {
        if(transparencyTechnique == "DepthPeeling")
        {
            m_defaultCompositorTransaprencyTechnique = DEPTHPEELING;
        }
        else if(transparencyTechnique == "DualDepthPeeling")
        {
            m_defaultCompositorTransaprencyTechnique = DUALDEPTHPEELING;
        }
        else if(transparencyTechnique == "WeightedBlendedOIT")
        {
            m_defaultCompositorTransaprencyTechnique = WEIGHTEDBLENDEDOIT;
        }
        else if(transparencyTechnique == "HybridTransparency")
        {
            m_defaultCompositorTransaprencyTechnique = HYBRIDTRANSPARENCY;
        }
    }
    if(useCelShading != "")
    {
        if(useCelShading == "yes")
        {
            m_defaultCompositorUseCelShading = true;
        }
    }
    if(nbPeel != "")
    {
        m_nbPeel = atoi(nbPeel.c_str());
    }
}

//-------------------------------------------------------------------------------------

void Layer::setCompositorChainEnabled(bool hasDefaultCompositorChain, std::string compositorChain)
{

    m_hasCompositorChain = hasDefaultCompositorChain;

    if(m_hasCompositorChain)
    {
        m_rawCompositorChain = compositorChain;
    }
}

//-------------------------------------------------------------------------------------

bool Layer::isDefaultCompositorEnabled()
{
    return m_hasDefaultCompositor;
}

//-------------------------------------------------------------------------------------

bool Layer::isCompositorChainEnabled()
{
    return m_hasCompositorChain;
}

//-------------------------------------------------------------------------------------

void Layer::setupDefaultCompositor()
{
    // Needed to setup compositors in GL3Plus, Ogre creates render targets
    m_renderService.lock()->makeCurrent();

    m_defaultCompositor = fwRenderOgre::DefaultCompositor::New();
    m_defaultCompositor->setViewport(m_viewport);
    m_defaultCompositor->setTransparencyTechnique(m_defaultCompositorTransaprencyTechnique);
    m_defaultCompositor->setCelShadingActivated(m_defaultCompositorUseCelShading);
    m_defaultCompositor->setTransparencyDepth(m_nbPeel);
    m_defaultCompositor->update();
}

//-------------------------------------------------------------------------------------

std::vector< std::string > Layer::trimSemicolons(std::string input)
{
    std::vector< std::string > elements;
    std::string currentElement("");

    for(unsigned long i(0); i < input.size(); ++i)
    {
        char c = input[i];

        if(c != ';')
        {
            currentElement += c;

            if(input.size() == i + 1)
            {
                elements.push_back(currentElement);
            }
        }
        else
        {
            elements.push_back(currentElement);
            currentElement = "";
        }
    }

    return elements;
}

//-------------------------------------------------------------------------------------

::fwRenderOgre::DefaultCompositor::sptr Layer::getDefaultCompositor()
{
    return m_defaultCompositor;
}

//-------------------------------------------------------------------------------------

::fwRenderOgre::SaoCompositorChainManager::sptr Layer::getSaoManager()
{
    return m_saoManager;
}

//-------------------------------------------------------------------------------------


CompositorChainManager::CompositorChainType Layer::getCompositorChain()
{
    return m_compositorChainManager.getCompositorChain();
}

//-------------------------------------------------------------------------------------

std::string Layer::getFinalChainCompositorName() const
{
    return CompositorChainManager::FINAL_CHAIN_COMPOSITOR;
}

//-------------------------------------------------------------------------------------

::Ogre::Viewport* Layer::getViewport() const
{
    return m_viewport;
}

//-------------------------------------------------------------------------------------

bool Layer::isSceneCreated() const
{
    return m_sceneCreated;
}

//-------------------------------------------------------------------------------------

} // namespace fwRenderOgre
