//  
// This file is part of MipTknzr Library Project
// Copyright (c) Antonino Calderone (antonino.calderone@gmail.com)
// All rights reserved.  
// Licensed under the MIT License. 
// See COPYING file in the project root for full license information.
//


/* -------------------------------------------------------------------------- */

#include "mip_base_token.h"


/* -------------------------------------------------------------------------- */

namespace mip {


/* -------------------------------------------------------------------------- */

const char* base_token_t::type2str(tcl_t type) {
    switch (type) {
    case tcl_t::ATOM:
        return "atomic";
    case tcl_t::BLANK:
        return "blank";
    case tcl_t::COMMENT:
        return "comment";
    case tcl_t::END_OF_FILE:
        return "eof";
    case tcl_t::END_OF_LINE:
        return "eol";
    case tcl_t::OTHER:
        return "other";
    case tcl_t::STRING:
        return "string";
    default:
        break;
    }

    return "unknown";
}


/* -------------------------------------------------------------------------- */

std::ostream& operator<<(std::ostream& os, base_token_t& tkn) {
    os << "type:'"
        << base_token_t::type2str(tkn.type())
        << "' value:'"
        << tkn.value() << "' at "
        << tkn.line() + 1 << "." << tkn.offset() + 1
        << std::endl;

    return os;
}


/* -------------------------------------------------------------------------- */

}
