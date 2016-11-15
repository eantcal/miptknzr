//  
// This file is part of MipTknzr Library Project
// Copyright (c) Antonino Calderone (antonino.calderone@gmail.com)
// All rights reserved.  
// Licensed under the MIT License. 
// See COPYING file in the project root for full license information.
//


/* -------------------------------------------------------------------------- */

#ifndef __MIP_TKNZR_H__
#define __MIP_TKNZR_H__


/* -------------------------------------------------------------------------- */

#include "mip_token.h"
#include "mip_base_tknzr.h"
#include "mip_base_esc_cnvrtr.h"

#include <memory>
#include <istream>
#include <set>
#include <map>
#include <string>


/* -------------------------------------------------------------------------- */

namespace mip {


/* -------------------------------------------------------------------------- */

class tknzr_t : public base_tknzr_t
{
    friend class tknzr_bldr_t;

public:
    using ml_commdef_t = std::pair<std::string, std::string>;

    //! Return next token found in a given input stream
    std::unique_ptr<base_token_t> next(std::istream & is) override;

    //! dtor
    virtual ~tknzr_t();

private:
    tknzr_t() noexcept {}
    tknzr_t(const tknzr_t&) = delete;
    tknzr_t& operator=(const tknzr_t&) = delete;

    enum class get_t {
        JUST_TKN,
        WHOLE_LN
    };

    size_t _offset = 0;
    size_t _line_number = 0;

    std::string _textline;
    std::string _eol_seq;
    std::string _other_token;

    bool _eof = false;

    static bool _is_prefix_ch(
        const char ch, 
        const std::set<std::string> & tknset);

    static std::set<std::string>::const_reverse_iterator _search_token(
        const std::set<std::string>& tknset,
        std::string & line);

    static std::string _extract_token(
        const std::string& set_tkn,
        std::string & line,
        get_t cut_type);

    bool _ml_comment_begin(
        const std::set<ml_commdef_t> & tknset,
        std::string & end_comment);

    void _reset();

    bool _getline(
        std::istream & is, 
        std::string & line, 
        std::string& eol_s, 
        bool & eof);

    std::unique_ptr<base_token_t> _extract_comment(
        std::string & comment,
        size_t end_comment_offset,
        std::string& end_comment,
        size_t line_number,
        size_t offset);

    std::unique_ptr<base_token_t> _search_eof();
    std::unique_ptr<base_token_t> _search_eol();
    std::unique_ptr<base_token_t> _search_other_tkn();
    std::unique_ptr<base_token_t> _get_comment(std::istream & is);

    std::unique_ptr<base_token_t> _get_tkn(
        const std::set<std::string> & tknset,
        token_t::tcl_t tkncl,
        get_t cut_type);

    std::unique_ptr<base_token_t> _get_string();

    std::set<std::string> _blkdef;
    std::set<std::string> _atomdef;
    std::set<base_tknzr_t::eol_t> _eoldef;
    std::set<std::string> _sl_comdef;
    std::set<ml_commdef_t> _ml_comdef;
    std::map< char /*quote*/, std::shared_ptr<base_esc_cnvrtr_t > > _strdef;
};


/* -------------------------------------------------------------------------- */

} // namespace mip


/* -------------------------------------------------------------------------- */

#endif // __MIP_TKNZR_H__
