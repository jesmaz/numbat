#ifndef MAIN_HPP
#define MAIN_HPP

#include <llvm/MC/MCAsmBackend.h>
#include <llvm/MC/SubtargetFeature.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetLibraryInfo.h>

#include "ast.hpp"
#include "core.hpp"
#include "numbat.hpp"
#include "module.hpp"
#include "generator.hpp"
#include "lexer.hpp"

#include <fstream>
#include <string>


std::string loadFromFile (const std::string & file);

void callback ();


#endif /* MAIN_HPP */
