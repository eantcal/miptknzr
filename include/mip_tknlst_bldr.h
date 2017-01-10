//  
// This file is part of MipTknzr Library Project
// Copyright (c) Antonino Calderone (antonino.calderone@gmail.com)
// All rights reserved.  
// Licensed under the MIT License. 
// See COPYING file in the project root for full license information.
//


/* -------------------------------------------------------------------------- */

#ifndef __MIP_TKNLST_BLDR_H__
#define __MIP_TKNLST_BLDR_H__


/* -------------------------------------------------------------------------- */

#include "mip_token.h"
#include "mip_tknzr_bldr.h"

#include <list>
#include <memory>
#include <cassert>


/* -------------------------------------------------------------------------- */

namespace mip {


/* -------------------------------------------------------------------------- */

using tknlist_t = std::list<std::unique_ptr<token_t>>;


/* -------------------------------------------------------------------------- */

/** 
 *  This helper class can be use to build a list of tokens from an input 
 *  stream
 */
class tknlst_bldr_t {
public:
    /**
     * ctor
     * @param tknzr_bldr is a tokenizer builder object
     * @param blnks is a set of token should be treated as blanks
     */
    tknlst_bldr_t(
        tknzr_bldr_t && tknzr_bldr, 
        const std::set<token_t::tcl_t> & blnks = { 
                token_t::tcl_t::BLANK ,
                token_t::tcl_t::END_OF_LINE,
                token_t::tcl_t::COMMENT 
            }
        ) 
        noexcept :
        _tknzr_bldr(std::move(tknzr_bldr)),
        _blnks(blnks)
    {
    }


    /**
     * Builds a list of tokens from an input stream
     * @param is must be an input stream
     * @param nonblnks will hold non-blanks list of tokens
     * @return true in case of success, false otherwise
     */
    bool build(_istream& is, tknlist_t & nonblnks) noexcept {
        return _build(is, nonblnks);
    }


    /**
    * Builds two lists of tokens from an input stream separating 
    * non-blanks from blanks classified tokens
    * @param is must be an input stream
    * @param nonblnks will hold non-blank classified tokens
    * @param blnks will hold blank classified tokens
    * @return true in case of success, false otherwise
    */
    bool build(_istream& is, tknlist_t & nonblnks, tknlist_t & blnks ) noexcept {
        return _build(is, nonblnks, blnks);
    }

private:
    

    // -------------------------------------------------------------------------

    void _insert(std::unique_ptr<token_t> tkn, tknlist_t & nonblnks) noexcept {
        if (_blnks.find(tkn->type()) == _blnks.end()) {
            nonblnks.push_back(std::move(tkn));
        }
    }


    // -------------------------------------------------------------------------

    void _insert(
        std::unique_ptr<token_t> tkn,  
        tknlist_t & nonblnks,
        tknlist_t & blanks) noexcept
    {
        if (_blnks.find(tkn->type()) == _blnks.end()) {
            nonblnks.push_back(std::move(tkn));
        }
        else {
            blanks.push_back(std::move(tkn));
        }
    }


    // -------------------------------------------------------------------------

    template <class ... T>
    bool _build(_istream& is, T&& ... args) noexcept {
        _tknzr = _tknzr_bldr.build();

        if (!_tknzr) {
            return false;
        }

        while (! _tknzr->eos(is)) {
            if (is.bad()) {
                return false;
            }

            auto tkn = _tknzr->next(is);

            if (!tkn) {
                return false;
            }

            _insert(std::move(tkn), std::forward<T>(args)...);
        }

        return true;
    }


    // -------------------------------------------------------------------------

    tknzr_bldr_t _tknzr_bldr;
    std::set<token_t::tcl_t> _blnks;
    std::unique_ptr<base_tknzr_t> _tknzr;
};


/* -------------------------------------------------------------------------- */

}


/* -------------------------------------------------------------------------- */

#endif // __MIP_TKNLST_BLDR_H__
