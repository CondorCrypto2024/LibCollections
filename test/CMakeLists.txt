set(BOOST_INCLUDE_DIR "/usr/include/boost")
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_STATIC_RUNTIME OFF)

find_library( BOOST_TEST_LIBRARIES boost_unit_test_framework "/usr/lib64/boost" )

include_directories( 
        ${BOOST_INCLUDE_DIR}        
        "${PROJECT_SOURCE_DIR}/LibCollections/include"        
        "${PROJECT_SOURCE_DIR}/LibHelpers/include"
)

set(TEST_SOURCES "${PROJECT_SOURCE_DIR}/LibCollections/test/Test.cpp")

set(ROCKSDB_LIBRARY "")
if(EXISTS "/usr/lib64/librocksdb.so")
        set(ROCKSDB_LIBRARY "rocksdb")
endif()

add_executable(test_collections ${TEST_SOURCES})

target_link_libraries( test_collections 
        ${BOOST_TEST_LIBRARIES}
        ${ROCKSDB_LIBRARY}
        pthread 
)
add_test(TestCollections test_collections --random -- COMMAND)

## Instalar test ejecutable junto con RPM
#set_property(TARGET test_collections APPEND_STRING PROPERTY LINK_FLAGS "-Wl,-rpath,/opt/${PROJECT_NAME}/lib")
#install(TARGETS test_collections RUNTIME DESTINATION "/opt/${PROJECT_NAME}/test/LibCollections")