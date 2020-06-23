set(LLVM_LINK_COMPONENTS
  Support
  )

#对应的cpp文件都要加进来
add_clang_executable(ClangObfuscator
  obfuscator.cpp 
)

# 后面会使用到这些库
target_link_libraries(ClangObfuscator
    PRIVATE
    clangAST
    clangBasic
    clangDriver
    clangFormat
    clangLex
    clangParse
    clangSema
    clangFrontend
    clangTooling
    clangToolingCore
    clangRewrite
    clangRewriteFrontend
)

if(UNIX)
  set(CLANGXX_LINK_OR_COPY create_symlink)
else()
  set(CLANGXX_LINK_OR_COPY copy)
endif()
