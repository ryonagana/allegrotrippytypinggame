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



contains(QT_ARCH,i386){
    QMAKE_CFLAGS += -m32
}else {
    QMAKE_CFLAGS += -m64
}

QMAKE_CFLAGS_DEBUG += -Og -g -Wpedantic
QMAKE_CFLAGS_RELEASE += -Wall -Wextra -s -O3 -Werror -fomit-frame-pointer

DEFINES += GAME_DATA_PACK
#DEFINES += DEBUG


win32: {






    QMAKE_CFLAGS += -static

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



   # LIBS += -lallegro_monolith.dll

    INCLUDEPATH += $$PWD/vendor/liballegro-5.2.8-static/include
    LIBS += -L$$PWD/vendor/liballegro-5.2.8-static/lib


    #WINDOWS LIBS
    #LIBS  +=  -lallegro_monolith-static
    #LIBS  +=   -logg -lFLAC -lvorbis -ldumb -lopus -lfreetype -lpng16 -lwebp -ljpeg
    #LIBS  +=  -lgdi32 -luser32 -lkernel32 -lopengl32 -lz -lwinmm -lShlwapi -lOle32 -lstdc++ -lcomdlg32 -ldsound
    #LIBS +=   -lopus -ljpeg -ldsound -ldumb -lFLAC -lvorbisfile -lfreetype -logg -lpng16 -lzlib -lwebp -lstdc++
    #LIBS +=  -lkernel32 -lgdiplus -luuid -lkernel32 -lwinmm -lpsapi -lopengl32 -lglu32 -luser32 -lcomdlg32 -lgdi32 -lshell32 -lole32 -ladvapi32 -lws2_32 -lshlwapi
    #LIBS += -lallegro-static -lallegro_acodec-static -lallegro_audio-static -lallegro_font-static -lallegro_ttf-static  -lallegro_dialog-static -lallegro_primitives-static -lallegro_image-static -lallegro_physfs-static
    LIBS  += -lallegro_monolith.dll

    #libphysfs (x64)
    #LIBS += -L$$PWD/../../libs/physfs-3.0.1/build-winx64/ -lphysfs
    LIBS += -LD:\libs\physfs-3.0.2\windows_build/
    LIBS += -LD:\projetos\libs\physfs-3.0.1\build-winx64
    LIBS +=  -LD:\programacao\C\openskigame\libs\physfs\mingw_x86_64\lib



    INCLUDEPATH += vendor/physfs-3.0.1-mingw_x64/include
    DEPENDPATH +=  vendor/physfs-3.0.1-mingw_x64/lib
    LIBS += -lphysfs

    LIBS += -lmingw32


}

unix : {
    debug:{
        DEFINES += DEBUG
    }


    INCLUDEPATH += /usr/include
    LIBS += -L/usr/lib64

    LIBS += -lallegro -lallegro_audio -lallegro_acodec -lallegro_font -lallegro_ttf -lallegro_dialog -lallegro_image -lallegro_physfs
    LIBS += -lphysfs
}

DISTFILES += \
    assets/shader/lavalamp_glsl.frag \
    assets/shader/lavalamp_glsl_allegro.frag \
    assets/shader/swirl_glsl_allegro.frag \
    readme.md

HEADERS += \
    main.h \
    particles.h \
    words.h
