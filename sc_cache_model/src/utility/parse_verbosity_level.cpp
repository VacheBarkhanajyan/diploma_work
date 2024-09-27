#include "parse_verbosity_level.h"


int parse_verbosity_level(std::string verbosity_level_str) {

    if (verbosity_level_str == "SC_DEBUG") {
        return sc_core::SC_DEBUG;
    }
    else if (verbosity_level_str == "SC_FULL") {
        return sc_core::SC_FULL;
    }
    else if (verbosity_level_str == "SC_HIGH") {
        return sc_core::SC_HIGH;
    }
    else if (verbosity_level_str == "SC_MEDIUM") {
        return sc_core::SC_MEDIUM;
    }
    else if (verbosity_level_str == "SC_LOW") {
        return sc_core::SC_LOW;
    }
    else if (verbosity_level_str == "SC_NONE") {
        return sc_core::SC_NONE;
    }
    else {
        return sc_core::SC_MEDIUM;
    }
}