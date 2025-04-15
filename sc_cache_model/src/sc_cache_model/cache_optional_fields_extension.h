#ifndef CACHE_OPTIONAL_FIELDS_EXTENSION_H
#define CACHE_OPTIONAL_FIELDS_EXTENSION_H


#include <systemc>
#include <tlm_utils/simple_initiator_socket.h>

class cache_optional_fields_extension : public tlm::tlm_extension<cache_optional_fields_extension> {

    public:

        int cache;

        cache_optional_fields_extension() {
            cache = 0;
        }

        ~cache_optional_fields_extension(){
        }

        virtual tlm::tlm_extension_base *clone() const {
            auto ret = new cache_optional_fields_extension;
            ret->copy_from(*this);

            return ret;
        }

        virtual void copy_from(const tlm::tlm_extension_base &_that) {
            const cache_optional_fields_extension &that = static_cast<const cache_optional_fields_extension &>(_that);

            cache      = that.cache;
        }

};

#endif // !CACHE_OPTIONAL_FIELDS_EXTENSION_H