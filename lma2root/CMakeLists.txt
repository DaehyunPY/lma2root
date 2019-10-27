CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
PROJECT(lma2root VERSION 2019.10.0 LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 11)
IF(MSVC)
    ADD_DEFINITIONS(/MP /FS)
ENDIF(MSVC)

### Link ROOT
LIST(APPEND CMAKE_PREFIX_PATH "$ENV{ROOTSYS}")
FIND_PACKAGE(ROOT 6.16 CONFIG REQUIRED COMPONENTS Gui)
INCLUDE(${ROOT_USE_FILE})

### Link resort
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib)
IF(MSVC)
    LINK_LIBRARIES(libResort64c-win.lib)
ENDIF(MSVC)

### Build lma2root
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
SET(HEADERS_DICT
    MyEvent/MyOriginalEvent/MyOriginalChannel/MyOriginalChannel.h
    MyEvent/MyOriginalEvent/MyOriginalChannel/MyOriginalChannelInfo.h
    MyEvent/MyOriginalEvent/MyOriginalEvent.h
    MyEvent/MyOriginalEvent/MyOriginalEventInfo.h
    MyEvent/MyOriginalEvent/MyPuls/MyPuls.h
    MyEvent/MySignalAnalyzedEvent/MyPeak/MyPeak.h
    MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedChannel/MyChannelSection.h
    MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedChannel/MySignalAnalyzedChannel.h
    MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedChannel/MySignalAnalyzedChannelInfo.h
    MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedEvent.h
    MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedEventInfo.h
    MyEvent/MySignalAnalyzedEvent/MyVoltage/MyVoltage.h
    MyEvent/MySortedEvent/MyDetektor/MyDetektor.h
    MyEvent/MySortedEvent/MyDetektor/MyDetektorHit.h
    MyEvent/MySortedEvent/MyDetektor/MyDetektorInfo.h
    MyEvent/MySortedEvent/MyDetektor/MyLayerProperty.h
    MyEvent/MySortedEvent/MyDetektor/MyTimeRange.h
    MyEvent/MySortedEvent/MyPunkt/MyPunkt.h
    MyEvent/MySortedEvent/MySortedEvent.h
    MyEvent/MySortedEvent/MySortedEventInfo.h
)
FOREACH(HEADER ${HEADERS_DICT})
    GET_FILENAME_COMPONENT(BASE ${HEADER} NAME_WE)
    ROOT_GENERATE_DICTIONARY(G__${BASE} ${HEADER})
    SET(SRCS_DICT ${SRCS_DICT} G__${BASE}.cxx)
ENDFOREACH(HEADER)
SET(HEADERS_MAIN
    BinaryDump.h
    MyArchive/MyArchive.h
    MyDetektorHitSorter/helperfunctionsforDetSorter.h
    MyDetektorHitSorter/MyDetektorHitSorter.h
    MyDetektorHitSorter/MyDetektorHitSorterAchim.h
    MyDetektorHitSorter/MyDetektorHitSorterAchimHex.h
    MyDetektorHitSorter/MyDetektorHitSorterAchimQuad.h
    MyDetektorHitSorter/MyDetektorHitSorterHex.h
    MyDetektorHitSorter/MyDetektorHitSorterQuad.h
    MyDetektorHitSorter/MyDetektorHitSorterSimple.h
    MyEventViewer/MyEventViewer.h
    MyFileProcessor/MyFileProcessor.h
    MyPulsHistoFiller/MyPulsHistoFiller.h
    MyRootManager/MyHistos.h
    MyRootManager/MyRootManager.h
    MySettings/MySettings.h
    MySignalAnalyzer/baselineCorrection.h
    MySignalAnalyzer/helperfunctionsforSTDC.h
    MySignalAnalyzer/MySignalAnalyzer.h
    MySignalAnalyzer/MySoftTDC.h
    MySignalAnalyzer/MySoftTDCCFD.h
    MySignalAnalyzer/MySoftTDCCoM.h
    # ThreadStuff/MyQueue.h
    # ThreadStuff/MySemaphore.h
    # ThreadStuff/MySoftTDCCFDWithNonWorkingThreats.h
    # ThreadStuff/MySoftTDCCoMWithNonWorkingThreats.h
    # ThreadStuff/MySoftTDCWithNonWorkingThreats.h
)
SET(SRCS_MAIN
    BinaryDump.cpp
    main.cpp
    MyArchive/MyArchive.cpp
    MyDetektorHitSorter/helperfunctionsforDetSorter.cpp
    MyDetektorHitSorter/MyDetektorHitSorter.cpp
    MyDetektorHitSorter/MyDetektorHitSorterAchim.cpp
    MyDetektorHitSorter/MyDetektorHitSorterAchimHex.cpp
    MyDetektorHitSorter/MyDetektorHitSorterAchimQuad.cpp
    MyDetektorHitSorter/MyDetektorHitSorterHex.cpp
    MyDetektorHitSorter/MyDetektorHitSorterQuad.cpp
    MyDetektorHitSorter/MyDetektorHitSorterSimple.cpp
    MyEvent/MyOriginalEvent/MyOriginalChannel/MyOriginalChannel.cpp
    MyEvent/MyOriginalEvent/MyOriginalChannel/MyOriginalChannelInfo.cpp
    MyEvent/MyOriginalEvent/MyOriginalEvent.cpp
    MyEvent/MyOriginalEvent/MyOriginalEventInfo.cpp
    MyEvent/MyOriginalEvent/MyPuls/MyPuls.cpp
    MyEvent/MySignalAnalyzedEvent/MyPeak/MyPeak.cpp
    MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedChannel/MyChannelSection.cpp
    MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedChannel/MySignalAnalyzedChannel.cpp
    MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedChannel/MySignalAnalyzedChannelInfo.cpp
    MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedEvent.cpp
    MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedEventInfo.cpp
    MyEvent/MySignalAnalyzedEvent/MyVoltage/MyVoltage.cpp
    MyEvent/MySortedEvent/MyDetektor/MyDetektor.cpp
    MyEvent/MySortedEvent/MyDetektor/MyDetektorHit.cpp
    MyEvent/MySortedEvent/MyDetektor/MyDetektorInfo.cpp
    MyEvent/MySortedEvent/MyDetektor/MyLayerProperty.cpp
    MyEvent/MySortedEvent/MyDetektor/MyTimeRange.cpp
    MyEvent/MySortedEvent/MyPunkt/MyPunkt.cpp
    MyEvent/MySortedEvent/MySortedEvent.cpp
    MyEvent/MySortedEvent/MySortedEventInfo.cpp
    MyEventViewer/MyEventViewer.cpp
    MyFileProcessor/MyFileProcessor.cpp
    MyPulsHistoFiller/MyPulsHistoFiller.cpp
    MyRootManager/MyHistos.cpp
    MyRootManager/MyRootManager.cpp
    MySettings/MySettings.cpp
    MySignalAnalyzer/baselineCorrection.cpp
    MySignalAnalyzer/helperfunctionsforSTDC.cpp
    MySignalAnalyzer/MySignalAnalyzer.cpp
    MySignalAnalyzer/MySoftTDCCFD.cpp
    MySignalAnalyzer/MySoftTDCCoM.cpp
    # ThreadStuff/MySoftTDCCFDWithNonWorkingThreats.cpp
    # ThreadStuff/MySoftTDCCoMWithNonWorkingThreats.cpp
    # ThreadStuff/MySoftTDCWithNonWorkingThreats.cpp
)
ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS_DICT} ${SRCS_MAIN})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${ROOT_LIBRARIES})
