cmake_minimum_required(VERSION 3.27)

project(MyServer LANGUAGES CXX)

include_directories(/usr/include/mysql/)  #add include directories

add_executable(MyServer main.cpp
    myserver.h myserver.cpp

    mydatabase.h mydatabase.cpp
    config.h
    threadpool.h threadpool.cpp
    config.cpp
)

target_link_libraries( MyServer
    PUBLIC
        libmysqlpp.so.3.3.0
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
