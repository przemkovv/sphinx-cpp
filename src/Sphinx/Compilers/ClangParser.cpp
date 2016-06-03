
#include "ClangParser.h"

#include <clang-c/Index.h>
#include <array>

template<typename... Ts>
constexpr auto make_array(Ts&&... ts)
    -> std::array<std::common_type_t<Ts...>,sizeof...(ts)>
{
    return { std::forward<Ts>(ts)... };
}


namespace Sphinx::Compilers { 

    ClangParser::ClangParser() : Compiler{}, logger(spdlog::stdout_logger_st(name(), true))
    {
    }

    std::string ClangParser::getVersion()
    {
        return "";
    }

    bool ClangParser::compile(File file) {
        logger.information("Compiling file: " + file.name);

        return true;
    }
    bool ClangParser::compile(Sandbox sandbox) {

        logger.information("Compiling files: ");
        for (auto & file : sandbox.getFiles() ) {
            logger.information(file.path());

            auto args = make_array( 
                    "-xc++", 
                    file.path().c_str(),
                    "-c",
                    "-I/usr/include",
                    "-I/usr/lib/clang/3.6.0/include" 
                    );



            auto index = clang_createIndex(0, 0);
            auto tu = clang_parseTranslationUnit(index,
                    0, begin(args.size(), 0, 0, CXTranslationUnit_None);

            for (uint32_t i = 0, N = clang_getNumDiagnostics(tu);  i != N; ++i) {
                auto diag = clang_getDiagnostic(tu, i);
                auto format_diag = clang_formatDiagnostic(diag, clang_defaultDiagnosticDisplayOptions());
                logger.information(clang_getCString(format_diag));
                clang_disposeString(format_diag);
            }

            clang_disposeTranslationUnit(tu);
            clang_disposeIndex(index);

        }
        

        return true;
    }
    std::string ClangParser::getOutput() const {
        return "";
    }
    std::string ClangParser::getErrors() const { 
        return "";
    }
}
