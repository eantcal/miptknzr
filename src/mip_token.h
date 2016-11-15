//  
// This file is part of MipTknzr Library Project
// Copyright (c) Antonino Calderone (antonino.calderone@gmail.com)
// All rights reserved.  
// Licensed under the MIT License. 
// See COPYING file in the project root for full license information.
//


/* -------------------------------------------------------------------------- */

#ifndef __MIP_TOKEN_H__
#define __MIP_TOKEN_H__


/* -------------------------------------------------------------------------- */

#include "mip_base_token.h"

#include <string>


/* -------------------------------------------------------------------------- */

namespace mip {


/* -------------------------------------------------------------------------- */

class token_t : public base_token_t
{
private:
    //! token type
    tcl_t _type = tcl_t::OTHER; 

    //! token value
    std::string _value;  
 
    //! text line number
    size_t _line = 0;    

    //! token offset in the text line
    size_t _offset = 0;  

    //! quote of any string token
    char _quote = 0; 

    //! escape sequence prefix; defined for token representing string
    char _esc = 0;   

public:
    token_t(
        const tcl_t& type,
        const std::string& value,
        size_t line,
        size_t column,
        char quote = 0,
        char esc = 0)
        noexcept
        :
        _type(type),
        _value(value),
        _line(line),
        _offset(column),
        _quote(quote),
        _esc(esc)
    {}

    //! return quote and escape sequence prefix
    std::pair<char, char> get_quote_esc() const noexcept override {
        return std::pair<char, char>(_quote, _esc);
    }

    //! return token type
    tcl_t type() const noexcept override {
        return _type;
    }

    //! return token value
    const std::string& value() const noexcept override {
        return _value;
    }

    //! return token line number
    size_t line() const noexcept override {
        return _line;
    }

    //! return the token offset in the source text line
    size_t offset() const noexcept override {
        return _offset;
    }

    //! dtor
    virtual ~token_t() {}
};


/* -------------------------------------------------------------------------- */

} // namespace mip


/* -------------------------------------------------------------------------- */

#endif // __MIP_TOKEN_H__
