cmake_minimum_required(VERSION 3.27)

project(MyServer LANGUAGES CXX)

include_directories(/usr/include/mysql/)  #add include directories

add_executable(MyServer main.cpp
    myserver.h myserver.cpp
    dbbroker.h dbbroker.cpp
    config.h config.cpp
    threadpool.h threadpool.cpp
    userstatusevaluator.h userstatusevaluator.cpp
    epolloperator.h epolloperator.cpp
    snowflakeidworker.h snowflakeidworker.cpp
    initcontrol.h initcontrol.cpp
    netpacketgenerator.h netpacketgenerator.cpp
    sendfilecontrol.h sendfilecontrol.cpp
    receivefilecontrol.h receivefilecontrol.cpp
    sendinfocontrol.h sendinfocontrol.cpp
    modifyinfocontrol.h modifyinfocontrol.cpp
    livelistmonitor.h livelistmonitor.cpp
    boostserver.h boostserver.cpp
    singleton.h
    noncopyable.h
    session.h session.cpp
)
add_subdirectory(json)

target_link_libraries( MyServer
    PUBLIC
        libmysqlpp.so.3.3.0
        nlohmann_json
)

target_compile_features( MyServer PRIVATE cxx_std_20 )

include(GNUInstallDirs)
install(TARGETS MyServer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# target_sources( MyServer 
#     PRIVATE
#         main.cpp
#         
#     PUBLIC
#         
# )
