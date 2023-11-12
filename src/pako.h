#ifndef PAKO_H
#define PAKO_H

class Pako {
private:
    Package pkg;
    Print out;
    Database db;
public:
    int8_t install(std::vector<std::string> arguments);
};

#endif
