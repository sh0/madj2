#
# Bento4 libraries
# Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for Bento4")

# Paths
set(BENTO4_PATH ${CMAKE_SOURCE_DIR}/libraries/bento4)
set(BENTO4_PATH_BUILD ${BENTO4_PATH}/cmakebuild)
set(BENTO4_PATH_ROOT ${BENTO4_PATH}/Source/C++)
set(BENTO4_PATH_CODECS ${BENTO4_PATH_ROOT}/Codecs)
set(BENTO4_PATH_CORE ${BENTO4_PATH_ROOT}/Core)
set(BENTO4_PATH_CRYPTO ${BENTO4_PATH_ROOT}/Crypto)
set(BENTO4_PATH_METADATA ${BENTO4_PATH_ROOT}/MetaData)
set(BENTO4_PATH_SYSTEM ${BENTO4_PATH_ROOT}/System)

# Set flags
set(BENTO4_FOUND TRUE)
set(BENTO4_CFLAGS -I${BENTO4_PATH_CORE} -I${BENTO4_PATH_CODECS} -I${BENTO4_PATH_CRYPTO} -I${BENTO4_PATH_METADATA})
set(BENTO4_LDFLAGS -L${BENTO4_PATH_BUILD} -lap4)
info_library("Bento4" BENTO4 FATAL_ERROR)
