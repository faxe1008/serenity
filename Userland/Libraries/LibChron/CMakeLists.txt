set(SOURCES
    ChronClient.cpp
)

set(GENERATED_SOURCES
    ../../Services/ChronServer/ChronServerEndpoint.h
    ../../Services/ChronServer/ChronClientEndpoint.h
)

serenity_lib(LibChron chron)
target_link_libraries(LibChron LibIPC)
