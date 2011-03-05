CONFIG += warn_on \
          qt \
          thread \
          exceptions \
          stl \
          rtti \
          staticlib \
 debug_and_release
QT += xml \
 core

TEMPLATE = lib

HEADERS += ../../Common/Map/AbstractProjection.h \
           ../../Common/Map/LambertConformalConicProjection.h \
           ../../Common/Map/MercatorProjection.h \
           ../../Common/Map/ProjectionFactory.h \
           ../../Common/Map/Mapper.h \
           ../../Common/Map/MapDefs.h \
           ../../Common/Map/World.h \
           ../../Common/Map/Block.h \
           ../../Common/Map/BlockHeader.h \
           ../../Common/Map/BlockRaster.h \
           ../../Common/Map/BlockVector.h \
           ../../Common/Map/AbstractVectorLayer.h \
           ../../Common/Map/VectorLayerPolyline.h \
           ../../Common/Map/VectorLayerArea.h \
           ../../Common/Map/VectorLayerPoint.h \
           ../../Common/Map/VectorLayerPopulatedPoint.h \
           ../../Common/Map/VectorLayerAirfield.h \
           ../../Common/Map/AbstractLayerEntity.h \
           ../../Common/Map/LayerEntityPolyline.h \
           ../../Common/Map/LayerEntityArea.h \
           ../../Common/Map/LayerEntityPoint.h \
           ../../Common/Map/LayerEntityPopulatedPoint.h \
           ../../Common/Map/LayerEntityAirfield.h \
           ../../Common/Map/MapImageZoom.h \
           ../../Common/Map/MapImage.h \
           ../../Common/Map/ContourColor.h \
           ../../Common/Map/TextLabel.h \
           ../../Common/Map/Airspace.h \
           ../../Common/Map/AirspaceContainer.h \
           ../../Common/Map/ActiveAirspaceContainer.h \
           ../../Common/Map/FlatEarth.h

SOURCES += ../../Common/Map/AbstractProjection.cpp \
           ../../Common/Map/LambertConformalConicProjection.cpp \
           ../../Common/Map/MercatorProjection.cpp \
           ../../Common/Map/ProjectionFactory.cpp \
           ../../Common/Map/World.cpp \
           ../../Common/Map/Mapper.cpp \
           ../../Common/Map/Block.cpp \
           ../../Common/Map/BlockRaster.cpp \
           ../../Common/Map/BlockHeader.cpp \
           ../../Common/Map/BlockVector.cpp \
           ../../Common/Map/AbstractVectorLayer.cpp \
           ../../Common/Map/VectorLayerPolyline.cpp \
           ../../Common/Map/VectorLayerArea.cpp \
           ../../Common/Map/VectorLayerPoint.cpp \
           ../../Common/Map/VectorLayerPopulatedPoint.cpp \
           ../../Common/Map/VectorLayerAirfield.cpp \
           ../../Common/Map/AbstractLayerEntity.cpp \
           ../../Common/Map/LayerEntityPolyline.cpp \
           ../../Common/Map/LayerEntityArea.cpp \
           ../../Common/Map/LayerEntityPoint.cpp \
           ../../Common/Map/LayerEntityPopulatedPoint.cpp \
           ../../Common/Map/LayerEntityAirfield.cpp \
           ../../Common/Map/MapImageZoom.cpp \
           ../../Common/Map/MapImage.cpp \
           ../../Common/Map/ContourColor.cpp \
           ../../Common/Map/TextLabel.cpp \
           ../../Common/Map/Airspace.cpp \
           ../../Common/Map/AirspaceContainer.cpp \
           ../../Common/Map/ActiveAirspaceContainer.cpp \
           ../../Common/Map/FlatEarth.cpp


; INCLUDEPATH += $$(HOME)/src/Kanardia/Public/Common
INCLUDEPATH += ../../Common


DEFINES += QT_NO_DEBUG



