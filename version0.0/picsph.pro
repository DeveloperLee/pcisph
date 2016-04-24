QT += core gui opengl
CONFIG += c++11
#CONFIG -= app_bundle

TARGET = pcisph
TEMPLATE = app

win32 {
    DEFINES += GLEW_STATIC
}

INCLUDEPATH += \
    src \
    packages \
    packages/glew/include \
    packages/eigen \
    packages/GL \
    packages/glfw/include \
    packages/tbb/include

DEPENDPATH += \
    src \
    packages \
    packages/glew/include \
    packages/eigen \
    packages/GL \
    packages/glfw/include \
    packages/json11

SOURCES += \
    src/app/main.cpp \
    src/app/mainwindow.cpp \
    src/app/glwidget.cpp \
    src/algorithm/SPH.cpp \
    src/utils/Settings.cpp \
    src/visualization/geometry/MarchingCubes.cpp \
    src/visualization/geometry/SDF.cpp \
    src/visualization/geometry/Voxelizer.cpp \
    src/visualization/mesh/Mesh.cpp \
    src/visualization/objLoader/ObjLoader.cpp \
    src/visualization/particle/ParticleGenerator.cpp \
    src/visualization/particle/ParticleMesher.cpp \
    src/visualization/scene/Scene.cpp \
    packages/json11/json11.cpp \
    packages/glew/src/glew.c \
    src/utils/STD.cpp

HEADERS += \
    src/app/mainwindow.h \
    src/app/glwidget.h \
    src/algorithm/SPH.h \
    src/algorithm/Kernel.h \
    src/utils/Box.h \
    src/utils/Vector.h \
    src/utils/STD.h \
    src/utils/Settings.h \
    src/utils/StringUtils.h \
    src/utils/Math.h \
    src/utils/ConcurrentUtils.h \
    src/visualization/geometry/MarchingCubes.h \
    src/visualization/geometry/SDF.h \
    src/visualization/geometry/Voxelizer.h \
    src/visualization/geometry/VoxelGrid.h \
    src/visualization/grid/grid.h \
    src/visualization/mesh/Mesh.h \
    src/visualization/objLoader/ObjLoader.h \
    src/visualization/particle/ParticleGenerator.h \
    src/visualization/particle/ParticleMesher.h \
    src/visualization/scene/Scene.h \
    src/utils/CameraUtils.h \
    src/shading/DomainShader.h \
    src/shading/GridShader.h \
    src/shading/MeshShader.h \
    src/shading/ParticleShader.h \
    src/shading/ParticleNormalShader.h \
    src/shading/ParticleSphereShader.h \
    packages/json11/json11.h \
    packages/glew/include/GL/glew.h

FORMS += src/app/mainwindow.ui

OTHER_FILES +=

DISTFILES += \
    scenes/dambreak.json

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/packages/tbb/release/ -ltbb_static
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/packages/tbb/debug/ -ltbb_static
else:unix: LIBS += -L$$PWD/packages/tbb/ -ltbb_static

INCLUDEPATH += $$PWD/packages/tbb
DEPENDPATH += $$PWD/packages/tbb

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/packages/tbb/release/libtbb_static.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/packages/tbb/debug/libtbb_static.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/packages/tbb/release/tbb_static.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/packages/tbb/debug/tbb_static.lib
else:unix: PRE_TARGETDEPS += $$PWD/packages/tbb/libtbb_static.a

RESOURCES +=
