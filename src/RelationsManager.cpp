#include "RelationsManager.h"

#include <iostream>

#include "ClientsManager.h"
#include "InputForm.h"
#include "ListView.h"
#include "PetsManager.h"
#include "rlutil.h"
#include "utils.h"

// habilitar para debug
/* RelationsManager::RelationsManager() {
    std::cout << "Relations manager incializado." << std::endl;
    if (VppConfigManager().isTesting()) {
        std::cout << "Modo de prueba activado." << std::endl;
    } else {
        std::cout << "Modo de prueba desactivado." << std::endl;
    }
    utils::pause();
} */

void RelationsManager::load() {
    InputForm idForm;
    PetRelations auxPetR;
    int nId = 0;
    bool alreadyExists = false;

    // pedir y buscar si el id ingresado existe

    idForm.setIntField("ID Relación", nId, 4);
    do {
        if (!retryIfIdExists(alreadyExists)) return;
        // si no completa el form, salir
        if (!idForm.fill()) return;
        alreadyExists = idExists(nId);
    } while (alreadyExists);

    // Si no existe la relacion, pedir el resto de datos
    auxPetR = loadForm();
    // Si no se completo el form, salir
    if (auxPetR.getPetId() == -1) return;

    // setear id ingresado
    auxPetR.setRelationId(nId);
    if (_petRelationsFile.writeFile(auxPetR)) {
        std::cout << "Relacion guardada con exito!\n";
    } else {
        std::cout << "Ocurrio un error al guardar el turno.\n";
    }
}

PetRelations RelationsManager::loadForm() {
    InputForm petRelsForm, petIdForm, clientIdForm, confirmForm;
    PetRelations auxPetR;
    PetsManager petsManager;
    ClientsManager clientsManager;
    bool owner;
    int clientId = 0, petId = 0;
    bool alreadyExists = true;

    // pedir y buscar si el id mascota ingresado existe
    petIdForm.setIntField("ID Mascota", petId, 4);
    do {
        // si no existe, preguntar si quiere reintentar
        if (!retryIfIdNotExists(alreadyExists)) return auxPetR;
        // si no completa el form, salir
        if (!petIdForm.fill()) return auxPetR;
        alreadyExists = petsManager.idExists(petId);
    } while (!alreadyExists);  // si no existe, volver a pedir

    // pedir y buscar si el id cliente ingresado existe
    alreadyExists = true;
    clientIdForm.setIntField("ID Cliente", clientId, 4);
    do {
        // si no existe, preguntar si quiere reintentar
        if (!retryIfIdNotExists(alreadyExists)) return auxPetR;
        // si no completa el form, salir
        if (!clientIdForm.fill()) return auxPetR;
        alreadyExists = clientsManager.idExists(clientId);
    } while (!alreadyExists);  // si no existe, volver a pedir

    // Verificar que no sea una relación cliente/mascota existente
    if (relationExists(petId, clientId)) {
        std::cout << "Ya existe una relación entre la mascota y el cliente.\n";
        std::cout
            << "Si desea puede editarla desde el menú 'Editar relación'\n";
        utils::pause();
        return auxPetR;
    }

    petRelsForm.setBoolField("¿Es propietario [SI/NO]?", owner);

    if (!petRelsForm.fill()) return auxPetR;

    // Si se define como propietario, se actualiza el archivo de mascotas
    if (owner) {
        bool confirm;
        std::cout << "Se reemplazará el dueño principal de la mascota.\n Desea "
                     "continuar?";
        confirmForm.setBoolField("[SI/NO]", confirm);
        if (!confirmForm.fill()) return auxPetR;
        if (!confirm) return auxPetR;
        bool successNewOwner = newOwner(petId, clientId);
        if (!successNewOwner) return auxPetR;
    }

    auxPetR.setPetId(petId);
    auxPetR.setClientId(clientId);
    auxPetR.setOwner(owner);
    auxPetR.setStatus(true);

    return auxPetR;
}

PetRelations RelationsManager::editForm(int regPos) {
    InputForm petRelsForm(true), petIdForm(true, true),
        clientIdForm(true, true);
    PetRelations auxPetR, auxFormR;
    PetsManager petsManager;
    ClientsManager clientsManager;
    int petId, clientId, nId;
    bool owner, alreadyExists = true;

    auxPetR = _petRelationsFile.readFile(regPos);
    if (auxPetR.getPetId() == -1) {
        std::cout << "Ocurrio un error al leer los registros.\n";
        return auxPetR;
    }
    // Se cargan los datos para mostrarlas en el form.
    nId = auxPetR.getRelationId();
    petId = auxPetR.getPetId();
    clientId = auxPetR.getClientId();
    owner = auxPetR.getOwner();

    std::cout << "Editando Relaciones de la Mascota #" << nId << std::endl;
    //  configurar form

    // pedir y buscar si el id mascota ingresado existe
    petIdForm.setIntField("ID Mascota", petId, 4);
    do {
        // si no existe, preguntar si quiere reintentar
        if (!retryIfIdNotExists(alreadyExists)) return auxFormR;
        // si no completa el form, salir
        if (!petIdForm.fill()) return auxFormR;
        alreadyExists = petsManager.idExists(petId);
    } while (!alreadyExists);  // si no existe, volver a pedir

    // pedir y buscar si el id cliente ingresado existe
    alreadyExists = true;
    clientIdForm.setIntField("ID Cliente", clientId, 4);
    do {
        // si no existe, preguntar si quiere reintentar
        if (!retryIfIdNotExists(alreadyExists)) return auxFormR;
        // si no completa el form, salir
        if (!clientIdForm.fill()) return auxFormR;
        alreadyExists = clientsManager.idExists(clientId);
    } while (!alreadyExists);  // si no existe, volver a pedir

    petRelsForm.setBoolField("¿Es propietario [SI/NO]?", owner);

    // si no se completa, devolver Relacion de la Mascota vacia
    if (!petRelsForm.fill()) return auxFormR;

    // si se completa
    // Si se cambió de propietario, se actualiza el archivo de mascotas
    bool ownerChanged = (owner != auxPetR.getOwner()) ||
                        (owner && clientId != auxPetR.getClientId());
    if (ownerChanged) {
        bool successNewOwner = newOwner(petId, clientId);
        if (!successNewOwner) return auxFormR;  // devolver vació
        std::cout << "Se actualizará el dueño principal de la mascota.\n";
    }

    auxFormR.setRelationId(nId);
    auxFormR.setClientId(clientId);
    auxFormR.setPetId(petId);
    auxFormR.setOwner(owner);
    auxFormR.setStatus(true);

    return auxFormR;
}

void RelationsManager::edit() {
    InputForm search;
    int nId;
    show(false);

    int totalRegs = _petRelationsFile.getTotalRegisters();
    if (totalRegs <= 0) return;

    std::cout << "\nIngrese el ID de la relacion a modificar.\n";
    search.setIntField("ID Relacion", nId, 4);
    if (!search.fill()) return;  // si no se completa, salir

    int regPos = _petRelationsFile.searchReg(searchById, nId);
    if (regPos == -1) {
        std::cout << "No se encontraron resultados.\n";
        utils::pause();
        return;
    }
    // Si se encontro, pedir datos
    PetRelations auxPetR = editForm(regPos);
    // Si no se completo el formulario, salir
    if (auxPetR.getPetId() == -1) {
        std::cout << "No se realizara la edicion.\n";
        utils::pause();
        return;
    }

    // guardar relacion de mascota actualizada
    if (_petRelationsFile.updateFile(auxPetR, regPos)) {
        std::cout << "Relación de la Mascota editada con exito!\n";
    } else {
        std::cout << "Ocurrio un error al actualizar los registros.\n";
    }
    utils::pause();
}

void RelationsManager::show(bool showAndPause) {
    int totalRegs = _petRelationsFile.getTotalRegisters();
    // calcular el total de celdas de nuestra lista, segun la cantidad de datos
    // que contiene 1 registro
    int totalCells = totalRegs * _petRelationsFields;

    if (totalRegs < 0) {
        std::cout << "Ocurrio un error al leer los registros.\n";
        utils::pause();
        return;
    }
    if (totalRegs == 0) {
        utils::coutCenter("No hay registros para mostrar.\n");
        utils::pause();
        return;
    }
    // Se crea la variable que va a contener todas las celdas, segun la cantidad
    // de registros
    std::string *cells = new std::string[totalCells];
    if (cells == NULL) {
        std::cout << "No hay memoria suficiente para mostrar las relaciones.\n";
        return;
    }
    int rowPos = 0;  // acumula la posicion actual a asignar
    for (int i = 0; i < totalRegs; i++) {
        PetRelations auxPetR = _petRelationsFile.readFile(i);
        // Obtener todas las propiedades del cliente
        // Guardarlas en un vector de string
        std::string vecStr[6];
        auxPetR.toVecString(vecStr);
        // Agregar nombre del cliente y mascota
        vecStr[4] = ClientsManager().getFullNameById(auxPetR.getClientId());
        vecStr[5] = PetsManager().getNameById(auxPetR.getPetId());
        for (int cell = 0; cell < _petRelationsFields; cell++) {
            // solo llena las celdas si es un registro activo
            if (auxPetR.getStatus()) {
                cells[rowPos + cell] = vecStr[cell];
            } else {
                cells[rowPos + cell] = "";
            }
        }

        // se incrementa la posicion de la celda segun la cantidad de datos que
        // contiene el registro, que equivale a una fila de la lista
        rowPos += _petRelationsFields;
    }
    // Vector que contiene las columnas de nuestra lista
    std::string columns[6] = {"ID RELACION",    "ID MASCOTA",
                              "ID CLIENTE",     "DUEÑO?",
                              "NOMBRE CLIENTE", "NOMBRE MASCOTA"};

    ListView petsRelationsList;
    petsRelationsList.addCells(cells, totalCells);
    petsRelationsList.addCols(columns, 6);
    petsRelationsList.setTitle("RELACIONES DE MASCOTAS");
    petsRelationsList.show();
    delete[] cells;  // liberar memoria!

    if (showAndPause) utils::pause();
}

// Solo compara si coincide el id
bool RelationsManager::searchById(PetRelations reg, int nId) {
    if (reg.getRelationId() == nId) return true;
    return false;
}

bool RelationsManager::idExists(int nId) {
    return _petRelationsFile.searchReg(searchById, nId) >= 0 ? true : false;
}

// Verifica si existe relacion entre el id de la mascota y el id del cliente
bool RelationsManager::searchRelation(PetRelations reg, int petId,
                                      int clientId) {
    if (reg.getPetId() == petId && reg.getClientId() == clientId) return true;
    return false;
}

bool RelationsManager::relationExists(int petId, int clientId) {
    int found = _petRelationsFile.searchReg(searchRelation, petId, clientId);
    return (found >= 0) ? true : false;
}

// Buscar relacion por id de mascota y cliente y devolverla
PetRelations RelationsManager::searchGetRelation(int petId, int clientId) {
    PetRelations auxPetR;
    int found = _petRelationsFile.searchReg(searchRelation, petId, clientId);
    if (found >= 0) {
        auxPetR = _petRelationsFile.readFile(found);
    }
    return auxPetR;
}

bool RelationsManager::retryIfIdExists(bool exists) {
    if (exists) {
        std::cout << "El ID ingresado ya existe, presione cualquier tecla "
                     "para reintentar o ESC para salir.\n";
        if (rlutil::getkey() == rlutil::KEY_ESCAPE) return false;
        rlutil::cls();
    }
    return true;
}

bool RelationsManager::retryIfIdNotExists(bool exists) {
    if (!exists) {
        std::cout << "El ID ingresado NO EXISTE, presione cualquier tecla "
                     "para reintentar o ESC para salir.\n";
        if (rlutil::getkey() == rlutil::KEY_ESCAPE) return false;
        rlutil::cls();
    }
    return true;
}

bool RelationsManager::newOwner(int petId, int clientId) {
    InputForm confirmForm;
    PetsManager petsManager;
    bool confirm;
    confirmForm.setBoolField("¿Confirmar? [SI/NO]", confirm);
    std::cout
        << "ATENCIÓN: El cliente se definió como propietario, esta acción "
           "reemplazará el dueño actual de la mascota.\n";
    std::cout << "Si elige NO, se agregará la relación como 'familiar'.\n";
    if (!confirmForm.fill()) return false;
    if (!confirm) return false;
    if (!petsManager.updateOwner(clientId, petId)) {
        std::cout
            << "Ocurrió un error al actualizar el registro de mascotas.\n";
        std::cout << "No se actualizará el dueño de la mascota pero se "
                     "conservará la relación como 'familiar'.\n";
        return false;
    }
    return true;
}

bool RelationsManager::updateRelation(PetRelations petR, int regPos) {
    return _petRelationsFile.updateFile(petR, regPos);
}

// Esta funcion solo se utiliza desde petsManager
int RelationsManager::autogenerateNew(int clientId, int petId) {
    PetRelations auxPetR;
    auxPetR.setClientId(clientId);
    auxPetR.setPetId(petId);
    auxPetR.setOwner(true);
    auxPetR.setStatus(true);
    // Si no existe se crea uno nuevo con id 1
    if (!utils::fileExists(_filePath)) {
        auxPetR.setRelationId(1);
        bool success = _petRelationsFile.writeFile(auxPetR);
        return success ? 1 : -1;
    }
    // Si ya existe, agrega relacion con id+1, hasta encontrar disponible
    int totalRegs = _petRelationsFile.getTotalRegisters();
    if (totalRegs == -1) return -1;
    // obtener ultimo id asignado
    int lastId = _petRelationsFile.readFile(totalRegs - 1).getRelationId();
    int newId = lastId + 1;  // sumar 1
    // buscar si existe el id, si existe, sumar 1 y volver a buscar
    while (idExists(newId)) {
        newId++;
    }
    auxPetR.setRelationId(newId);
    // guardar nueva relación
    bool success = _petRelationsFile.writeFile(auxPetR);
    // Si tuvo éxito, devolver nro de Id, sino -1
    return success ? newId : -1;
}

void RelationsManager::deleteRel() {
    InputForm searchId, confirmForm;
    int nId;
    bool confirm;
    // mostrar relacion
    show(false);

    std::cout << "\nIngrese el ID de la Relación a dar de baja.\n";
    searchId.setIntField("ID Relación", nId, 4);
    if (!searchId.fill()) return;  // si no se completa, salir
    int regPos = _petRelationsFile.searchReg(searchById, nId);
    if (regPos == -1) {
        std::cout << "No existe Relación con el ID ingresado.\n";
        utils::pause();
        return;
    }
    PetRelations auxPetR = _petRelationsFile.readFile(regPos);
    if (auxPetR.getPetId() == -1) {
        std::cout << "Ocurrió un error al leer los registros.\n";
        utils::pause();
        return;
    }

    if (auxPetR.getOwner()) {
        std::cout << "El cliente de esta relación está definido como DUEÑO.\n"
                     "Por favor cargue/edite otra con el nuevo dueño antes de "
                     "dar de baja la actual.\n";
        utils::pause();
        return;
    }

    printf(
        "Se seleccionó la Relación #%d, confirma la baja definitiva? Esta "
        "acción no se puede deshacer.\n",
        nId);
    confirmForm.setBoolField("[SI/NO]", confirm);
    if (!confirmForm.fill()) return;
    if (!confirm) {
        std::cout << "No se realizará la baja.\n";
        utils::pause();
        return;
    }

    bool success = _petRelationsFile.deleteReg(regPos);

    if (success) {
        std::cout << "Baja realizada con éxito!\n";
    } else {
        std::cout << "Ocurrió un error al intentar realizar la baja.\n";
    }
    utils::pause();
}
