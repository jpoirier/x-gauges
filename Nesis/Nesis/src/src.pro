# RESOURCES = Nesis.qrc
DEFINES = _KANARDIA_NESIS \
          QT_NO_DEBUG

TARGET = ../bin/nesis

CONFIG += warn_on \
          qt \
          thread \
          exceptions \
          stl \
          rtti \
          console \
		  debug_and_release \
          precompile_header


QT += xml \
 core \
 gui

PRECOMPILED_HEADER = stable.h

TRANSLATIONS = nesis_sl_SI.ts \
                nesis_pt_PT.ts \

TEMPLATE = app

HEADERS += ../../Common/Defines.h \
           ../../Common/Tolerance.h \
           ../../Common/JulianDay.h \
           ../../Common/svn.h \
           ../../Common/MathEx.h \
           ../../Common/CommonDefs.h \
           ../../Common/CommonDefsQt.h \
           ../../Common/Delegate.h \
           ../../Common/KanardiaHardwareUnits.h \
           ../../Common/SphereNavigation.h \
           ../../Common/CRC-32.h \
           ../../Common/CRC-8.h \
           ../../Common/SuperFastHash.h \
           ../../Common/XmlWriter.h \
           ../../Common/LZO/LZOMini.h \
           ../../Common/LZO/LZO.h \
           ../../Common/LZO/lzoconf.h \
           ../../Common/LZO/lzodefs.h \
           ../../Common/LZO/minilzo.h \
           ../../Common/PacketComm/packet.h \
           ../../Common/PacketComm/AbstractPackage.h \
           ../../Common/PacketComm/AbstractProcessor.h \
           ../../Common/PacketComm/AbstractPort.h \
           ../../Common/PacketComm/AbstractEIAPackage.h \
           ../../Common/PacketComm/CanEIAPackageEmiter.h \
           ../../Common/PacketComm/NesisProtocol.h \
           ../../Common/PacketComm/PackageProcessor.h \
           ../../Common/PacketComm/Serial.h \
           ../../Common/PacketComm/Mabu.h \
           ../../Common/Chart/Label.h \
           ../../Common/Geometry/Angle.h \
           ../../Common/Geometry/Point2D.h \
           ../../Common/Geometry/Line2D.h \
           ../../Common/Geometry/Polygon2D.h \
           ../../Common/Geometry/Contour2D.h \
           ../../Common/Geometry/gpc.h \
           ../../Common/Route/Waypoint.h \
           ../../Common/Route/Airfield.h \
           ../../Common/Route/WaypointVFR.h \
           ../../Common/Route/NavAid.h \
           ../../Common/Route/WaypointContainer.h \
           ../../Common/Route/WaypointContainerSingleton.h \
           ../../Common/Route/Route.h \
           ../../Common/Route/RouteContainer.h \
           ../../Common/Route/RouteContainerSingleton.h \
           ../../Common/Matrix/Matrix.h \
           ../../Common/Matrix/MatrixHeap.h \
           ../../Common/Matrix/Vector.h \
           ../../Common/Matrix/VectorHeap.h \
           ../../Common/Matrix/Cholesky.h \
           ../../Common/Matrix/LU.h \
           ../../Common/Matrix/Quaternion.h \
           ../../Common/Numeric/BestFit.h \
           ../../Common/Numeric/FitCompass.h \
           ../../Common/Numeric/FitSolverLevenbergMarquard.h \
           ../../Common/Logbook/Logbook.h \
           ../../Common/Logbook/Entry.h \
           ../../Common/Logbook/Record.h \
           ../../Common/Logbook/PilotList.h \
           ../../Common/CanAerospace/Message.h \
           ../../Common/CanAerospace/MessageAerospace.h \
           ../../Common/CanAerospace/AerospaceData.h \
           ../../Common/CanAerospace/AbstractUnit.h \
           ../../Common/CanAerospace/AbstractAerospaceDataManager.h \
           ../../Common/CanAerospace/UploadService.h \
           ../../Common/CanAerospace/DownloadService.h \
           ../../Common/CanAerospace/FlashProgrammingService.h \
           ../../Common/CanAerospace/ApplicationProgrammingService.h \
           ../../Common/CanAerospace/ModuleInfoService.h \
           ../../Common/CanAerospace/ModuleConfigService.h \
           ../../Common/CanAerospace/NodeIdSettingService.h \
           ../../Common/CanAerospace/UnitProgrammer.h \
           ../../Common/CanAerospace/StaticHelper.h \
           ../../Common/CanUnitInfo/UnitInfoBase.h \
           ../../Common/CanUnitInfo/UnitInfoAiru.h \
           ../../Common/CanUnitInfo/UnitInfoMagu.h \
           ../../Common/CanUnitInfo/UnitInfoMabu.h \
           ../../Common/CanUnitInfo/UnitInfoDaqu.h \
           ../../Common/CanUnitInfo/UnitInfoBaseContainer.h \
           ../../Common/CanUnitInfo/MagSphereCalibration.h \
           ../../Common/ISA/Altitude.h \
           ../../Common/Unit/Unit.h \
           ../../Common/Unit/Group.h \
           ../../Common/Unit/Manager.h \
           ../../Common/Unit/Value.h \
           ../../Common/BestFit/AbstractBestFit.h \
           ../../Common/BestFit/AbstractBestFitLinear.h \
           ../../Common/GeoMag/GeoMagModel.h \
           ApplicationDefines.h \
           stable.h \
           AbstractPanel.h \
           AbstractNesisInput.h \
           AbstractNesisDialog.h \
           Alarms.h \
           AlarmContainer.h \
           Settings.h \
           FastUnitKeys.h \
           WidgetMain.h \
           WidgetMap.h \
           WidgetMessage.h \
           ActiveNavigation.h \
           GraphicsSingleton.h \
           WidgetHSIMap.h \
           WidgetHSIMiniMap.h \
           PanelClassic.h \
           PanelHSIMap.h \
           PanelEngine.h \
           PanelModern.h \
           PanelCheckList.h \
           PanelMiscellaneous.h \
           AirspaceVertical.h \
           DialogQNH.h \
           DialogHeading.h \
           DialogAltitude.h \
           DialogWaypoint.h \
           DialogLineEdit.h \
           DialogRouteLeg.h \
           DialogRouteSelect.h \
           DialogFuelLevel.h \
           DialogYesNo.h \
           DialogShutDown.h \
           DialogFlightPlan.h \
           DialogLogbook.h \
           DialogPilots.h \
           DialogCompassCalibration.h \
           DialogSoftwareUpdate.h \
           DialogSoftwareManagement.h \
           DialogSettings.h \
           DialogBrowser.h \
           DialogSetTime.h\
           DialogMount.h \
           DialogAlarms.h \
           DialogNearestAirfield.h \
           DialogStartup.h \
           DialogEngine.h \
           DialogReboot.h \
           ModelWaypointTable.h \
           ModelRouteList.h \
           ModelRouteWptTable.h \
           NesisEventFilter.h \
           NesisSpinBox.h \
           NesisDoubleSpinBox.h \
           NesisCheckBox.h \
           NesisEditBox.h \
           NesisEditBox2.h \
           NesisTableView.h \
           NesisListView.h \
           NesisBrowser.h \
           NesisComboBox.h \
           NesisAerospaceDataManager.h \
           NesisUnitCAN.h \
           NesisUnitInfoContainer.h \
           NesisSplash.h \
           NesisGroupBox.h \
           PixmapBattery.h \
           PixmapDirection.h \
           PacketManager.h \
           SoftwarePackage.h \
           DialogUserWaypoint.h \
           WidgetQuickMap.h

SOURCES += ../../Common/CommonDefs.cpp \
           ../../Common/JulianDay.cpp \
           ../../Common/LZO/LZOMini.cpp \
           ../../Common/LZO/minilzo.c \
           ../../Common/PacketComm/AbstractPort.cpp \
           ../../Common/PacketComm/Mabu.cpp \
           ../../Common/PacketComm/Serial.cpp \
           ../../Common/PacketComm/AbstractEIAPackage.cpp \
           ../../Common/PacketComm/CanEIAPackageEmiter.cpp \
           ../../Common/PacketComm/PackageProcessor.cpp \
           ../../Common/Chart/Label.cpp \
           ../../Common/Geometry/Angle.cpp \
           ../../Common/Geometry/Point2D.cpp \
           ../../Common/Geometry/Line2D.cpp \
           ../../Common/Geometry/Polygon2D.cpp \
           ../../Common/Geometry/Contour2D.cpp \
           ../../Common/Geometry/gpc.cpp \
           ../../Common/Route/Waypoint.cpp \
           ../../Common/Route/Airfield.cpp \
           ../../Common/Route/WaypointVFR.cpp \
           ../../Common/Route/NavAid.cpp \
           ../../Common/Route/WaypointContainer.cpp \
           ../../Common/Route/WaypointContainerSingleton.cpp \
           ../../Common/Route/Route.cpp \
           ../../Common/Route/RouteContainer.cpp \
           ../../Common/Route/RouteContainerSingleton.cpp \
           ../../Common/Numeric/BestFit.cpp \
           ../../Common/Numeric/FitCompass.cpp \
           ../../Common/Numeric/FitSolverLevenbergMarquard.cpp \
           ../../Common/Logbook/Logbook.cpp \
           ../../Common/Logbook/Entry.cpp \
           ../../Common/Logbook/Record.cpp \
           ../../Common/Logbook/PilotList.cpp \
           ../../Common/XmlWriter.cpp \
           ../../Common/CRC-32.cpp \
           ../../Common/CRC-8.cpp \
           ../../Common/CanAerospace/MessageAerospace.cpp \
           ../../Common/CanAerospace/AerospaceData.cpp \
           ../../Common/CanAerospace/AbstractAerospaceDataManager.cpp \
           ../../Common/CanAerospace/DownloadService.cpp \
           ../../Common/CanAerospace/FlashProgrammingService.cpp \
           ../../Common/CanAerospace/ApplicationProgrammingService.cpp \
           ../../Common/CanAerospace/UnitProgrammer.cpp \
           ../../Common/CanAerospace/UploadService.cpp \
           ../../Common/CanAerospace/ModuleInfoService.cpp \
           ../../Common/CanAerospace/ModuleConfigService.cpp \
           ../../Common/CanAerospace/NodeIdSettingService.cpp \
           ../../Common/CanAerospace/StaticHelper.cpp \
           ../../Common/CanUnitInfo/UnitInfoBase.cpp \
           ../../Common/CanUnitInfo/UnitInfoAiru.cpp \
           ../../Common/CanUnitInfo/UnitInfoMagu.cpp \
           ../../Common/CanUnitInfo/UnitInfoMabu.cpp \
           ../../Common/CanUnitInfo/UnitInfoDaqu.cpp \
           ../../Common/CanUnitInfo/UnitInfoBaseContainer.cpp \
           ../../Common/CanUnitInfo/MagSphereCalibration.cpp \
           ../../Common/Unit/Unit.cpp \
           ../../Common/Unit/Group.cpp \
           ../../Common/Unit/Manager.cpp \
           ../../Common/Unit/Value.cpp \
           ../../Common/BestFit/AbstractBestFit.cpp \
           ../../Common/BestFit/AbstractBestFitLinear.cpp \
           ../../Common/GeoMag/GeoMagModel.cpp \
           ../../Common/CommonDefsQt.cpp \
           Nesis.cpp \
           Settings.cpp \
           FastUnitKeys.cpp \
           WidgetMain.cpp \
           WidgetMap.cpp \
           AirspaceVertical.cpp \
           WidgetMessage.cpp \
           ActiveNavigation.cpp \
           GraphicsSingleton.cpp \
           WidgetHSIMap.cpp \
           WidgetHSIMiniMap.cpp \
           AbstractPanel.cpp \
           AbstractNesisInput.cpp \
           AbstractNesisDialog.cpp \
           Alarms.cpp \
           AlarmContainer.cpp \
           PanelClassic.cpp \
           PanelHSIMap.cpp \
           PanelEngine.cpp \
           PanelModern.cpp \
           PanelCheckList.cpp \
           PanelMiscellaneous.cpp \
           DialogQNH.cpp \
           DialogHeading.cpp \
           DialogAltitude.cpp \
           DialogWaypoint.cpp \
           DialogYesNo.cpp \
           DialogLineEdit.cpp \
           DialogRouteLeg.cpp \
           DialogRouteSelect.cpp \
           DialogFuelLevel.cpp \
           DialogShutDown.cpp \
           DialogFlightPlan.cpp \
           DialogLogbook.cpp \
           DialogPilots.cpp \
           DialogCompassCalibration.cpp \
           DialogSoftwareUpdate.cpp \
           DialogSoftwareManagement.cpp \
           DialogSettings.cpp \
           DialogBrowser.cpp \
           DialogSetTime.cpp\
           DialogMount.cpp \
           DialogNearestAirfield.cpp \
           DialogStartup.cpp \
           DialogEngine.cpp \
           DialogAlarms.cpp \
           DialogReboot.cpp \
           ModelWaypointTable.cpp \
           ModelRouteList.cpp \
           ModelRouteWptTable.cpp \
           NesisEventFilter.cpp \
           NesisSpinBox.cpp \
           NesisDoubleSpinBox.cpp \
           NesisCheckBox.cpp \
           NesisEditBox.cpp \
           NesisEditBox2.cpp \
           NesisTableView.cpp \
           NesisListView.cpp \
           NesisBrowser.cpp \
           NesisComboBox.cpp \
           NesisAerospaceDataManager.cpp \
           NesisUnitCAN.cpp \
           NesisUnitInfoContainer.cpp \
           NesisGroupBox.cpp \
           NesisSplash.cpp \
           PixmapBattery.cpp \
           PacketManager.cpp \
           SoftwarePackage.cpp \
           PixmapDirection.cpp \
           DialogUserWaypoint.cpp \
           WidgetQuickMap.cpp

DESTDIR = .

; INCLUDEPATH += $$(HOME)/src/Kanardia/Public/Common \
; 					$$(HOME)/src/Kanardia/Public/Instruments/src
INCLUDEPATH += ../../Common \
					../../Instruments/src


TARGETDEPS += ../Instruments/libInstruments.a \
  ../Map/libMap.a \
  ../Style/libStyle.a


INSTALLS += target

target.path = /cm-x270/nesis

DEFINES -= _KANARDIA_NESIS

LIBS += ../Map/libMap.a \
  ../Style/libStyle.a \
  ../Instruments/libInstruments.a

