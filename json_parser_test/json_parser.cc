//  
// This file is part of MipTknzr Library Project
// Copyright (c) Antonino Calderone (antonino.calderone@gmail.com)
// All rights reserved.  
// Licensed under the MIT License. 
// See COPYING file in the project root for full license information.
//


/* -------------------------------------------------------------------------- */

#include <assert.h>
#include <sstream>
#include <iostream>
#include <list>
#include <vector>
#include <unordered_map>
#include <memory>
#include <set>
#include <map>
#include <fstream>
#include <string>

#ifdef _UNICODE
#include <locale> 
#include <codecvt>
#include <iomanip>
#endif

#include "mip_unicode.h"
#include "mip_tknzr_bldr.h"
#include "mip_esc_cnvrtr.h"

#include "mip_json.h"


/* -------------------------------------------------------------------------- */

namespace mip {

class json_parser_t {
private:
    int _level = 0;

    static string_t spc(int i) noexcept {
        string_t s;
        while (i-- > 0) {
            s += _T(" ");
        }
        return s;
    }

    struct with {
        std::unique_ptr<base_token_t>& _tkn;

        with(std::unique_ptr<base_token_t>& tkn) noexcept : _tkn(tkn) {}

        friend _ostream & operator<<(_ostream &os, const with & obj) {
            if (obj._tkn) {
                os << _T(" at ") << obj._tkn->line() + 1 << _T(":") << obj._tkn->offset() + 1;
            }
            return os;
        }
    };

    static constexpr const char_t* err_reading_stream() { return _T("reading input stream"); }
    static constexpr const char_t* err_val_expected() { return _T("value expected"); }
    static constexpr const char_t* err_inv_value() { return _T("invalid value"); }
    static constexpr const char_t* err_str_expected() { return _T("string expected"); }
    static constexpr const char_t* err_colon_expected() { return _T("':' expected"); }
    static constexpr const char_t* err_unmatching_curly_brace() { return _T("'}' or ',' expected"); }
    static constexpr const char_t* err_curly_brace_expected() { return _T("'{' expected"); }
public:
    using result_t = std::pair<bool, json_obj_t::handle_t>;

    json_parser_t() {
        tknzr_bldr_t tknzbldr;

        tknzbldr.def_atom(_T("["));
        tknzbldr.def_atom(_T("]"));
        tknzbldr.def_atom(_T("}"));
        tknzbldr.def_atom(_T("{"));
        tknzbldr.def_atom(_T(";"));
        tknzbldr.def_atom(_T(","));
        tknzbldr.def_atom(_T(":"));

        tknzbldr.def_blank(_T(" "));
        tknzbldr.def_blank(_T("\r")); // treat \r like a blank
        tknzbldr.def_blank(_T("\t"));

        tknzbldr.def_eol(mip::base_tknzr_t::eol_t::LF); // linefeed is end of line marker

        tknzbldr.def_string(_T('\"'), nullptr);

        _tknzr = tknzbldr.build();

        assert(_tknzr);
    }

    std::unique_ptr<base_token_t> get_token(_istream & is) {
        while (!is.bad()) {
            auto tkn = _tknzr->next(is);

            if (!tkn) {
                return nullptr;
            }

            const auto tkntype = tkn->type();

            assert(tkntype != token_t::tcl_t::COMMENT);

            // just ignore blanks, eol
            if (tkntype == token_t::tcl_t::BLANK ||
                tkntype == token_t::tcl_t::END_OF_LINE ||
                tkntype == token_t::tcl_t::COMMENT) {
                continue;
            }

            return tkn;
        }

        return nullptr;
    }

    result_t parse_number(const base_token_t & tkn, _istream & is, _ostream & err)
    {
        json_obj_t::handle_t ret;

        auto tt = tkn.type();

        switch (tt) {

        case token_t::tcl_t::ATOM:
        case token_t::tcl_t::STRING:
        case token_t::tcl_t::END_OF_FILE:

            err
                << err_val_expected()
                << _T(" at ") << tkn.line() + 1 
                << _T(":") << tkn.offset() + 1;

            return std::make_pair(false, nullptr);

        case token_t::tcl_t::OTHER:
        default:
            try {
                const auto value = tkn.value();

                if (value.size() > 1 && (
                    value[0] != '-' &&
                    value[0] != '.' &&
                    (value[0] < '0' || value[0] > '9')))
                {
                    err
                        << err_inv_value()
                        << _T(" at ") << tkn.line() + 1 
                        << _T(":") << tkn.offset() + 1;
                    
                    return std::make_pair(false, nullptr);
                }

                bool double_num =
                    (value.find('.') != std::string::npos) ||
                    (value.find('e') != std::string::npos) ||
                    (value.find('E') != std::string::npos);

                size_t idx = 0;

                if (double_num) {
                    const double dv = std::stod(tkn.value(), &idx);
#ifdef _DEBUG
                    std::_cout << spc(_level * 2) << dv;
#endif

                    ret = std::make_unique<json_obj_t>(dv);
                }
                else {
                    if (value.size() > 1 && value[0] == '-') {
                        const int64_t ullv = std::stoull(tkn.value(), &idx);
                        ret = std::make_unique<json_obj_t>(ullv);
                    }
                    else {
                        const int64_t llv = std::stoll(tkn.value(), &idx);
                        ret = std::make_unique<json_obj_t>(llv);
                    }
                }

                if (idx != value.size()) {
                    err
                        << err_inv_value()
                        << _T(" at ") 
                        << tkn.line() + 1 
                        << _T(":") 
                        << tkn.offset() + 1;

                    return std::make_pair(false, nullptr);
                }
            }
            catch (std::exception &) {
                err
                    << err_inv_value()
                    << _T(" at ") 
                    << tkn.line() + 1 
                    << _T(":") << tkn.offset() + 1;

                return std::make_pair(false, nullptr);
            }

            break;
        }

        return std::make_pair(true, std::move(ret));
    }

    result_t parse_value(_istream & is, _ostream & err)
    {
        json_obj_t::handle_t ret;

        auto tkn = get_token(is);

        if (!tkn) {
            err << err_reading_stream() << with(tkn);
            return std::make_pair(false, nullptr);
        }

        auto tt = tkn->type();

        switch (tt) {
        case token_t::tcl_t::END_OF_FILE:
            err << err_val_expected() << with(tkn);
            return std::make_pair(false, nullptr);

        case token_t::tcl_t::STRING:
#ifdef _DEBUG
            std::_cout
                << spc(_level * 2)
                << _T("\"") << tkn->value() << "\"" << std::endl;
#endif
            ret = std::make_unique<json_obj_t>(tkn->value());
            break;


        case token_t::tcl_t::ATOM:
            if (tkn->value() == _T("{")) {
                return parse_object(is, err);
            }
            else if (tkn->value() == _T("[")) {
                return parse_array(is, err);
            }

            return std::make_pair(false, nullptr);

        case token_t::tcl_t::OTHER:
        default:
            if (tkn->value() == _T("null")) {
#ifdef _DEBUG
                std::_cout
                    << spc(_level * 2)
                    << _T("null") << std::endl;
#endif
                ret = std::make_unique<json_obj_t>();
                break;
            }
            else if (tkn->value() == _T("true")) {
#ifdef _DEBUG
                std::_cout
                    << spc(_level * 2)
                    << _T("true") << std::endl;
#endif
                ret = std::make_unique<json_obj_t>(true);
                break;
            }
            else if (tkn->value() == _T("false")) {
#ifdef _DEBUG
                std::_cout
                    << spc(_level * 2)
                    << _T("false") << std::endl;
#endif
                ret = std::make_unique<json_obj_t>(false);
                break;
            }
            else {
                return parse_number(*tkn, is, err);
            }
        }

        return std::make_pair(true, std::move(ret));
    }

    result_t parse_object(_istream & is, _ostream & err) {
        ++_level;

        json_obj_t::handle_t obj = json_obj_t::make_object();

        do {
            auto tkn = get_token(is);

            if (!tkn) {
                err << err_reading_stream() << with(tkn);
                return std::make_pair(false, nullptr);
            }

            if (tkn->type() == token_t::tcl_t::END_OF_FILE) {
                break;
            }

            if (tkn->type() != token_t::tcl_t::STRING) {
                err << err_str_expected() << with(tkn);
                return std::make_pair(false, nullptr);
            }

            const auto item_name = tkn->value();

#ifdef _DEBUG
            std::_cout
                << spc(_level * 2)
                << "{ \""
                << item_name
                << "\"" << std::endl;
#endif

            tkn = get_token(is);

            if (!tkn) {
                err << err_reading_stream() << with(tkn);
                return std::make_pair(false, nullptr);
            }

            if (tkn->type() == token_t::tcl_t::END_OF_FILE) {
                err << err_val_expected() << with(tkn);
                return std::make_pair(false, nullptr);
            }

            if (tkn->type() != token_t::tcl_t::ATOM || tkn->value() != _T(":")) {
                err << err_colon_expected() << with(tkn);
                return std::make_pair(false, nullptr);
            }

            auto ret = parse_value(is, err);

            if (!ret.first) {
                return std::make_pair(false, nullptr);
            }

            obj->add_member(item_name, std::move(ret.second));

#ifdef _DEBUG
            std::_cout << std::endl;
#endif

            tkn = get_token(is);

            if (!tkn) {
                err << err_reading_stream() << with(tkn);
                return std::make_pair(false, nullptr);
            }

            if (tkn->type() == token_t::tcl_t::END_OF_FILE) {
                err << err_val_expected() << with(tkn);
                return std::make_pair(false, nullptr);
            }

            if (tkn->type() != token_t::tcl_t::ATOM ||
                (tkn->value() != _T("}") && (tkn->value() != _T(","))))
            {
                err << err_unmatching_curly_brace() << with(tkn);
                return std::make_pair(false, nullptr);
            }

            if (tkn->value() == _T("}")) {
#ifdef _DEBUG
                std::_cout
                    << spc(_level * 2)
                    << "}" << std::endl;

                --_level;
#endif
                break;
            }

        } while (1);

        return std::make_pair(true, std::move(obj));
    }

    result_t parse_array(_istream & is, _ostream & err) {
        ++_level;

#ifdef  _DEBUG
        std::_cout
            << spc(_level * 2)
            << "[" << std::endl;
#endif

        json_obj_t::handle_t obj = json_obj_t::make_array();

        do {
            auto ret = parse_value(is, err);

            if (!ret.first) {
                return std::make_pair(false, nullptr);
            }

            obj->push_back(std::move(ret.second));

            auto tkn = get_token(is);

            if (!tkn) {
                err << err_reading_stream() << with(tkn);
                return std::make_pair(false, nullptr);
            }

            if (tkn->type() == token_t::tcl_t::END_OF_FILE) {
                err << err_val_expected() << with(tkn);
                return std::make_pair(false, nullptr);
            }

            if (tkn->type() != token_t::tcl_t::ATOM) {
                err << err_unmatching_curly_brace() << with(tkn);
                return std::make_pair(false, nullptr);
            }

            if (tkn->value() == _T("]")) {

#ifdef _DEBUG
                std::_cout
                    << spc(_level * 2)
                    << "]" << std::endl;

                --_level;
#endif

                break;
            }
            else if (tkn->value() != _T(",")) {
                err << err_unmatching_curly_brace() << with(tkn);
                return std::make_pair(false, nullptr);
            }

        } while (true);

        return std::make_pair(true, std::move(obj));
    }

    result_t parse(_istream & is, _ostream & err) {

        auto tkn = get_token(is);

        if (!tkn) {
            err << err_reading_stream() << with(tkn);
            return std::make_pair(false, nullptr);
        }

        if (tkn->type() == token_t::tcl_t::END_OF_FILE) {
            return std::make_pair(true, nullptr);
        }


        if (tkn->type() != base_token_t::tcl_t::ATOM ||
            (tkn->value() != _T("{") && (tkn->value() != _T("["))))
        {
            err << err_curly_brace_expected() << with(tkn);
            return std::make_pair(false, nullptr);
        }

        bool obj = tkn->value() == _T("{");

        return obj ?
            parse_object(is, err) :
            parse_array(is, err);
    }

private:
    std::unique_ptr< base_tknzr_t > _tknzr;
};


/* -------------------------------------------------------------------------- */

}


/* -------------------------------------------------------------------------- */

int main(int argc, mip::char_t* argv[])
{
    if (argc<2) {
        std::_cerr << _T("File name missing") << std::endl;
        return 1;
    }

    mip::json_parser_t parser;
    const mip::char_t* filename = argv[1];
    mip::_ifstream is(filename, std::ios::in | std::ios::binary);

    if (!is.is_open()) {
        std::_cerr << _T("Cannot open ") << filename << std::endl;
        return false;
    }

#ifdef _UNICODE
    const std::locale utf16_locale
        = std::locale(
            std::locale(),
            new std::codecvt_utf16<
            wchar_t,
            0x10ffff,
            std::codecvt_mode(std::little_endian | std::consume_header)>());

    is.imbue(utf16_locale);
#endif

    if (!is.is_open() || is.bad()) {
        std::cerr << _T("error reading the input stream");
        return false;
    }

    auto res = parser.parse(is, std::_cerr);

    if (! res.first) {
        std::_cerr << std::endl << _T("Parser error") << std::endl;
        return 1;
    }

    if (res.second) {
        std::_cout << *res.second << std::endl;
    }
    else {
        std::_cout << _T("null") << std::endl;
    }

    std::_cout << _T("END.") << std::endl;

#ifdef _DEBUG
    int wait;
    std::_cin >> wait;
#endif

    return 0;
}
