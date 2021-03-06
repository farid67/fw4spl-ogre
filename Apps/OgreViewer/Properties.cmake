
set( NAME OgreViewer )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    extDataReg
    ctrlSelection
    ctrlCamp
    gui
    guiQt
    ioData
    ioAtoms
    io
    ioVTK
    servicesReg
    uiVisuQt
    uiImageQt
    uiReconstructionQt
    uiMedDataQt
    uiIO
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    launcher
    appXml
    arMedia
    arDataReg
    visuOgre
    visuOgreQt
    visuOgreAdaptor
    uiVisuOgre
    material
    media
    preferences
    )

bundleParam(appXml PARAM_LIST config PARAM_VALUES OgreViewer_Extension)
