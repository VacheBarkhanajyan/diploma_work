
#include "gp_extension_convertor.h"

void gp_extension_convertor::do_pack(const tlm_generic_payload &t, uvmc_packer &packer)
{
    cache_optional_fields_extension *ext;
    t.get_extension(ext);

    if (!ext)
    {
        UVMC_FATAL("AXI_PACK", "TLM GP does not have a extension", "");
    }
    uvmc_converter<tlm_generic_payload>::do_pack(t, packer); // pack Base GP

    packer << ext->vm_id;
    packer << ext->axi_id;
    packer << ext->cache;

    if (packer.does_uvmc_own_trans())
    {
        (const_cast<tlm_generic_payload &>(t)).clear_extension(ext);
        delete ext;
    }

}

void gp_extension_convertor::do_unpack(tlm_generic_payload &t, uvmc_packer &unpacker)
{

    cache_optional_fields_extension *ext;

    if (unpacker.does_uvmc_own_trans())
    {
        //        alloc
        ext = new cache_optional_fields_extension;
        t.set_extension(ext);
    }
    else 
    {
        t.get_extension(ext);
        if (!ext)
        {
            UVMC_FATAL("AXI_UNPACK", "TLM GP does not have a extension", "");
        }
    }

    uvmc_converter<tlm_generic_payload>::do_unpack(t, unpacker); // unpack for Base GP
    
    unpacker    >> ext->vm_id ;
    unpacker    >> ext->axi_id ;
    unpacker    >> ext->cache ;


    //uvmc_print<tlm_generic_payload>::print(t);

}

