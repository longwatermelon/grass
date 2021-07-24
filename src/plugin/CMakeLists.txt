project(plugin)

add_library(plugin
    plugin.h
    plugin.cpp
    detail/token.h
    detail/lexer.h
    detail/lexer.cpp
    detail/parser.h
    detail/parser.cpp
    detail/node.h
)

target_include_directories(grass PRIVATE src)
