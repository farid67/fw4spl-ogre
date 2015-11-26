#ifndef __FWRENDEROGE_COMPOSITOR_SAOCOMPOSITORCHAINMANAGER_HPP
#define __FWRENDEROGE_COMPOSITOR_SAOCOMPOSITORCHAINMANAGER_HPP


// used to view the content of the Ogre Compositor Chain
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgreCompositor.h>
#include <OGRE/OgrePrerequisites.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreTexture.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgrePass.h>


#include <fwCore/BaseObject.hpp>
#include <fwRenderOgre/config.hpp>
#include <vector>

namespace Ogre
{
class CompositorManager;
class Viewport;
}

namespace fwRenderOgre // on s'insère ici
{
    class FWRENDEROGRE_CLASS_API SaoCompositorChainManager : public ::fwCore::BaseObject
    {
    public:
        fwCoreClassDefinitionsWithFactoryMacro ((SaoCompositorChainManager)(::fwRenderOgre::SaoCompositorChainManager),(()),
                                                new SaoCompositorChainManager);
        fwCoreAllowSharedFromThis(); // permet de générer un pointeur vers la classe souhaitée en la castant correctement

        typedef std::string SaoCompositorIdType; // un compositor ne sera ici qu'une chaine de caractère correspondant à son nom (pas de booléen car toujours activé lorsque la chaine est choisie)
        typedef std::vector<SaoCompositorIdType> SaoCompositorChainType; // liste des

        FWRENDEROGRE_API SaoCompositorChainManager();
        FWRENDEROGRE_API SaoCompositorChainManager(::Ogre::Viewport* ogreViewport);// Chaine de compositor attachée au viewport Ogre

        FWRENDEROGRE_API void setOgreViewport(::Ogre::Viewport* viewport);

        // pas sur -> bouton = enable?
        FWRENDEROGRE_API void setSaoState(bool state);

        // inline function of the end

        FWRENDEROGRE_API SaoCompositorChainType getSaoCompositorChain();

        // Here we add the class derivated from the Ogre Listener
        class FWRENDEROGRE_CLASS_API SaoListener : public ::Ogre::CompositorInstance::Listener
        {
        public:
            SaoListener():m_VP(nullptr){}
            SaoListener(::Ogre::Viewport* vp):m_VP(vp){}
            ~SaoListener(){}
            void notifyMaterialRender(::Ogre::uint32 pass_id, ::Ogre::MaterialPtr& mat)
                // method called before a render_target operation involving a material to set dynamically the material parameters
            {
                //§ When entering in this method, the Mip Map compositor has finished its work

                std::cout << "No? " << std::endl;
                std::cout << mat.get()->getName() << std::endl;


                // ---------------------------------------------------
                //  Try to change the content of the Second Compositor
                // ---------------------------------------------------


                // get a pointer on the precedent compositor


                ::Ogre::CompositorChain* compChain =
                    ::Ogre::CompositorManager::getSingletonPtr()->getCompositorChain(m_VP);
                ::Ogre::CompositorChain::InstanceIterator compIter = compChain->getCompositors();


//                ::Ogre::Compositor *MipMap,*Test;
                ::Ogre::TexturePtr mip0,mip1,mip2,mip3,mip4,mip5,mip6,mip7, rt0;
                while( compIter.hasMoreElements())
                {
                    ::Ogre::CompositorInstance* targetComp = compIter.getNext();
                    if (targetComp->getCompositor()->getName() == "MipMap")
                    {
                        mip0 = targetComp->getTextureInstance("mip0",0);
                        mip1 = targetComp->getTextureInstance("mip1",0);
                        mip2 = targetComp->getTextureInstance("mip2",0);
                        mip3 = targetComp->getTextureInstance("mip3",0);
                        mip4 = targetComp->getTextureInstance("mip4",0);
                        mip5 = targetComp->getTextureInstance("mip5",0);
                        mip6 = targetComp->getTextureInstance("mip6",0);
                        mip7 = targetComp->getTextureInstance("mip7",0);
                    }
                    if (targetComp->getCompositor()->getName() == "AO_Samples")
                    {
                        rt0 = targetComp->getTextureInstance("rt0",0);
                    }
                }


                // ---------------------------------------------------
                //  Copy the content of mip0,mip1... in rt0
                // ---------------------------------------------------


                rt0.get()->freeInternalResources();

                rt0.get()->changeGroupOwnership(mip0.get()->getGroup());
                rt0.get()->setWidth(mip0.get()->getWidth());
                rt0.get()->setHeight(mip0.get()->getHeight());
                rt0.get()->setNumMipmaps(7);
                rt0.get()->setFormat(::Ogre::PixelFormat::PF_FLOAT32_R);
                rt0.get()->setUsage(::Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
                rt0.get()->setTextureType(::Ogre::TextureType::TEX_TYPE_2D);

                rt0.get()->createInternalResources();

                // copy the content of the mip textures in the mipmap (rt0)
                // essayer de le faire de manière automatique avec une boucle + fonction qui récupèrerai l'ensemble des textures de MipMap dans un conteneur

                rt0.get()->getBuffer(0,0)->blit(mip0.get()->getBuffer());
                rt0.get()->getBuffer(0,1)->blit(mip1.get()->getBuffer());
                rt0.get()->getBuffer(0,2)->blit(mip2.get()->getBuffer());
                rt0.get()->getBuffer(0,3)->blit(mip2.get()->getBuffer());
                rt0.get()->getBuffer(0,4)->blit(mip2.get()->getBuffer());
                rt0.get()->getBuffer(0,5)->blit(mip2.get()->getBuffer());
                rt0.get()->getBuffer(0,6)->blit(mip2.get()->getBuffer());
                rt0.get()->getBuffer(0,7)->blit(mip2.get()->getBuffer());


            }

        private:
            ::Ogre::Viewport* m_VP;
        };

    private :
        /// Getter for the Ogre CompositorManager
        ::Ogre::CompositorManager* getCompositorManager();

        /// The parent layer's viewport.
        /// The ogre's compositor manager needs it in order to access the right compositor chain.
        ::Ogre::Viewport* m_ogreViewport;

        SaoCompositorChainType m_saoChain;

//        ::Ogre::TexturePtr m_MipMap_tex;

    };

    //-----------------------------------------------------------------------------
    // Inline method(s)

    inline SaoCompositorChainManager::SaoCompositorChainType SaoCompositorChainManager::getSaoCompositorChain()
    {
        return m_saoChain;
    }

    //-----------------------------------------------------------------------------

    inline void SaoCompositorChainManager::setOgreViewport(::Ogre::Viewport* viewport)
    {
        m_ogreViewport = viewport;
    }

    //-----------------------------------------------------------------------------


}// namespace fwRenderOgre

#endif // __FWRENDEROGE_COMPOSITOR_SAOCOMPOSITORCHAINMANAGER_HPP
