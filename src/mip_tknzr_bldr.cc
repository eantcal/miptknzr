//  
// This file is part of MipTknzr Library Project
// Copyright (c) Antonino Calderone (antonino.calderone@gmail.com)
// All rights reserved.  
// Licensed under the MIT License. 
// See COPYING file in the project root for full license information.
//


/* -------------------------------------------------------------------------- */

#include "mip_tknzr_bldr.h"

#include <cassert>
#include <memory>


/* -------------------------------------------------------------------------- */

namespace mip {


/* -------------------------------------------------------------------------- */

bool tknzr_bldr_t::_build_tknzr()
{
    if (!_tknzr) {
        _tknzr.reset(new tknzr_t());

        assert(_tknzr);

        if (!_tknzr) {
            return false;
        }
    }

    return true;
}


/* -------------------------------------------------------------------------- */

std::unique_ptr< base_tknzr_t > tknzr_bldr_t::build()
{
    return std::move(_tknzr);
}


/* -------------------------------------------------------------------------- */

bool tknzr_bldr_t::def_atom(const std::string& value)
{
    return _def_item(value, _tknzr->_atomdef);
}


/* -------------------------------------------------------------------------- */

bool tknzr_bldr_t::def_blank(const std::string& value)
{
    return _def_item(value, _tknzr->_blkdef);
}


/* -------------------------------------------------------------------------- */

bool tknzr_bldr_t::def_eol(const base_tknzr_t::eol_t& value)
{
    return _def_item(value, _tknzr->_eoldef);
}


/* -------------------------------------------------------------------------- */

bool tknzr_bldr_t::def_sl_comment(const std::string& prefix)
{
    return _def_item(prefix, _tknzr->_sl_comdef);
}


/* -------------------------------------------------------------------------- */

bool tknzr_bldr_t::def_ml_comment(
    const std::string& begin,
    const std::string& end)
{
    std::pair<std::string, std::string> value{ begin, end };
    return _def_item(value, _tknzr->_ml_comdef);
}


/* -------------------------------------------------------------------------- */

bool tknzr_bldr_t::def_string(char quote, std::shared_ptr<base_esc_cnvrtr_t> et)
{
    auto it = _tknzr->_strdef.find(quote);
    if (it != _tknzr->_strdef.end()) {
        return false;
    }

    _tknzr->_strdef[quote] = et;

    return true;
}


/* -------------------------------------------------------------------------- */

} // namespace mip


/* -------------------------------------------------------------------------- */
