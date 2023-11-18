#include "ProductsManager.h"

#include <iostream>

#include "InputForm.h"
#include "ListView.h"
#include "ProductCategory.h"
#include "rlutil.h"

void ProductsManager::load() {
    InputForm idForm;
    Product auxProduct;
    int nId = 0;
    bool alreadyExists = false;

    // pedir y buscar si el id ingresado existe
    do {
        if (alreadyExists) {
            std::cout
                << "El ID del producto ya existe, presione cualquier tecla "
                   "para reintentar o ESC para salir.\n";
            if (rlutil::getkey() == rlutil::KEY_ESCAPE) return;
            rlutil::cls();
        }
        idForm.setIntField("ID Producto", nId, 4);
        // si no completa el form, salir
        if (!idForm.fill()) return;
        alreadyExists =
            _productsFile.searchReg(searchById, nId) >= 0 ? true : false;
        idForm.clearAll();  // limpiar form
    } while (alreadyExists);

    auxProduct = loadForm();
    // Si no se completo el form, salir
    if (auxProduct.getProductId() == 0) return;

    auxProduct.setProductId(nId);  // set del Id ingresado anteriormente
    if (_productsFile.writeFile(auxProduct)) {
        std::cout << "Producto guardado con exito!\n";
    } else {
        std::cout << "Ocurrio un error al guardar el producto.\n";
    }
}
/*   int _productID, _quantity, _stock;
    float _price;
    char _productName[30], _description[45], _brand[15], _productType[30];
    ProductCategory _productCategory;
*/
Product ProductsManager::loadForm() {
    InputForm productForm;
    Product auxProduct;
    std::string productName, description, brand, productType;
    float price;
    int quantity, stock;
    // TODO: Ver si está bien incluirlo asi
    ProductCategory productCategory;

    productForm.setStrField("Nombre", productName, 30);
    productForm.setStrField("Descripcion", description, 45);
    productForm.setStrField("Marca", brand, 15);
    productForm.setStrField("Tipo Producto", productType, 30);
    productForm.setIntField("Cantidad", quantity, 4);
    productForm.setIntField("Stock", stock, 4);
    // TODO: Ver como metemos float y product category

    ////productForm.setFFFFFFFFFFFFFF ("Precio", price, XXXXXXX)
    ////productForm. PRODUCT CATEGORY

    if (!productForm.fill()) return auxProduct;

    auxProduct.setProductName(productName);
    auxProduct.setDescription(description);
    auxProduct.setBrand(brand);
    auxProduct.setProductType(productType);
    auxProduct.setPrice(price);
    auxProduct.setQuantity(quantity);
    auxProduct.setStock(stock);
    auxProduct.setProductCategory(productCategory);
    return auxProduct;
}

// Solo compara si coincide el id
bool ProductsManager::searchById(Product reg, int nId) {
    if (reg.getProductId() == nId) return true;
    return false;
}

Product ProductsManager::editForm(int regPos) {
    InputForm productForm;
    Product auxProduct;
    std::string productName, description, brand, productType;
    float price;
    int quantity, stock, nId;
    ProductCategory productcategory;

    auxProduct = _productsFile.readFile(regPos);
    if (auxProduct.getProductId() == 0) {
        std::cout << "Ocurrio un error al leer los registros.\n";
        return auxProduct;
    }
    // Se cargan los datos para mostrarlas en el form.
    productName = auxProduct.getProductName();
    description = auxProduct.getDescription();
    brand = auxProduct.getBrand();
    nId = auxProduct.getProductId();
    productType = auxProduct.getProductType();
    /////price = auxProduct.getPrice();
    quantity = auxProduct.getQuantity();
    stock = auxProduct.getStock();
    productcategory = auxProduct.getProductCategory();

    std::cout << "Editando producto #" << nId << std::endl;
    // configurar form
    productForm.setEditMode(true);  // modo edicion
    productForm.setStrField("Nombre", productName, 30);
    productForm.setStrField("Descripcion", description, 45);
    productForm.setStrField("Marca", brand, 15);
    productForm.setStrField("Tipo Producto", productType, 30);
    productForm.setIntField("Cantidad", quantity, 4);
    productForm.setIntField("Stock", stock, 4);

    // TODO: Ver como metemos float y product category

    ////productForm.setFFFFFFFFFFFFFF ("Precio", price, XXXXXXX)
    ////productForm. PRODUCT CATEGORY

    // completar form
    bool success = productForm.fill();
    if (success) {  // si se completa
        auxProduct.setProductName(productName);
        auxProduct.setDescription(description);
        auxProduct.setBrand(brand);
        auxProduct.setProductType(productType);
        ///// auxProduct.setPrice(price);
        auxProduct.setQuantity(quantity);
        auxProduct.setStock(stock);
        /////auxProduct.setProductCategory(productCategory);

        return auxProduct;
    }
    // si no se completa, devolver Producto vacio
    return auxProduct;
}

void ProductsManager::edit() {
    InputForm search;
    int nId;
    show();
    std::cout << "\nIngrese el ID del producto a modificar.\n";
    search.setIntField("ID Producto", nId, 4);
    if (!search.fill()) return;  // si no se completa, salir
    int regPos = _productsFile.searchReg(searchById, nId);
    if (regPos == -1) {
        std::cout << "No se encontraron resultados.\n";
        system("pause");
        return;
    }
    // Si se encontro, pedir datos
    Product auxProduct = editForm(regPos);
    // Si no se completo el formulario, salir
    if (auxProduct.getProductId() == 0) {
        std::cout << "No se realizara la edicion.\n";
        system("pause");
        return;
    }

    // guardar cliente actualizado
    if (_productsFile.updateFile(auxProduct, regPos)) {
        std::cout << "Producto editado con exito!\n";
    } else {
        std::cout << "Ocurrio un error al guardar el registro.\n";
    }
    system("pause");
}

void ProductsManager::show() {
    int totalRegs = _productsFile.getTotalRegisters();
    // calcular el total de celdas de nuestra lista, segun la cantidad de datos
    // que contiene 1 registro
    int totalCells = totalRegs * _productFields;

    if (totalRegs < 0) {
        std::cout << "Ocurrio un error al leer los registros.\n";
        system("pause");  // TODO: usar rlutil ?
        return;
    }
    // Se crea la variable que va a contener todas las celdas, segun la cantidad
    // de registros
    std::string *cells = new std::string[totalCells];
    if (cells == NULL) {
        std::cout << "No hay memoria suficiente para mostrar los clientes.\n";
        return;
    }
    int cellPos = 0;  // acumula la posicion actual a asignar
    for (int i = 0; i < totalRegs; i++) {
        Product auxProduct = _productsFile.readFile(i);
        // Obtener todas las propiedades del cliente
        // Guardarlas en un vector de string
        std::string vecStr[9];
        auxProduct.toVecString(vecStr);
        for (int cell = 0; cell < _productFields; cell++) {
            cells[cellPos + cell] = vecStr[cell];
        }
        // se incrementa la posicion de la celda segun la cantidad de datos que
        // contiene el registro, que equivale a una fila de la lista
        cellPos += _productFields;
    }
    // Vector que contiene las columnas de nuestra lista
    std::string columns[9] = {
        "ID",       "Nombre", "Descripcion", "Marca",    "Tipo de producto",
        "Cantidad", "Precio", "Stock",       "Categoria"};

    ListView productsList;
    productsList.addCells(cells, totalCells);
    productsList.addCols(columns, 9);
    productsList.setTitle("PRODUCTOS");
    productsList.show();
    delete[] cells;  // liberar memoria!
}