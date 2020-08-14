cmake_minimum_required(VERSION 3.9)
project(ed25519 VERSION 1.0.0 DESCRIPTION "ED25519 utilities library")

if (NOT TARGET hashing)
# Hashing functions library
add_subdirectory(extras/hashing)
endif()

set(ed25519_src
   src/sign.c
   src/add_scalar.c
   src/fe.c
   src/sc.c
   src/key_exchange.c
   src/keypair.c
   src/verify.c
   src/ge.c
)

add_library(ed25519 ${ed25519_src})

target_include_directories(ed25519 PRIVATE include)
target_include_directories(ed25519 PRIVATE extras/hashing/include)

set_target_properties(ed25519 PROPERTIES
   VERSION ${PROJECT_VERSION}
   POSITION_INDEPENDENT_CODE 1
   PUBLIC_HEADER "include/ed25519.h"
)

include(GNUInstallDirs)
install(TARGETS ed25519
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

configure_file(ed25519.pc.in ed25519.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/ed25519.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
