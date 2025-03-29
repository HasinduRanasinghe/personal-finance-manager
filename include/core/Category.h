#ifndef CATEGORY_H
#define CATEGORY_H

#include <string>

struct Category {
    std::string name;
    std::string description;
    
    Category() {}
    
    Category(const std::string& name, const std::string& description)
        : name(name), description(description) {}
};

#endif // CATEGORY_H