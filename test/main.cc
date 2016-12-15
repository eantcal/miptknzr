//  
// This file is part of MipTknzr Library Project
// Copyright (c) Antonino Calderone (antonino.calderone@gmail.com)
// All rights reserved.  
// Licensed under the MIT License. 
// See COPYING file in the project root for full license information.
//


/* -------------------------------------------------------------------------- */

#include <assert.h>
#include <string>
#include <sstream>
#include <iostream>
#include <list>
#include <vector>
#include <unordered_map>
#include <memory>
#include <set>
#include <map>
#include <fstream>

#ifdef _UNICODE
#include <locale> 
#include <codecvt>
#include <iomanip>
#endif

#include "mip_unicode.h"
#include "mip_tknzr_bldr.h"
#include "mip_esc_cnvrtr.h"


/* -------------------------------------------------------------------------- */

int main(int argc, mip::char_t* argv[])
{
    std::wcout;

    mip::tknzr_bldr_t bldr;

    bldr.def_atom(_T("("));
    bldr.def_atom(_T(")"));
    bldr.def_atom(_T(">"));
    bldr.def_atom(_T("->"));
    bldr.def_atom(_T(">="));
    bldr.def_atom(_T(";"));
    bldr.def_atom(_T("<<"));

    bldr.def_sl_comment(_T("//"));
    bldr.def_sl_comment(_T("#"));
    bldr.def_blank(_T(" "));
    bldr.def_blank(_T("\r")); // treat \r like a blank
    bldr.def_blank(_T("\t"));

    bldr.def_eol(mip::base_tknzr_t::eol_t::LF); // linefeed is end of line marker

    bldr.def_string(_T('\"'), std::make_shared<mip::esc_cnvrtr_t>(_T('\\')));

    bldr.def_ml_comment(_T("/*"), _T("*/"));

    auto tknzr = bldr.build();

    mip::string_t filename;
    
    if (argc > 1) {
        filename = argv[1];
    }
    
    while (filename.empty()) {
        std::_cout << _T("Insert a file name then press ENTER: ");
        std::_cin >> filename;
    }
        
    mip::_ifstream is(filename, std::ios::in | std::ios::binary);

    if (!is.is_open()) {
        std::_cerr << _T("Cannot open input file") << std::endl;
        return 1;
    }

#ifdef _UNICODE
    if (is.is_open()) {

        const std::locale utf16_locale
            = std::locale(
                std::locale(), 
                new std::codecvt_utf16<
                    wchar_t, 
                    0x10ffff, 
                    std::codecvt_mode(std::little_endian | std::consume_header)>());

        is.imbue(utf16_locale);
    }
#endif

    while (is.is_open() && !is.bad()) {
        auto tkn = tknzr->next(is);

        if (!tkn) {
            std::_cerr << _T("Error parsing the input file") << std::endl;
            break;
        }

        std::_cout << *tkn << std::endl;

        if (tkn->type() == mip::token_t::tcl_t::END_OF_FILE) {
            std::_cout << _T("Success.") << std::endl;
            break;
        }
    }

}
