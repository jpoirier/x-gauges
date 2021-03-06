# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./src
# Target is an application:  ../bin/nesim
#DEFINES += NDEBUG
HEADERS += ../../../Common/Map/AbstractProjection.h \
           ../../../Common/Map/LambertConformalConicProjection.h \
           ../../../Common/Map/MercatorProjection.h \
           ../../../Common/Map/ProjectionFactory.h \
           ../../../Common/Map/Mapper.h \
           ../../../Common/Map/MapDefs.h \
           ../../../Common/Map/World.h \
           ../../../Common/Map/Block.h \
           ../../../Common/Map/BlockHeader.h \
           ../../../Common/Map/BlockRaster.h \
           ../../../Common/Map/BlockVector.h \
           ../../../Common/Map/AbstractVectorLayer.h \
           ../../../Common/Map/VectorLayerPolyline.h \
           ../../../Common/Map/VectorLayerArea.h \
           ../../../Common/Map/VectorLayerAirfield.h \
           ../../../Common/Map/VectorLayerPoint.h \
           ../../../Common/Map/VectorLayerPopulatedPoint.h \
           ../../../Common/Map/AbstractLayerEntity.h \
           ../../../Common/Map/LayerEntityPolyline.h \
           ../../../Common/Map/LayerEntityArea.h \
           ../../../Common/Map/LayerEntityAirfield.h \
           ../../../Common/Map/LayerEntityPoint.h \
           ../../../Common/Map/LayerEntityPopulatedPoint.h \
           ../../../Common/Map/MapImageZoom.h \
           ../../../Common/Map/MapImage.h \
           ../../../Common/Map/ContourColor.h \
           ../../../Common/Map/TextLabel.h \
           ../../../Common/Map/Airspace.h \
           ../../../Common/Map/AirspaceContainer.h \
           ../../../Common/Map/ActiveAirspaceContainer.h \
           ../../../Common/Map/FlatEarth.h \
           ../../../Common/Chart/ChartWidget.h \
           ../../../Common/Chart/Proportion.h \
           ../../../Common/Chart/Label.h \
           ../../../Common/Chart/Axis.h \
           ../../../Common/Chart/AxisX.h \
           ../../../Common/Chart/AxisY.h \
           ../../../Common/Chart/PlotBase.h \
           ../../../Common/Chart/PlotLine.h \
           ../../../Common/Chart/PlotXY.h \
           ../../../Common/Chart/Mark.h \
           ../../../Common/Chart/Chart.h \
           ../../../Common/Chart/PlotLineBar.h \
           ../../../Common/Geometry/Angle.h \
           ../../../Common/Geometry/Point2D.h \
           ../../../Common/Geometry/Line2D.h \
           ../../../Common/Geometry/Polygon2D.h \
           ../../../Common/CommonDefs.h \
           ../../../Common/SphereNavigation.h \
           ../../../Common/Unit.h \
           NeSim.h \
           Nesis.h \
           ../../src/WidgetMap.h \
           ../../src/Settings.h \
           ../../src/SenapData.h 
SOURCES += ../../../Common/Map/AbstractProjection.cpp \
           ../../../Common/Map/LambertConformalConicProjection.cpp \
           ../../../Common/Map/MercatorProjection.cpp \
           ../../../Common/Map/ProjectionFactory.cpp \
           ../../../Common/Map/Mapper.cpp \
           ../../../Common/Map/World.cpp \
           ../../../Common/Map/Block.cpp \
           ../../../Common/Map/BlockHeader.cpp \
           ../../../Common/Map/BlockRaster.cpp \
           ../../../Common/Map/BlockVector.cpp \
           ../../../Common/Map/AbstractVectorLayer.cpp \
           ../../../Common/Map/VectorLayerPolyline.cpp \
           ../../../Common/Map/VectorLayerArea.cpp \
           ../../../Common/Map/VectorLayerAirfield.cpp \
           ../../../Common/Map/VectorLayerPoint.cpp \
           ../../../Common/Map/VectorLayerPopulatedPoint.cpp \
           ../../../Common/Map/AbstractLayerEntity.cpp \
           ../../../Common/Map/LayerEntityPolyline.cpp \
           ../../../Common/Map/LayerEntityArea.cpp \
           ../../../Common/Map/LayerEntityAirfield.cpp \
           ../../../Common/Map/LayerEntityPoint.cpp \
           ../../../Common/Map/LayerEntityPopulatedPoint.cpp \
           ../../../Common/Map/MapImageZoom.cpp \
           ../../../Common/Map/MapImage.cpp \
           ../../../Common/Map/ContourColor.cpp \
           ../../../Common/Map/TextLabel.cpp \
           ../../../Common/Map/Airspace.cpp \
           ../../../Common/Map/AirspaceContainer.cpp \
           ../../../Common/Map/ActiveAirspaceContainer.cpp \
           ../../../Common/Map/FlatEarth.cpp \
           ../../../Common/Chart/ChartWidget.cpp \
           ../../../Common/Chart/Proportion.cpp \
           ../../../Common/Chart/Label.cpp \
           ../../../Common/Chart/Axis.cpp \
           ../../../Common/Chart/AxisX.cpp \
           ../../../Common/Chart/AxisY.cpp \
           ../../../Common/Chart/PlotBase.cpp \
           ../../../Common/Chart/PlotLine.cpp \
           ../../../Common/Chart/PlotXY.cpp \
           ../../../Common/Chart/Mark.cpp \
           ../../../Common/Chart/Chart.cpp \
           ../../../Common/Chart/PlotLineBar.cpp \
           ../../../Common/Geometry/Angle.cpp \
           ../../../Common/Geometry/Point2D.cpp \
           ../../../Common/Geometry/Line2D.cpp \
           ../../../Common/Geometry/Polygon2D.cpp \
           ../../../Common/CommonDefsQt.cpp \
           ../../../Common/CommonDefs.cpp \
           NeSim.cpp \
           Nesis.cpp \
           ../../src/WidgetMap.cpp \
           ../../src/Settings.cpp \
           ../../src/SenapData.cpp 
INCLUDEPATH += $$(HOME)/src/Kanardia/Public/Common \
               $$(HOME)/src/Kanardia/Public/Nesis/src
CONFIG += debug \
warn_on \
qt \
thread \
exceptions \
stl \
rtti \
console
TARGET=../bin/nesim
