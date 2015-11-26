#include "fwRenderOgre/compositor/SaoCompositorChainManager.hpp"

#include <fwCore/spyLog.hpp>

#include <fwCore/spyLog.hpp>

namespace fwRenderOgre {

//-----------------------------------------------------------------------------

SaoCompositorChainManager::SaoCompositorChainManager(): m_ogreViewport(0)
{
    m_saoChain.push_back("MipMap");
//    m_saoChain.push_back("Test");
    m_saoChain.push_back("AO_Samples");
    // create MipMap texture
}

SaoCompositorChainManager::SaoCompositorChainManager(::Ogre::Viewport* viewport): m_ogreViewport(viewport)
{
    // create the chain
    m_saoChain.push_back("MipMap");
    //here add the second compositor
//    m_saoChain.push_back("Test");
    m_saoChain.push_back("AO_Samples");

}



// This method can let the manager change the content of the original Ogre's compositor chain
::Ogre::CompositorManager* SaoCompositorChainManager::getCompositorManager()
{
    return ::Ogre::CompositorManager::getSingletonPtr();
}


// fonction that set the Sao Compositor
void SaoCompositorChainManager::setSaoState(bool state)
{
    std::cout << "try to change the state of the sao manager" << std::endl;


    // get the Compositor Manager of Ogre (of the scene)
    ::Ogre::CompositorManager* compositorManager = this->getCompositorManager();


    // if here, we add the compositor present of the chain in the Ogre chain compositor
    if (state)
    {
        // check the content of the Ogre Compositor chain

        ::Ogre::CompositorChain* compChain =
            ::Ogre::CompositorManager::getSingletonPtr()->getCompositorChain(m_ogreViewport);
        ::Ogre::CompositorChain::InstanceIterator compIter = compChain->getCompositors();

        // vérifier que les compositors MipMap et Test sont bien dans la liste
        if (compChain->getCompositor("MipMap") == nullptr)
            compositorManager->addCompositor(m_ogreViewport,"MipMap");

//        if (compChain->getCompositor("Test") == nullptr)
//            compositorManager->addCompositor(m_ogreViewport,"Test");

        if (compChain->getCompositor("AO_Samples") == nullptr)
            compositorManager->addCompositor(m_ogreViewport,"AO_Samples");


        compChain->getCompositor("AO_Samples")->addListener(new SaoListener(m_ogreViewport));

        // désactivation des autres compositors -> uniquement pour les tests, surement non nécessaire
//        while( compIter.hasMoreElements())
//        {
//            ::Ogre::CompositorInstance* targetComp = compIter.getNext();
//            std::cout << targetComp->getCompositor()->getName() << std::endl;
//            if (targetComp->getEnabled())
//                targetComp->setEnabled(false);
//  //              std::cout << "      Enable" << std::endl;
//        }


        compIter = compChain->getCompositors();


        // add all the compositor of the chain
        std::cout << "try to enable the sao chain" << std::endl;
        for(SaoCompositorIdType compositorName : m_saoChain)
        {
            std::cout << "compositor found in the sao chain" << std::endl;
            if(this->getCompositorManager()->resourceExists(compositorName))
            {
                std::cout << "Le compositor " << static_cast<std::string>(compositorName) << " existe et a correctement été chargé " << std::endl;
                //compositorManager->addCompositor(m_ogreViewport, compositorName);
                compositorManager->setCompositorEnabled(m_ogreViewport, compositorName, true);
            }
            else
                OSLM_WARN("\"" << compositorName << "\" does not refer to an existing compositor");

        }

    }
    else // disable the sao Chain
    {
        for(SaoCompositorIdType compositorName : m_saoChain)
        {
            if(this->getCompositorManager()->resourceExists(compositorName))
            {
                compositorManager->setCompositorEnabled(m_ogreViewport, compositorName, false);
            }
        }
    }
}


}// namespace fwRenderOgre
