project(plugin)

add_library(plugin
    src/plugin.h
    src/plugin.cpp
    src/detail/token.h
    src/detail/lexer.h
    src/detail/lexer.cpp
    src/detail/parser.h
    src/detail/parser.cpp
    src/detail/node.h
)

target_include_directories(grass PRIVATE src)
