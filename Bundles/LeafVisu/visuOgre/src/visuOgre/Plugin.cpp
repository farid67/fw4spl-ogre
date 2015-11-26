/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwRenderOgre/Utils.hpp>

#include "visuOgre/Plugin.hpp"

#include <OgreLogManager.h>

namespace visuOgre
{

//------------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("visuOgre::Plugin");

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

Plugin::~Plugin() throw()
{
}

//------------------------------------------------------------------------------

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    // Redirect Ogre Log to FW4SPL Log
//    ::Ogre::LogManager *logMgr = new ::Ogre::LogManager();
//    ::Ogre::Log *log           = logMgr->createLog("home/farid/Ogre.log", true, false, false);
//    log->addListener(new F4SOgreListener());
//    log->setLogDetail(::Ogre::LL_BOREME);
}

//------------------------------------------------------------------------------

void Plugin::stop() throw()
{
}

//------------------------------------------------------------------------------

void F4SOgreListener::messageLogged(const::Ogre::String &_message, ::Ogre::LogMessageLevel _lml, bool _maskDebug,
                                    const::Ogre::String &_logName, bool &_skipThisMessage)
{
    if (!_skipThisMessage)
    {
        switch(_lml)
        {
            case ::Ogre::LML_TRIVIAL:
                SLM_INFO(_message);
                break;
            case ::Ogre::LML_NORMAL:
                SLM_DEBUG(_message);
                break;
            case ::Ogre::LML_CRITICAL:
                SLM_ERROR(_message);
                break;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace operators
