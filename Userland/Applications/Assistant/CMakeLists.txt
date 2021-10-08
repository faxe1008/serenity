serenity_component(
    Assistant
    RECOMMENDED
    TARGETS Assistant
)

set(SOURCES
    Providers.cpp
    FuzzyMatch.cpp
    main.cpp
    Lexer.cpp
    Token.cpp
    Parser.cpp
    )

serenity_app(Assistant ICON app-run)
target_link_libraries(Assistant LibCore LibDesktop LibGUI LibJS LibThreading)
