/**
 * @file VppFile.h
 * @authors Bertello Ana, Carabajal Marcelo, Masino Facundo, Tola Lucas
 * @brief VppFile contiene metodos para el manejo basico de archivos.
 * Contiene todos los metodos basicos, necesarios y recurrentes que
 * se utilizan a la hora de realizar operaciones con los archivos de datos .vpp
 * Cada vez que una clase necesite crear o manipular un archivo .vpp, se
 * debera crear un objeto de tipo VppFile
 * @version 0.1
 * @date 2023-10-11
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <string>
template <class VppClass>
class VppFile {
public:
    VppFile(std::string fileName);

    int getTotalRegisters();
    VppClass readFile(int regNumber);
    bool readFile(VppClass *reg, int totalToRead);
    bool writeFile(VppClass reg);
    bool updateFile(VppClass reg, int regPos);
    bool deleteReg(int regNumber);
    bool markForDelete(int regNumber);
    int deleteAllMarked();

    template <typename AnyType>
    int searchReg(bool (*fCallback)(VppClass reg, AnyType value),
                  AnyType value);
    template <typename AnyType>
    int searchReg(bool (*fCallback)(VppClass reg, AnyType val1, AnyType val2),
                  AnyType val1, AnyType val2);
    // bool updateFile(int regNumber, bool *fCallback); // TODO: desarrollo
    // pendiente

private:
    std::string _fileName;
};

#include "VppFile_def.h"