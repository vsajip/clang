//===--- CLCompatOptions.td - Options for clang-cl ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the options accepted by clang-cl.
//
//===----------------------------------------------------------------------===//

def cl_Group : OptionGroup<"<clang-cl options>">,
  HelpText<"CL.EXE COMPATIBILITY OPTIONS">;

def cl_compile_Group : OptionGroup<"<clang-cl compile-only options>">,
  Group<cl_Group>;

def cl_ignored_Group : OptionGroup<"<clang-cl ignored options>">,
  Group<cl_Group>;

class CLFlag<string name> : Option<["/", "-"], name, KIND_FLAG>,
  Group<cl_Group>, Flags<[CLOption, DriverOption]>;

class CLCompileFlag<string name> : Option<["/", "-"], name, KIND_FLAG>,
  Group<cl_compile_Group>, Flags<[CLOption, DriverOption]>;

class CLIgnoredFlag<string name> : Option<["/", "-"], name, KIND_FLAG>,
  Group<cl_ignored_Group>, Flags<[CLOption, DriverOption]>;

class CLJoined<string name> : Option<["/", "-"], name, KIND_JOINED>,
  Group<cl_Group>, Flags<[CLOption, DriverOption]>;

class CLCompileJoined<string name> : Option<["/", "-"], name, KIND_JOINED>,
  Group<cl_compile_Group>, Flags<[CLOption, DriverOption]>;

class CLIgnoredJoined<string name> : Option<["/", "-"], name, KIND_JOINED>,
  Group<cl_ignored_Group>, Flags<[CLOption, DriverOption]>;

class CLJoinedOrSeparate<string name> : Option<["/", "-"], name,
  KIND_JOINED_OR_SEPARATE>, Group<cl_Group>, Flags<[CLOption, DriverOption]>;

class CLCompileJoinedOrSeparate<string name> : Option<["/", "-"], name,
  KIND_JOINED_OR_SEPARATE>, Group<cl_compile_Group>,
  Flags<[CLOption, DriverOption]>;

class CLRemainingArgs<string name> : Option<["/", "-"], name,
  KIND_REMAINING_ARGS>, Group<cl_Group>, Flags<[CLOption, DriverOption]>;

// Aliases:
// (We don't put any of these in cl_compile_Group as the options they alias are
// already in the right group.)

def _SLASH_c : CLFlag<"c">, HelpText<"Compile only">, Alias<c>;
def _SLASH_D : CLJoinedOrSeparate<"D">, HelpText<"Define macro">,
  MetaVarName<"<macro[=value]>">, Alias<D>;
def _SLASH_GR : CLFlag<"GR">, HelpText<"Enable RTTI">, Alias<frtti>;
def _SLASH_GR_ : CLFlag<"GR-">, HelpText<"Disable RTTI">, Alias<fno_rtti>;
def _SLASH_GF_ : CLFlag<"GF-">, HelpText<"Disable string pooling">,
  Alias<fwritable_strings>;
def _SLASH_help : CLFlag<"help">, Alias<help>,
  HelpText<"Display available options">;
def _SLASH_I : CLJoinedOrSeparate<"I">,
  HelpText<"Add directory to include search path">, MetaVarName<"<dir>">,
  Alias<I>;
def _SLASH_J : CLFlag<"J">, HelpText<"Make char type unsigned">,
  Alias<funsigned_char>;
def _SLASH_O : CLJoined<"O">, HelpText<"Optimization level">,
  MetaVarName<"<n>">, Alias<O>;
def _SLASH_Ob0 : CLFlag<"Ob0">, HelpText<"Disable inlining">,
  Alias<fno_inline>;
def _SLASH_Od : CLFlag<"Od">, HelpText<"Disable optimization">, Alias<O0>;
def _SLASH_Oi : CLFlag<"Oi">, HelpText<"Enable use of builtin functions">,
  Alias<fbuiltin>;
def _SLASH_Oi_ : CLFlag<"Oi-">, HelpText<"Disable use of builtin functions">,
  Alias<fno_builtin>;
def _SLASH_Os : CLFlag<"Os">, HelpText<"Optimize for size">, Alias<O>,
  AliasArgs<["s"]>;
def _SLASH_Ot : CLFlag<"Ot">, HelpText<"Optimize for speed">, Alias<O>,
  AliasArgs<["2"]>;
def _SLASH_Ox : CLFlag<"Ox">, HelpText<"Maximum optimization">, Alias<O>,
  AliasArgs<["3"]>;
def _SLASH_Oy : CLFlag<"Oy">, HelpText<"Enable frame pointer omission">,
  Alias<fomit_frame_pointer>;
def _SLASH_Oy_ : CLFlag<"Oy-">, HelpText<"Disable frame pointer omission">,
  Alias<fno_omit_frame_pointer>;
def _SLASH_P : CLFlag<"P">, HelpText<"Only run the preprocessor">, Alias<E>;
def _SLASH_QUESTION : CLFlag<"?">, Alias<help>,
  HelpText<"Display available options">;
def _SLASH_showIncludes : CLFlag<"showIncludes">,
  HelpText<"Print info about included files to stderr">,
  Alias<show_includes>;
def _SLASH_U : CLJoinedOrSeparate<"U">, HelpText<"Undefine macro">,
  MetaVarName<"<macro>">, Alias<U>;
def _SLASH_W0 : CLFlag<"W0">, HelpText<"Disable all warnings">, Alias<w>;
def _SLASH_W1 : CLFlag<"W1">, HelpText<"Enable -Wall">, Alias<Wall>;
def _SLASH_W2 : CLFlag<"W2">, HelpText<"Enable -Wall">, Alias<Wall>;
def _SLASH_W3 : CLFlag<"W3">, HelpText<"Enable -Wall">, Alias<Wall>;
def _SLASH_W4 : CLFlag<"W4">, HelpText<"Enable -Wall">, Alias<Wall>;
def _SLASH_Wall : CLFlag<"Wall">, HelpText<"Enable -Wall">, Alias<Wall>;
def _SLASH_WX : CLFlag<"WX">, HelpText<"Treat warnings as errors">,
  Alias<W_Joined>, AliasArgs<["error"]>;
def _SLASH_WX_ : CLFlag<"WX-">, HelpText<"Do not treat warnings as errors">,
  Alias<W_Joined>, AliasArgs<["no-error"]>;
def _SLASH_w_flag : CLFlag<"w">, HelpText<"Disable all warnings">, Alias<w>;
def _SLASH_Zs : CLFlag<"Zs">, HelpText<"Syntax-check only">,
  Alias<fsyntax_only>;


// Non-aliases:

def _SLASH_M_Group : OptionGroup<"</M group>">, Group<cl_compile_Group>;

def _SLASH_FA : CLFlag<"FA">,
  HelpText<"Output assembly code file during compilation">;
def _SLASH_Fa : CLJoined<"Fa">,
  HelpText<"Output assembly code to this file during compilation">,
  MetaVarName<"<file or directory>">;
def _SLASH_fallback : CLCompileFlag<"fallback">,
  HelpText<"Fall back to cl.exe if clang-cl fails to compile">;
def _SLASH_FI : CLJoinedOrSeparate<"FI">,
  HelpText<"Include file before parsing">, Alias<include_>;
def _SLASH_Fe : CLJoined<"Fe">,
  HelpText<"Set output executable file or directory (ends in / or \\)">,
  MetaVarName<"<file or directory>">;
def _SLASH_Fo : CLCompileJoined<"Fo">,
  HelpText<"Set output object file, or directory (ends in / or \\)">,
  MetaVarName<"<file or directory>">;
def _SLASH_LD : CLFlag<"LD">, HelpText<"Create DLL">;
def _SLASH_LDd : CLFlag<"LDd">, HelpText<"Create debug DLL">;
def _SLASH_link : CLRemainingArgs<"link">,
  HelpText<"Forward options to the linker">, MetaVarName<"<options>">;
def _SLASH_MD : Option<["/", "-"], "MD", KIND_FLAG>, Group<_SLASH_M_Group>,
  Flags<[CLOption, DriverOption]>, HelpText<"Use DLL run-time">;
def _SLASH_MDd : Option<["/", "-"], "MDd", KIND_FLAG>, Group<_SLASH_M_Group>,
  Flags<[CLOption, DriverOption]>, HelpText<"Use DLL debug run-time">;
def _SLASH_MT : Option<["/", "-"], "MT", KIND_FLAG>, Group<_SLASH_M_Group>,
  Flags<[CLOption, DriverOption]>, HelpText<"Use static run-time">;
def _SLASH_MTd : Option<["/", "-"], "MTd", KIND_FLAG>, Group<_SLASH_M_Group>,
  Flags<[CLOption, DriverOption]>, HelpText<"Use static debug run-time">;
def _SLASH_Tc : CLCompileJoinedOrSeparate<"Tc">,
  HelpText<"Specify a C source file">, MetaVarName<"<filename>">;
def _SLASH_TC : CLCompileFlag<"TC">, HelpText<"Treat all source files as C">;
def _SLASH_Tp : CLCompileJoinedOrSeparate<"Tp">,
  HelpText<"Specify a C++ source file">, MetaVarName<"<filename>">;
def _SLASH_TP : CLCompileFlag<"TP">, HelpText<"Treat all source files as C++">;


// Ignored:

def _SLASH_analyze_ : CLIgnoredFlag<"analyze-">;
def _SLASH_errorReport : CLIgnoredJoined<"errorReport">;
def _SLASH_GF : CLIgnoredFlag<"GF">;
def _SLASH_GS_ : CLIgnoredFlag<"GS-">;
def _SLASH_nologo : CLIgnoredFlag<"nologo">;
def _SLASH_Ob1 : CLIgnoredFlag<"Ob1">;
def _SLASH_Ob2 : CLIgnoredFlag<"Ob2">;
def _SLASH_RTC : CLIgnoredJoined<"RTC">;
def _SLASH_sdl : CLIgnoredFlag<"sdl">;
def _SLASH_sdl_ : CLIgnoredFlag<"sdl-">;
def _SLASH_vmg : CLIgnoredFlag<"vmg">;
def _SLASH_w : CLIgnoredJoined<"w">;
def _SLASH_Zc_forScope : CLIgnoredFlag<"Zc:forScope">;
def _SLASH_Zc_wchar_t : CLIgnoredFlag<"Zc:wchar_t">;
def _SLASH_Zm : CLIgnoredJoined<"Zm">;


// Unsupported:

def _SLASH_arch : CLJoined<"arch:">;
def _SLASH_bigobj : CLFlag<"bigobj">;
def _SLASH_EH : CLJoined<"EH">;
def _SLASH_FA_joined : CLJoined<"FA">;
def _SLASH_Fd : CLJoined<"Fd">;
def _SLASH_Fp : CLJoined<"Fp">;
def _SLASH_fp : CLJoined<"fp">;
def _SLASH_Gd : CLFlag<"Gd">;
def _SLASH_GL : CLFlag<"GL">;
def _SLASH_GL_ : CLFlag<"GL-">;
def _SLASH_Gm : CLFlag<"Gm">;
def _SLASH_Gm_ : CLFlag<"Gm-">;
def _SLASH_GS : CLFlag<"GS">;
def _SLASH_Gy : CLFlag<"Gy">;
def _SLASH_Gy_ : CLFlag<"Gy-">;
def _SLASH_GZ : CLFlag<"GZ">;
def _SLASH_MP : CLJoined<"MP">;
def _SLASH_o : CLJoinedOrSeparate<"o">;
def _SLASH_vmb : CLFlag<"vmb">;
def _SLASH_vmm : CLFlag<"vmm">;
def _SLASH_vms : CLFlag<"vms">;
def _SLASH_vmv : CLFlag<"vmv">;
def _SLASH_Yu : CLJoined<"Yu">;
def _SLASH_Za : CLFlag<"Za">;
def _SLASH_Zc : CLJoined<"Zc:">;
def _SLASH_ZI : CLFlag<"ZI">;
def _SLASH_Zi : CLFlag<"Zi">;
