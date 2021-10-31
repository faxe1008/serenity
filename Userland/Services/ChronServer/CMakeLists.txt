serenity_component(
    ChronServer
    REQUIRED
    TARGETS ChronServer
)

compile_ipc(ChronServer.ipc ChronServerEndpoint.h)
compile_ipc(ChronClient.ipc ChronClientEndpoint.h)

set(SOURCES
    ClientConnection.cpp
    main.cpp
    ChronServerEndpoint.h
    ChronClientEndpoint.h
)

serenity_bin(ChronServer)
target_link_libraries(ChronServer LibIPC)
