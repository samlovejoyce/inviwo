set_property(GLOBAL PROPERTY USE_FOLDERS On)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER utils)

#--------------------------------------------------------------------
# Add own cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_BINARY_DIR}/cmake/")

include(globalmacros)

#--------------------------------------------------------------------
# Output paths for the executables, runtimes, archives and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin CACHE PATH
   "Single Directory for all Executables.")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE PATH
   "Single Directory for all static libraries.")
   
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE PATH
   "Single Directory for all Libraries")
   
mark_as_advanced(CMAKE_INSTALL_PREFIX CMAKE_RUNTIME_OUTPUT_DIRECTORY CMAKE_BUNDLE_OUTPUT_DIRECTORY CMAKE_ARCHIVE_OUTPUT_DIRECTORY CMAKE_LIBRARY_OUTPUT_DIRECTORY)

#--------------------------------------------------------------------
# Path for this solution
if(NOT EXECUTABLE_OUTPUT_PATH)
  set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH 
    "Single output directory for building all executables.")
endif(NOT EXECUTABLE_OUTPUT_PATH)

if(NOT LIBRARY_OUTPUT_PATH)
  set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib CACHE PATH
    "Single output directory for building all libraries.")
endif(NOT LIBRARY_OUTPUT_PATH)

mark_as_advanced(EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH)

# Set Common Variables
set(IVW_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(IVW_MODULE_DIR ${CMAKE_SOURCE_DIR}/modules)
set(IVW_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(IVW_EXTENSIONS_DIR ${CMAKE_SOURCE_DIR}/ext)
set(IVW_BINARY_DIR ${CMAKE_BINARY_DIR})
set(IVW_LIBRARY_DIR ${LIBRARY_OUTPUT_PATH})
set(IVW_EXECUTABLE_DIR ${EXECUTABLE_OUTPUT_PATH})
set(IVW_CMAKE_SOURCE_MODULE_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(IVW_CMAKE_BINARY_MODULE_DIR ${CMAKE_BINARY_DIR}/cmake)

configure_file(${IVW_CMAKE_SOURCE_MODULE_DIR}/inviwocommondefines_template.h ${IVW_INCLUDE_DIR}/inviwo/core/inviwocommondefines.h @ONLY)

# Set ignored libs
set(VS_MULTITHREADED_DEBUG_DLL_IGNORE_LIBRARY_FLAGS
    "/NODEFAULTLIB:libc.lib
     /NODEFAULTLIB:libcmt.lib
     /NODEFAULTLIB:msvcrt.lib
     /NODEFAULTLIB:libcd.lib
     /NODEFAULTLIB:libcmtd.lib"
)
set(VS_MULTITHREADED_RELEASE_DLL_IGNORE_LIBRARY_FLAGS
    "/NODEFAULTLIB:libc.lib
     /NODEFAULTLIB:libcmt.lib
     /NODEFAULTLIB:libcd.lib
     /NODEFAULTLIB:libcmtd.lib
     /NODEFAULTLIB:msvcrtd.lib"
)
	
#--------------------------------------------------------------------
# Disable deprecation warnings for standard C functions
if(CMAKE_COMPILER_2005)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
endif(CMAKE_COMPILER_2005)

#--------------------------------------------------------------------
# Set preprocessor definition to indicate whether 
# to use the debug postfix

# Add debug postfix if WIN32
IF(WIN32)
    IF(MSVC)
        SET( CMAKE_DEBUG_POSTFIX "d" )
    ENDIF(MSVC)
ENDIF()

if(DEBUG_POSTFIX)
	add_definitions(-D_DEBUG_POSTFIX)
endif(DEBUG_POSTFIX)

# Build shared libs or static libs
option(BUILD_SHARED_LIBS "Build shared libs, else static libs" OFF)

#--------------------------------------------------------------------
# Specify build-based defintions
if(BUILD_SHARED_LIBS)
  add_definitions(-DINVIWO_ALL_DYN_LINK)
else(BUILD_SHARED_LIBS)
  add_definitions(-DFREEGLUT_STATIC)
  add_definitions(-DGLEW_STATIC)
endif(BUILD_SHARED_LIBS)

#--------------------------------------------------------------------
# TODO: REMOVE!!!
find_package(Qt REQUIRED)

set(QT_DONT_USE_QTCORE FALSE)
set(QT_DONT_USE_QTGUI FALSE)
set(QT_USE_QT3SUPPORT FALSE)
set(QT_USE_QTASSISTANT FALSE)
set(QT_USE_QTDESIGNER FALSE)
set(QT_USE_QTMOTIF FALSE)
set(QT_USE_QTMAIN FALSE)
set(QT_USE_QTNETWORK FALSE)
set(QT_USE_QTNSPLUGIN FALSE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTSQL FALSE)
set(QT_USE_QTXML FALSE)
set(QT_USE_QTSVG FALSE)
set(QT_USE_QTTEST FALSE)
set(QT_USE_QTUITOOLS FALSE)
set(QT_USE_QTDBUS FALSE)
set(QT_USE_QTSCRIPT FALSE)
set(QT_USE_QTASSISTANTCLIENT FALSE)
set(QT_USE_QTHELP FALSE)
set(QT_USE_QTWEBKIT FALSE)
set(QT_USE_QTXMLPATTERNS FALSE)
set(QT_USE_PHONON FALSE)

