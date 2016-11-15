//  
// This file is part of MipTknzr Library Project
// Copyright (c) Antonino Calderone (antonino.calderone@gmail.com)
// All rights reserved.  
// Licensed under the MIT License. 
// See COPYING file in the project root for full license information.
//


/* -------------------------------------------------------------------------- */

#include "mip_esc_cnvrtr.h"

#include <memory>
#include <istream>


/* -------------------------------------------------------------------------- */

namespace mip {


/* -------------------------------------------------------------------------- */

void esc_cnvrtr_t::_tail(std::string & str, size_t cnt)
{
    if (str.size() <= cnt) {
        str.clear();
    }
    else {
        str = str.substr(cnt, str.size() - cnt);
    }
}


/* -------------------------------------------------------------------------- */

bool esc_cnvrtr_t::_octal2dec(const std::string & str, unsigned int& res, size_t & cnt)
{
    if (str.size() < 2) {
        return false;
    }

    cnt = str.size();
    if (cnt > 3) {
        cnt = 3;
    }

    res = 0;
    for (size_t i = 0; i < cnt; ++i) {

        auto ch = str[i];

        if (ch<'0' || ch>'7') {
            return false;
        }

        res += (ch - '0') << (3 * (cnt - i - 1));
    }

    return true;
}


/* -------------------------------------------------------------------------- */

bool esc_cnvrtr_t::_hex2dec(const std::string & str, unsigned int& res, size_t & cnt)
{
    if (str.size() < 2 || (str[0] != 'x' && str[0] != 'X')) {
        return false;
    }

    cnt = str.size();
    std::string hex = str.substr(1, cnt - 1);

    size_t i = 0;
    for (; i < cnt; ++i) {
        const auto ch = ::tolower(hex[i]);

        bool ok = (ch >= '0' && ch <= '9') || (ch >= 'a' && ch < 'f');
        if (!ok)
            break;
    }

    hex = hex.substr(0, i);
    cnt = i + 1;

    try {
        res = std::stoi(hex, 0, 16);
    }
    catch (std::exception&) {
        return false;
    }

    return true;
}


/* -------------------------------------------------------------------------- */

bool esc_cnvrtr_t::convert(const std::string& str, size_t & rcnt, char & ch) const
{
    if (str.size() <= 1) {
        return false;
    }

    const char prefix = str[1];
    rcnt = 2;

    switch (prefix) {
    case '\'':
        ch = '\'';
        break;
    case '"':
        ch = '"';
        break;
    case '\\':
        ch = '\\';
        break;
    case 'n':
        ch = '\n';
        break;
    case 't':
        ch = '\t';
        break;
    case 'b':
        ch = '\b';
        break;
    case 'f':
        ch = '\f';
        break;
    case 'a':
        ch = '\a';
        break;
    case '?':
        ch = '?';
        break;
    case '0':
        ch = '\0';
        break;
    default:
        if (str.size() >= 3 && (str[1] >= '0' && str[1] <= '7')) {
            unsigned int res = 0;
            const bool ok = _octal2dec(str.c_str() + 1, res, rcnt);

            if (!ok) {
                return false;
            }

            ch = static_cast<char>(res);
            ++rcnt;

            break;
        }
        else if (str.size() >= 2 && (str[1] == 'x' || str[1] == 'X')) {
            unsigned int res = 0;
            const bool ok = _hex2dec(str.c_str() + 1, res, rcnt);

            if (!ok) {
                return false;
            }

            ch = static_cast<char>(res);
            ++rcnt;

            break;
        }
        return false;
    }


    return true;
}


/* -------------------------------------------------------------------------- */

} // namespace mip


/* -------------------------------------------------------------------------- */
