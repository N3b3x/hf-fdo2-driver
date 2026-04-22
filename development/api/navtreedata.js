/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "HF-FDO2 Driver", "index.html", [
    [ "Table of contents", "index.html#autotoc_md1", null ],
    [ "Overview", "index.html#autotoc_md2", null ],
    [ "Features", "index.html#autotoc_md3", null ],
    [ "Quick start", "index.html#autotoc_md4", null ],
    [ "Documentation", "index.html#autotoc_md5", null ],
    [ "Examples", "index.html#autotoc_md6", null ],
    [ "Official references", "index.html#autotoc_md7", null ],
    [ "License", "index.html#autotoc_md8", null ],
    [ "API reference", "md_docs_2api__reference.html", [
      [ "Transport — fdo2::UartInterface<Derived>", "md_docs_2api__reference.html#autotoc_md10", null ],
      [ "Driver — fdo2::Driver<UartT>", "md_docs_2api__reference.html#autotoc_md11", null ],
      [ "Types — fdo2_types.hpp", "md_docs_2api__reference.html#autotoc_md12", null ],
      [ "Umbrella include", "md_docs_2api__reference.html#autotoc_md13", null ]
    ] ],
    [ "CMake integration", "md_docs_2cmake__integration.html", [
      [ "Standalone consumer", "md_docs_2cmake__integration.html#autotoc_md15", null ],
      [ "Variables of interest (cmake/hf_fdo2_build_settings.cmake)", "md_docs_2cmake__integration.html#autotoc_md16", null ],
      [ "Package config", "md_docs_2cmake__integration.html#autotoc_md17", null ]
    ] ],
    [ "Examples", "md_docs_2examples.html", [
      [ "ESP32-S3 (examples/esp32)", "md_docs_2examples.html#autotoc_md20", null ]
    ] ],
    [ "Hardware setup", "md_docs_2hardware__setup.html", [
      [ "Logic levels", "md_docs_2hardware__setup.html#autotoc_md22", null ],
      [ "UART parameters", "md_docs_2hardware__setup.html#autotoc_md23", null ],
      [ "Boot delay", "md_docs_2hardware__setup.html#autotoc_md24", null ],
      [ "Connector", "md_docs_2hardware__setup.html#autotoc_md25", null ],
      [ "ESP32-S3 shipped examples (default)", "md_docs_2hardware__setup.html#autotoc_md26", null ],
      [ "Fast versus verbose reads", "md_docs_2hardware__setup.html#autotoc_md27", null ]
    ] ],
    [ "HF-FDO2 driver documentation", "md_docs_2index.html", [
      [ "Documentation structure", "md_docs_2index.html#autotoc_md29", [
        [ "Getting started", "md_docs_2index.html#autotoc_md30", null ],
        [ "Hardware and integration", "md_docs_2index.html#autotoc_md31", null ],
        [ "Reference and examples", "md_docs_2index.html#autotoc_md32", null ],
        [ "Manufacturer", "md_docs_2index.html#autotoc_md33", null ]
      ] ],
      [ "Need help?", "md_docs_2index.html#autotoc_md34", null ]
    ] ],
    [ "Installation", "md_docs_2installation.html", [
      [ "Requirements", "md_docs_2installation.html#autotoc_md36", null ],
      [ "Repository layout", "md_docs_2installation.html#autotoc_md37", null ],
      [ "Submodule (recommended)", "md_docs_2installation.html#autotoc_md38", null ],
      [ "ESP-IDF component", "md_docs_2installation.html#autotoc_md39", null ],
      [ "Doxygen (optional)", "md_docs_2installation.html#autotoc_md40", null ]
    ] ],
    [ "Quick start", "md_docs_2quickstart.html", [
      [ "Wire and baud", "md_docs_2quickstart.html#autotoc_md42", null ],
      [ "Minimal adapter (sketch)", "md_docs_2quickstart.html#autotoc_md43", null ],
      [ "First measurements", "md_docs_2quickstart.html#autotoc_md44", null ],
      [ "Volume % O₂", "md_docs_2quickstart.html#autotoc_md45", null ]
    ] ],
    [ "Troubleshooting", "md_docs_2troubleshooting.html", [
      [ "No response or garbage data", "md_docs_2troubleshooting.html#autotoc_md47", null ],
      [ "Timeout or ProtocolError", "md_docs_2troubleshooting.html#autotoc_md48", null ],
      [ "DeviceError after #ERRO", "md_docs_2troubleshooting.html#autotoc_md49", null ],
      [ "Status S not 0 or 1", "md_docs_2troubleshooting.html#autotoc_md50", null ],
      [ "Need more help?", "md_docs_2troubleshooting.html#autotoc_md51", null ]
    ] ],
    [ "FDO2-G2 UART protocol", "md_docs_2uart__protocol.html", [
      [ "Electrical and framing", "md_docs_2uart__protocol.html#autotoc_md53", null ],
      [ "Implemented commands", "md_docs_2uart__protocol.html#autotoc_md54", null ],
      [ "Numeric decoding", "md_docs_2uart__protocol.html#autotoc_md55", null ],
      [ "Volume % O₂", "md_docs_2uart__protocol.html#autotoc_md56", null ],
      [ "Errors", "md_docs_2uart__protocol.html#autotoc_md57", null ],
      [ "Out of scope in this driver", "md_docs_2uart__protocol.html#autotoc_md58", null ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", null ],
        [ "Functions", "functions_func.html", null ],
        [ "Variables", "functions_vars.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"annotated.html",
"structfdo2_1_1MrawReading.html#a61d95787da3a3faf2006fd62c827a09d"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';