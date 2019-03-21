cmake_minimum_required(VERSION 3.12)
project(ccmark VERSION 1.0.0 DESCRIPTION "Markdown Parser")

set(CMAKE_CXX_STANDARD 17)


include_directories(/usr/local/Cellar/boost/1_67_0/)

set(sources
        src/ccmark.cpp
        src/CoreState.cpp
        src/BlockParser.cpp
        src/Renderer.cpp
        src/InlineParser.cpp
        src/options.cpp src/token.cpp
        src/helpers/common.cpp src/helpers/entities.c src/helpers/html_re.cpp src/helpers/encode.cpp
        src/processor.cpp src/LinkIds.cpp src/FootnoteIds.cpp
        
        src/block_rules/BlockState.cpp src/block_rules/block_rules.cpp src/block_rules/heading.cpp src/block_rules/paragraph.cpp src/block_rules/code.cpp src/block_rules/fence.cpp src/block_rules/hr.cpp src/block_rules/blockquote.cpp src/block_rules/list.cpp src/block_rules/reference.cpp src/block_rules/lheading.cpp src/block_rules/html_block.cpp src/block_rules/math_block.cpp src/block_rules/footnote_def.cpp
        src/inline_rules/InlineState.cpp src/inline_rules/inline_rules.cpp src/inline_rules/text.cpp src/inline_rules/newline.cpp src/inline_rules/escape.cpp src/inline_rules/autolink.cpp src/inline_rules/backtick.cpp src/inline_rules/strikethrough.cpp src/inline_rules/link.cpp src/inline_rules/image.cpp src/inline_rules/entity.cpp src/inline_rules/emphasis.cpp src/inline_rules/html_inline.cpp src/inline_rules/math_inline.cpp src/inline_rules/footnote_ref.cpp src/inline_rules/footnote_inline.cpp)

add_library(ccmark SHARED
        ${sources})
target_include_directories(ccmark PUBLIC src)
set_target_properties(ccmark PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(ccmark PROPERTIES PUBLIC_HEADER src/ccmark.h)

target_link_libraries(ccmark PUBLIC -lboost_regex)

add_executable(md src/main.cpp)
target_link_libraries(md PUBLIC ccmark)

# single test
#add_executable(test
#        tests/test.cpp
#        src/markdown-tokens.cpp
#        src/markdown.cpp
#        src/markdown.h
#        src/markdown-tokens.h)
#target_link_libraries(test PUBLIC -lboost_regex -lstdc++fs)


# google tests
#add_definitions(-DNDEBUG)
set (gtest_SOURCE_DIR "/usr/local/Cellar/googletest")
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
add_executable(gtests
        tests/gtests.cpp
        ${sources}
        )
target_link_libraries(gtests PUBLIC -lstdc++fs -lboost_regex -L${gtest_SOURCE_DIR}/build gtest)

# single test1
add_executable(test1
        tests/test1.cpp
        )
target_link_libraries(test1 PUBLIC -lboost_regex ccmark)

add_executable(test_entities
        tests/test_entities.cpp
        )
#target_link_libraries(test_entities PUBLIC -lboost_regex)

# google bench
add_executable(gbench
        tests/gbench.cpp
        )
target_link_libraries(gbench PUBLIC ccmark -lboost_regex -lbenchmark)