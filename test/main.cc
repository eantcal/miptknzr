
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

#include "../src/mip_tknzr_bldr.h"
#include "../src/mip_esc_cnvrtr.h"


/* -------------------------------------------------------------------------- */

int main()
{
    mip::tknzr_bldr_t bldr;

    bldr.def_atom("(");
    bldr.def_atom(")");
    bldr.def_atom(">");
    bldr.def_atom("->");
    bldr.def_atom(">=");
    bldr.def_atom(";");
    bldr.def_atom("<<");

    bldr.def_sl_comment("//");
    bldr.def_sl_comment("#");
    bldr.def_blank(" ");
    bldr.def_blank("\t");

    bldr.def_eol(mip::base_tknzr_t::eol_t::CR_LF);
    bldr.def_eol(mip::base_tknzr_t::eol_t::LF);

    bldr.def_string('\"', std::make_shared<mip::esc_cnvrtr_t>('\\'));

    bldr.def_ml_comment("/*","*/");

    auto tknzr = bldr.build();

    std::ifstream is("Makefile");

    while (is.is_open() && !is.bad()) {
        auto tkn = tknzr->next(is);

        if (!tkn) {
            std::cerr << "Error !" << std::endl;
            break;
        }

        std::cout << *tkn << std::endl;

        if (tkn->type() == mip::token_t::tcl_t::END_OF_FILE) {
            std::cout << "Success." << std::endl;
            break;
        }
    };

    int pausa = 0;
    std::cin >> pausa;

}
