#include "VaccinationManager.h"

#include <iostream>

#include "PetsManager.h"
#include "InputForm.h"
#include "ListView.h"
#include "rlutil.h"
#include "utils.h"

void VaccinationManager::load() {
    InputForm idForm;
    Vaccination auxVaccination;
    int nId = 0;
    bool alreadyExists = false;

    // pedir y buscar si el id ingresado existe
  idForm.setIntField("ID Vacunación", nId, 4);
    do {
        if (!retryIfIdExists(alreadyExists)) return; 
           // si no completa el form, salir
        if (!idForm.fill()) return; 
                  alreadyExists = idExists(nId); }
                  while (alreadyExists);
         // Si no existe el id de vac, pedir el resto de datos
    auxVaccination = loadForm();
    // Si no se completo el form, salir
    if (auxVaccination.getPeId() == -1) return;

 // setear id ingresado
    auxVaccination.setAplicationId(nId);  // set del Id ingresado anteriormente
    if (_vaccinationFile.writeFile(auxVaccination)) {
        std::cout << "Vacunacion guardada con exito!\n";
    } else {
        std::cout << "Ocurrio un error al guardar la vacunacion.\n";
    }
}

Vaccination VaccinationManager::loadForm() {
    InputForm vaccinationForm,  petIdForm,dateForm;
    PetsManager petsManager;
    Vaccination auxVaccination;
    std::string nameVaccine;
    Date dateAplication, dateRevaccination;
    int petId=0;
    bool alreadyExists=true;
    // bool notified;

// pedir y buscar si el id mascota ingresado existe
    petIdForm.setIntField("ID Mascota", petId, 4);
 do {
     // si no existe, preguntar si quiere reintentar
        if (!retryIfIdNotExists(alreadyExists)) return auxVaccination;
 // si no completa el form, salir
        if (!petIdForm.fill()) return auxVaccination;
         alreadyExists = petsManager.idExists(petId);
    } while (!alreadyExists);  // si no existe, volver a pedir


    vaccinationForm.setStrField(" Vacuna", nameVaccine, 15);

    dateForm.setDateField("Fecha de Aplicacion", dateAplication);
     bool validDate = true;
    do {
        if (!validDate) {
            std::cout << "La fecha debe ser  igual a la de hoy.\n";
        }
        if (!dateForm.fill()) return auxVaccination;
        validDate = validAplicDate(dateAplication);
    } while (!validDate);

    dateForm.setDateField("Fecha de revacunacion", dateRevaccination);
    bool validDate = true;
    do {
        if (!validDate) {
            std::cout << "La fecha debe ser un año posterior a la fecha de aplicación.\n";
        }
        if (!dateForm.fill()) return auxVaccination;
        validDate = validVaccRevaccDate( dateAplication, dateRevaccination);
    } while (!validDate);

    // vaccinationForm.setBoolField("Notificado", notified);

    if (!vaccinationForm.fill()) return auxVaccination;

    auxVaccination.setNameVaccine(nameVaccine);
    auxVaccination.setPetId(petId);
    auxVaccination.setDateAplication(dateAplication);
    auxVaccination.setDateRevaccination(dateRevaccination);
    // auxVaccination.setNotified(notified);

    return auxVaccination;
}

Vaccination VaccinationManager::editForm(int regPos) {
    InputForm vaccinationForm, dateForm;
    Vaccination auxVaccination;
    std::string nameVaccine;
    Date dateAplication, dateRevaccination;
    int petId, nId;
    bool notified;

    auxVaccination = _vaccinationFile.readFile(regPos);
    if (auxVaccination.getAplicationId() == -1) {
        std::cout << "Ocurrio un error al leer los registros.\n";
        return auxVaccination;
    }
    // Se cargan los datos para mostrarlas en el form.
    nameVaccine = auxVaccination.getNameVaccine();
    nId = auxVaccination.getAplicationId();
    dateAplication = auxVaccination.getDateAplication();
    dateRevaccination = auxVaccination.getDateRevaccination();
    notified = auxVaccination.getNottified();
    petId = auxVaccination.getPeId();

    std::cout << "Editando vacunacion#" << nId << std::endl;
    // configurar form
    vaccinationForm.setEditMode(true);  // modo edicion

    vaccinationForm.setStrField(" Vacuna", nameVaccine, 15);
    vaccinationForm.setIntField("ID Mascota", petId, 4);
    dateForm.setDateField("Fecha de aplicacion", dateAplication);

  bool validDate = true;
    do {
        if (!validDate) {
            std::cout << "La fecha debe ser  igual a la de hoy.\n";
        }
        if (!dateForm.fill()) return auxVaccination;
        validDate = validAplicDate(dateAplication);
    } while (!validDate);


    dateForm.setDateField("Fecha de revacunacion", dateRevaccination);
    bool validDate = true;
    do {
        if (!validDate) {
            std::cout << "La fecha debe ser un año posterior a la fecha de aplicación.\n";
        }
        if (!dateForm.fill()) return auxVaccination;
        validDate = validVaccRevaccDate(dateAplication,  dateRevaccination );

    } while (!validDate);
    vaccinationForm.setBoolField("Notificado", notified);

    // completar form
    bool success = vaccinationForm.fill();
    if (success) {  // si se completa
        auxVaccination.setAplicationId(nId);
        auxVaccination.setNameVaccine(nameVaccine);
        auxVaccination.setPetId(petId);
        auxVaccination.setDateAplication(dateAplication);
        auxVaccination.setDateRevaccination(dateRevaccination);
        auxVaccination.setNotified(notified);

        return auxVaccination;
    }
    // si no se completa, devolver Vaccination vacio
    return auxVaccination;
}

void VaccinationManager::edit() {
    InputForm search;
    int nId;
    show();
    std::cout << "\nIngrese el ID del vacunaciona modificar.\n";
    search.setIntField("ID Vacunacion", nId, 4);
    if (!search.fill()) return;  // si no se completa, salir
    int regPos = _vaccinationFile.searchReg(searchById, nId);
    if (regPos == -1) {
        std::cout << "No se encontraron resultados.\n";
        utils::pause();
        return;
    }
    // Si se encontro, pedir datos
    Vaccination auxVaccination = editForm(regPos);
    // Si no se completo el formulario, salir
    if (auxVaccination.getAplicationId() == 0) {
        std::cout << "No se realizara la edicion.\n";
        utils::pause();
        return;
    }

    // guardar vacunacion actualizado
    if (_vaccinationFile.updateFile(auxVaccination, regPos)) {
        std::cout << "Vacunacion editada con exito!\n";
    } else {
        std::cout << "Ocurrio un error al guardar el registro.\n";
    }
    utils::pause();
}

void VaccinationManager::show() {
    int totalRegs = _vaccinationFile.getTotalRegisters();
    // calcular el total de celdas de nuestra lista, segun la cantidad de datos
    // que contiene 1 registro
    int totalCells = totalRegs * _vaccinationFields;

    if (totalRegs < 0) {
        std::cout << "Ocurrio un error al leer los registros.\n";
        utils::pause();
        return;
    }
    // Se crea la variable que va a contener todas las celdas, segun la cantidad
    // de registros
    std::string *cells = new std::string[totalCells];
    if (cells == NULL) {
        std::cout << "No hay memoria suficiente para mostrar las aplicaciones "
                     "realizadas.\n";
        return;
    }
    int cellPos = 0;  // acumula la posicion actual a asignar
    for (int i = 0; i < totalRegs; i++) {
        Vaccination auxVaccination = _vaccinationFile.readFile(i);
        // Obtener todas las propiedades del cliente
        // Guardarlas en un vector de string
        std::string vecStr[6];
        auxVaccination.toVecString(vecStr);
        for (int cell = 0; cell < _vaccinationFields; cell++) {
            cells[cellPos + cell] = vecStr[cell];
        }
        // se incrementa la posicion de la celda segun la cantidad de datos que
        // contiene el registro, que equivale a una fila de la lista
        cellPos += _vaccinationFields;
    }
    // Vector que contiene las columnas de nuestra lista
    std::string columns[6] = {"ID Aplicacion", "Id Mascota ",  "Vacuna",
                              "Aplicacion",    "Revacunacion", "¿Notificado?"};

    ListView VaccinationList;
    VaccinationList.addCells(cells, totalCells);
    VaccinationList.addCols(columns, 6);
    VaccinationList.setTitle("Vacunaciones");
    VaccinationList.show();

    delete[] cells;  // liberar memoria!
}
// Solo compara si coincide el id
bool VaccinationManager::searchById(Vaccination reg, int nId) {
    if (reg.getAplicationId() == nId) return true;
    return false;
}

bool VaccinationManager::idExists(int nId) {
    return _vaccinationFile.searchReg(searchById, nId) >= 0 ? true : false;
}


bool VaccinationManager::retryIfIdExists(bool exists) {
    if (exists) {
        std::cout << "El ID ingresado ya existe, presione cualquier tecla "
                     "para reintentar o ESC para salir.\n";
        if (rlutil::getkey() == rlutil::KEY_ESCAPE) return false;
        rlutil::cls();
    }
    return true;
}

bool VaccinationManager::retryIfIdNotExists(bool exists) {
    if (!exists) {
        std::cout << "El ID ingresado NO EXISTE, presione cualquier tecla "
                     "para reintentar o ESC para salir.\n";
        if (rlutil::getkey() == rlutil::KEY_ESCAPE) return false;
        rlutil::cls();
    }
    return true;
}


bool VaccinationManager::validVaccRevaccDate (Date dateA, Date dateR) {
int Year=dateA.getYear();
Year++;
   if (dateA.getDay()== dateR.getDay() && dateA.getMonth()== 
   dateR.getMonth() && dateR.getYear() == Year)
 { return true;}
  else { return false;  }
}

bool VaccinationManager::validAplicDate(Date date) {
    Date today;
    if (date == today) return true;
    return false;
}