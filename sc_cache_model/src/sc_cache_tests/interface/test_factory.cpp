#include "test_factory.h"

std::map<std::string, test_factory::create_function>& getFactoryMap() {
    
    static std::map<std::string, test_factory::create_function> factory_map;

    return factory_map;
}

test_factory::test_factory(sc_core::sc_module_name name) 
                              : sc_module(name) 
{
    
}


test_factory* test_factory::create(const std::string& test_name, const cache_params& params) {
    
    std::map<std::string, test_factory::create_function>& factory_map = getFactoryMap();
    std::map<std::string, test_factory::create_function>::iterator it = factory_map.find(test_name);

    if (it != factory_map.end()) {
        return it->second(test_name.c_str(), params);
    } else {
        return nullptr;
    }
}

bool test_factory::register_test(const std::string& test_name, create_function create_func) {
    std::map<std::string, test_factory::create_function>& factory_map = getFactoryMap();

    factory_map[test_name] = create_func;
    return true;
}