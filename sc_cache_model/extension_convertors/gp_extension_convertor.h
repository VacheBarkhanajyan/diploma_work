
#include <vector>
#include "uvmc.h"

#include "cache_optional_fields_extension.h"

struct gp_extension_convertor : public uvmc_converter<tlm_generic_payload>
{
    static void do_pack(const tlm_generic_payload &t, uvmc_packer &packer);
 
    static void do_unpack(tlm_generic_payload &t, uvmc_packer &packer);

};
