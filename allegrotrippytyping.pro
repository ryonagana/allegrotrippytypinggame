TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += use_c_linker

SOURCES += \
        main.c \
        particles.c \
        words.c


QMAKE_CC=gcc
QMAKE_CFLAGS +=-std=c11 -Wall -Wextra -Werror -Wno-unused-function -Wno-switch -Wuninitialized -Wstrict-prototypes

DEFINES += DEBUG

contains(QT_ARCH,i386){
    QMAKE_CFLAGS += -m32
}else {
    QMAKE_CFLAGS += -m64
}

QMAKE_CFLAGS_DEBUG += -Og -g -Wpedantic
QMAKE_CFLAGS_RELEASE += -Wall -Wextra -s -O2 -Werror -fomit-frame-pointer


win32: {


    INCLUDEPATH += D:\libs\allegro_x86_64\include
    INCLUDEPATH += D:\projetos\libs\liballegro-5.2.8-release\include
    INCLUDEPATH += D:\projetos\libs\allegro-x86_64\allegro\include
    INCLUDEPATH += D:\libs\liballegro-5.2.8-x64\include
    INCLUDEPATH += include



    #INCLUDEPATH += libs/jansson/mingw32_64/include
    #LIBS += -Llibs\jansson\mingw32_64\lib
    #LIBS +=
    #LIBS += -Llibs/jansson/mingw32_64/lib
    #LIBS += -L/home/archdark/projetos/c/openskigame/libs/jansson/linux/x64/lib
   # LIBS += -LD:\programacao\C\openskigame\libs\jansson\mingw32_64\lib
    #LIBS +=  -ljansson


    #LIBS += -LD:\libs\allegro_x86_64\lib
    #LIBS += -LD:\projetos\libs\allegro-x86_64\allegro\lib
    LIBS += -LD:\projetos\libs\liballegro-5.2.8-release\lib
    LIBS += -LD:\libs\liballegro-5.2.8-x64\lib
    TARGET += game


    LIBS += -lallegro_monolith.dll

    #libphysfs (x64)
    #LIBS += -L$$PWD/../../libs/physfs-3.0.1/build-winx64/ -lphysfs
    LIBS += -LD:\libs\physfs-3.0.2\windows_build/
    LIBS += -LD:\projetos\libs\physfs-3.0.1\build-winx64
    LIBS +=  -LD:\programacao\C\openskigame\libs\physfs\mingw_x86_64\lib

    LIBS += -lphysfs

    INCLUDEPATH += $$PWD/../../libs/physfs-3.0.1/build-winx64
    DEPENDPATH += $$PWD/../../libs/physfs-3.0.1/build-winx64


    LIBS += -lmingw32


}

DISTFILES += \
    assets/shader/lavalamp_glsl.frag \
    assets/shader/lavalamp_glsl_allegro.frag \
    assets/shader/swirl_glsl_allegro.frag

HEADERS += \
    main.h \
    particles.h \
    words.h
