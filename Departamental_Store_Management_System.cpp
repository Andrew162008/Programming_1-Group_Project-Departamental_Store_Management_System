#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

//  CONSTANTS (values that never change; used as array sizes)
const int MAX_PRODUCTS = 100;
const int MAX_CUSTOMERS = 50;
const int MAX_SALES = 100;
const int MAX_CART = 20;
const int NUM_DEPTS = 5;

const double TAX_RATE = 0.07;   // 7% tax
const double BULK_DISCOUNT = 0.05;   // 5% off when buying 10 or more of one item
const int    BULK_AMOUNT = 10;

// An ARRAY of text: department number 0 is "Grocery", 1 is "Electronics" etc
string departmentNames[NUM_DEPTS] = {
    "Grocery", "Electronics", "Clothing", "Household", "Beauty"
};

//  PART 1: Helper Functions

void printLine() {
    cout << "------------------------------------------------------------\n";
}

void printTitle(string title) {
    cout << "\n============================================================\n";
    cout << "  " << title << "\n";
    cout << "============================================================\n";
}

// Asks for a whole number and keeps asking until it is between low and high.
int getInt(string message, int low, int high) {
    int number;

    while (true) {
        cout << message;
        cin >> number;
        if (cin.fail()) {                // the user typed something that is not a number
            cin.clear();                 // reset the error
            cin.ignore(1000, '\n');      // throw away the bad input
            cout << "  Please enter a whole number.\n";
        }
        else {
            cin.ignore(1000, '\n');      // remove the ENTER key from the input
            if (number >= low && number <= high) {
                return number;
            }
            cout << "  Please enter a number from " << low << " to " << high << ".\n";
        }
    }
}

// Same idea, but for numbers with decimals (like prices).
double getDouble(string message, double low, double high) {
    double number;

    while (true) {
        cout << message;
        cin >> number;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "  Please enter a number.\n";
        }
        else {
            cin.ignore(1000, '\n');
            if (number >= low && number <= high) {
                return number;
            }
            cout << "  Please enter a value from " << low << " to " << high << ".\n";
        }
    }
}

// Asks for a line of text and makes sure it is not empty.
string getText(string message) {
    string text;

    do {
        cout << message;
        getline(cin, text);
        if (text == "") {
            cout << "  This cannot be empty.\n";
        }
    } while (text == "");               // do-while: runs at least once

    return text;
}

void waitForEnter() {
    cout << "\nPress ENTER to continue...";
    string dummy;
    getline(cin, dummy);
}

// Rounds a money value to 2 decimal places.
double roundMoney(double amount) {
    int cents = (int)(amount * 100 + 0.5);
    return cents / 100.0;
}

//  PART 2: Classes

//  CLASS: Product
class Product {
private:                    // private = only usable inside this class
    int    id;
    string name;
    int    department;      // a number from 0 to 4 (see departmentNames)
    double price;
    int    quantity;        // how many are in stock
    int    reorderLevel;    // warn when stock falls to this number or below

public:
    // Constructor with no values (needed to create an ARRAY of products)
    Product() {
        id = 0;
        name = "";
        department = 0;
        price = 0.0;
        quantity = 0;
        reorderLevel = 10;
    }

    // Constructor with values
    Product(int newId, string newName, int newDept, double newPrice,
        int newQty, int newReorder) {
        id = newId;
        name = newName;
        department = newDept;
        price = newPrice;
        quantity = newQty;
        reorderLevel = newReorder;
    }

    // "Getters": let other code READ the private values
    int    getId() { return id; }
    string getName() { return name; }
    double getPrice() { return price; }
    int    getQuantity() { return quantity; }

    void addStock(int amount) {
        if (amount > 0) {
            quantity = quantity + amount;
        }
    }

    // Takes items out of stock. Returns true if it worked.
    bool removeStock(int amount) {
        if (amount > 0 && amount <= quantity) {
            quantity = quantity - amount;
            return true;
        }
        return false;
    }

    bool isLowStock() {
        return quantity <= reorderLevel;
    }

    // Shows one row of the product table.
    void display() {
        cout << left << setw(7) << id
            << setw(24) << name
            << setw(13) << departmentNames[department]
            << right << "$" << setw(7) << fixed << setprecision(2) << price
            << setw(6) << quantity;
        if (isLowStock()) {
            cout << "  <-- LOW";
        }
        cout << "\n";
    }
};

//  CLASS: Customer
class Customer {
private:
    int    id;
    string name;
    string phone;
    int    points;          // loyalty points (1 point per dollar spent)
    double totalSpent;

public:
    Customer() {
        id = 0;
        name = "";
        phone = "";
        points = 0;
        totalSpent = 0.0;
    }

    Customer(int newId, string newName, string newPhone) {
        id = newId;
        name = newName;
        phone = newPhone;
        points = 0;
        totalSpent = 0.0;
    }

    int    getId() { return id; }
    string getName() { return name; }
    int    getPoints() { return points; }

    void recordPurchase(double amount) {
        totalSpent = totalSpent + amount;
        points = points + (int)amount;
    }

    void display() {
        cout << left << setw(6) << id
            << setw(22) << name
            << setw(14) << phone
            << right << setw(7) << points
            << "  $" << setw(8) << fixed << setprecision(2) << totalSpent << "\n";
    }
};

//  CLASS: Store
//  Holds the ARRAYS and all the functions that run the store.
class Store {
private:
    // ARRAYS of objects
    Product  products[MAX_PRODUCTS];
    Customer customers[MAX_CUSTOMERS];

    // ARRAYS of numbers (one slot per sale)
    int    saleReceipt[MAX_SALES];
    int    saleUnits[MAX_SALES];
    double saleTotal[MAX_SALES];

    // counters: how many slots of each array are used
    int productCount;
    int customerCount;
    int saleCount;

    // next numbers to hand out
    int nextProductId;
    int nextCustomerId;
    int nextReceipt;

public:
    //  Constructor: sets up the store with some starting data
    Store() {
        productCount = 0;
        customerCount = 0;
        saleCount = 0;
        nextProductId = 101;
        nextCustomerId = 1;
        nextReceipt = 1001;
        loadSampleData();
    }

    // Adds a product to the array. Returns true if it worked.
    bool addProductToArray(string name, int dept, double price, int qty, int reorder) {
        if (productCount >= MAX_PRODUCTS) {
            return false;                       // the array is full
        }
        products[productCount] = Product(nextProductId, name, dept, price, qty, reorder);
        productCount++;
        nextProductId++;
        return true;
    }

    // Puts some products and customers in the store to start with
    void loadSampleData() {
        // name, department, price, quantity, warn-level
        addProductToArray("Milk", 0, 1.99, 60, 20);
        addProductToArray("Rice", 0, 12.50, 25, 10);
        addProductToArray("Orange Juice", 0, 3.79, 8, 12);
        addProductToArray("Wireless Mouse", 1, 18.99, 30, 8);
        addProductToArray("Bluetooth Speaker", 1, 44.00, 12, 5);
        addProductToArray("LED Desk Lamp", 1, 22.50, 4, 6);
        addProductToArray("T-Shirt", 2, 14.99, 50, 15);
        addProductToArray("Jacket", 2, 89.00, 9, 5);
        addProductToArray("Dish Soap", 3, 3.25, 80, 25);
        addProductToArray("Towel Set", 3, 19.99, 18, 8);
        addProductToArray("Shampoo", 4, 6.49, 45, 15);
        addProductToArray("Face Cream", 4, 11.99, 7, 10);
        customers[0] = Customer(1, "John", "555-0101");
        customers[1] = Customer(2, "Tom", "555-0102");
        customers[2] = Customer(3, "Adam", "555-0103");
        customerCount = 3;
        nextCustomerId = 4;
        customers[0].recordPurchase(620.00);
        customers[1].recordPurchase(150.00);
        customers[2].recordPurchase(1100.00);
    }

    //  POINTER FUNCTIONS
    //  These return a POINTER to the object inside the array, or
    //  nullptr ("points to nothing") if it is not found.
    Product* findProduct(int id) {
        for (int i = 0; i < productCount; i++) {
            if (products[i].getId() == id) {
                return &products[i];            // & means "address of"
            }
        }
        return nullptr;
    }

    Customer* findCustomer(int id) {
        for (int i = 0; i < customerCount; i++) {
            if (customers[i].getId() == id) {
                return &customers[i];
            }
        }
        return nullptr;
    }

    // Swaps two whole products using pointers (used by the sort).
    // *a means "the object that a points to".
    void swapProducts(Product* a, Product* b) {
        Product temp = *a;
        *a = *b;
        *b = temp;
    }

    // VIEWING PRODUCTS

    void showTableHeader() {
        cout << left << setw(7) << "ID"
            << setw(24) << "Name"
            << setw(13) << "Department"
            << right << setw(8) << "Price"
            << setw(6) << "Qty" << "\n";
        printLine();
    }

    void viewAllProducts() {
        printTitle("ALL PRODUCTS");
        showTableHeader();
        for (int i = 0; i < productCount; i++) {
            products[i].display();
        }
        printLine();
        cout << productCount << " products in total.\n";
    }

    //  BUBBLE SORT: compares neighbours and swaps them if they are in
    //  the wrong order. byPrice = true sorts by price, false sorts by ID.
    void sortProducts(bool byPrice) {
        for (int pass = 0; pass < productCount - 1; pass++) {
            for (int i = 0; i < productCount - 1 - pass; i++) {
                bool wrongOrder;
                if (byPrice) {
                    wrongOrder = products[i].getPrice() > products[i + 1].getPrice();
                }
                else {
                    wrongOrder = products[i].getId() > products[i + 1].getId();
                }
                if (wrongOrder) {
                    swapProducts(&products[i], &products[i + 1]);
                }
            }
        }
    }

    void viewSortedByPrice() {
        printTitle("PRODUCTS SORTED BY PRICE");
        sortProducts(true);
        showTableHeader();
        for (int i = 0; i < productCount; i++) {
            products[i].display();
        }
        printLine();
        sortProducts(false);                    // put the list back in ID order
    }

    // MANAGING PRODUCTS

    void addNewProduct() {
        printTitle("ADD NEW PRODUCT");
        if (productCount >= MAX_PRODUCTS) {
            cout << "Sorry, the store cannot hold any more products.\n";
            return;
        }
        string name = getText("Product name: ");
        for (int i = 0; i < NUM_DEPTS; i++) {
            cout << "  " << (i + 1) << ". " << departmentNames[i] << "\n";
        }
        int dept = getInt("Department: ", 1, NUM_DEPTS) - 1;
        double price = getDouble("Price ($): ", 0.01, 10000);
        int qty = getInt("Starting stock: ", 0, 10000);
        int reorder = getInt("Warn me when stock reaches: ", 0, 1000);
        addProductToArray(name, dept, roundMoney(price), qty, reorder);
        cout << "\n\"" << name << "\" was added with ID " << (nextProductId - 1) << ".\n";
    }

    void restockProduct() {
        printTitle("RESTOCK A PRODUCT");
        int id = getInt("Product ID: ", 1, 99999);
        Product* p = findProduct(id);           // p is a POINTER to a product
        if (p == nullptr) {                     // nothing found
            cout << "No product has that ID.\n";
            return;
        }

        // p->getName() is the same as (*p).getName()
        cout << p->getName() << " currently has " << p->getQuantity() << " in stock.\n";
        int amount = getInt("How many units to add? ", 1, 10000);
        p->addStock(amount);                    // changes the REAL product, not a copy
        cout << "New stock level: " << p->getQuantity() << "\n";
    }

    void lowStockReport() {
        printTitle("LOW STOCK REPORT");
        showTableHeader();
        int count = 0;
        for (int i = 0; i < productCount; i++) {
            if (!products[i].isLowStock()) {
                continue;                       // skip products that are fine
            }
            products[i].display();
            count++;
        }
        printLine();
        cout << count << " product(s) need restocking.\n";
    }

    // CUSTOMERS

    void registerCustomer() {
        printTitle("REGISTER A CUSTOMER");
        if (customerCount >= MAX_CUSTOMERS) {
            cout << "The customer list is full.\n";
            return;
        }
        string name = getText("Customer name: ");
        string phone = getText("Phone number : ");
        customers[customerCount] = Customer(nextCustomerId, name, phone);
        cout << "\nWelcome, " << name << "! Your customer ID is " << nextCustomerId << ".\n";
        customerCount++;
        nextCustomerId++;
    }

    void viewCustomers() {
        printTitle("CUSTOMER LIST");
        cout << left << setw(6) << "ID"
            << setw(22) << "Name"
            << setw(14) << "Phone"
            << right << setw(7) << "Points"
            << setw(11) << "Spent" << "\n";
        printLine();
        for (int i = 0; i < customerCount; i++) {
            customers[i].display();
        }
        printLine();
        cout << customerCount << " customer(s).\n";
    }

    // SALES REPORT

    void salesReport() {
        printTitle("SALES REPORT");
        if (saleCount == 0) {
            cout << "No sales have been made yet.\n";
            return;
        }
        cout << left << setw(10) << "Receipt"
            << right << setw(8) << "Items"
            << setw(14) << "Total" << "\n";
        printLine();
        double grandTotal = 0.0;
        int totalUnits = 0;
        for (int i = 0; i < saleCount; i++) {
            cout << left << setw(10) << saleReceipt[i]
                << right << setw(8) << saleUnits[i]
                << "  $" << setw(10) << fixed << setprecision(2) << saleTotal[i] << "\n";
            grandTotal = grandTotal + saleTotal[i];
            totalUnits = totalUnits + saleUnits[i];
        }
        printLine();
        cout << "Number of sales : " << saleCount << "\n";
        cout << "Items sold      : " << totalUnits << "\n";
        cout << "Total income    : $" << grandTotal << "\n";
        cout << "Average sale    : $" << grandTotal / saleCount << "\n";
    }

    // SELLING (CHECKOUT)

    // Shows the cart. The cart is made of TWO parallel arrays:
    // cartId[i] is the product, cartQty[i] is how many of it.
    void showCart(int cartId[], int cartQty[], int cartCount) {
        cout << "\n..........................................................\n";
        cout << "  YOUR CART\n";
        cout << "..........................................................\n";
        if (cartCount == 0) {
            cout << "  (empty)\n";
            return;
        }
        double subtotal = 0.0;
        for (int i = 0; i < cartCount; i++) {
            Product* p = findProduct(cartId[i]);
            double lineTotal = p->getPrice() * cartQty[i];
            cout << "  " << (i + 1) << ". " << left << setw(22) << p->getName()
                << right << setw(4) << cartQty[i] << " x $"
                << fixed << setprecision(2) << setw(6) << p->getPrice()
                << " = $" << setw(7) << lineTotal << "\n";
            subtotal = subtotal + lineTotal;
        }
        cout << "..........................................................\n";
        cout << "  Subtotal: $" << subtotal << "\n";
    }

    void makeSale() {
        printTitle("NEW SALE");

        // Step 1: Who is the customer? 
        Customer* customer = nullptr;           // starts as "no customer"
        int customerId = getInt("Customer ID (0 for a guest): ", 0, 99999);
        if (customerId != 0) {
            customer = findCustomer(customerId);
            if (customer == nullptr) {
                cout << "Customer not found. Continuing as a guest.\n";
            }
            else {
                cout << "Hello, " << customer->getName() << "!\n";
            }
        }

        // Step 2: Fill the cart
        int cartId[MAX_CART];                   // product IDs in the cart
        int cartQty[MAX_CART];                  // quantities in the cart
        int cartCount = 0;                      // how many cart slots are used
        bool shopping = true;
        while (shopping) {
            showCart(cartId, cartQty, cartCount);
            cout << "\n  1. Add an item\n";
            cout << "  2. Go to checkout\n";
            cout << "  3. Cancel this sale\n";
            int choice = getInt("Choose: ", 1, 3);
            switch (choice) {
            case 1: {                       // ----- ADD AN ITEM -----
                int id = getInt("Product ID: ", 1, 99999);
                Product* p = findProduct(id);
                if (p == nullptr) {
                    cout << "  That product does not exist.\n";
                    break;
                }
                cout << "  " << p->getName() << " - $" << fixed << setprecision(2)
                    << p->getPrice() << " (" << p->getQuantity() << " in stock)\n";
                int qty = getInt("  How many? ", 1, 1000);

                // Is this product already in the cart? (-1 means no)
                int existing = -1;
                int alreadyInCart = 0;
                for (int i = 0; i < cartCount; i++) {
                    if (cartId[i] == id) {
                        existing = i;
                        alreadyInCart = cartQty[i];
                        break;
                    }
                }
                if (alreadyInCart + qty > p->getQuantity()) {
                    cout << "  Not enough stock.\n";
                }
                else if (existing != -1) {
                    cartQty[existing] = cartQty[existing] + qty;
                    cout << "  Cart updated.\n";
                }
                else if (cartCount >= MAX_CART) {
                    cout << "  The cart is full.\n";
                }
                else {
                    cartId[cartCount] = id;
                    cartQty[cartCount] = qty;
                    cartCount++;
                    cout << "  Added to the cart.\n";
                }
                break;
            }
            case 2:                         // CHECKOUT
                if (cartCount == 0) {
                    cout << "  You cannot check out with an empty cart.\n";
                }
                else {
                    shopping = false;       // ends the while loop
                }
                break;
            case 3:                         // CANCEL
                cout << "Sale cancelled.\n";
                return;                     // leave the function
            }
        }

        // Step 3: Work out the bill
        double subtotal = 0.0;
        double discount = 0.0;
        int totalUnits = 0;
        for (int i = 0; i < cartCount; i++) {
            Product* p = findProduct(cartId[i]);
            double lineTotal = p->getPrice() * cartQty[i];
            subtotal = subtotal + lineTotal;
            totalUnits = totalUnits + cartQty[i];
            if (cartQty[i] >= BULK_AMOUNT) {
                discount = discount + lineTotal * BULK_DISCOUNT;
            }
        }
        double tax = roundMoney((subtotal - discount) * TAX_RATE);
        double total = roundMoney(subtotal - discount + tax);

        // Step 4: Payment
        cout << "\nAmount to pay: $" << fixed << setprecision(2) << total << "\n";
        cout << "  1. Cash\n  2. Card\n";
        int method = getInt("Payment method: ", 1, 2);
        string methodName;
        double change = 0.0;
        if (method == 1) {
            methodName = "Cash";
            double given;
            do {
                given = getDouble("Cash given: $", 0, 1000000);
                if (given < total) {
                    cout << "  That is not enough.\n";
                }
            } while (given < total);
            change = roundMoney(given - total);
        }
        else {
            methodName = "Card";
            cout << "Card approved.\n";
        }

        // Step 5: Update stock and records
        for (int i = 0; i < cartCount; i++) {
            Product* p = findProduct(cartId[i]);
            p->removeStock(cartQty[i]);
        }
        if (customer != nullptr) {
            customer->recordPurchase(total);
        }

        // Save the sale in the arrays (only if there is still room)
        int receiptNumber = nextReceipt;
        nextReceipt++;
        if (saleCount < MAX_SALES) {
            saleReceipt[saleCount] = receiptNumber;
            saleUnits[saleCount] = totalUnits;
            saleTotal[saleCount] = total;
            saleCount++;
        }

        // Step 6: Print the receipt
        cout << "\n**************************************************\n";
        cout << "            DEPARTMENTAL STORE RECEIPT\n";
        cout << "**************************************************\n";
        cout << "Receipt number: " << receiptNumber << "\n";
        if (customer != nullptr) {
            cout << "Customer      : " << customer->getName() << "\n";
        }
        else {
            cout << "Customer      : Guest\n";
        }
        cout << "--------------------------------------------------\n";
        for (int i = 0; i < cartCount; i++) {
            Product* p = findProduct(cartId[i]);
            cout << left << setw(22) << p->getName()
                << right << setw(4) << cartQty[i] << " x "
                << setw(6) << p->getPrice()
                << setw(10) << p->getPrice() * cartQty[i] << "\n";
        }
        cout << "--------------------------------------------------\n";
        cout << right << setw(36) << "Subtotal: " << setw(10) << subtotal << "\n";
        if (discount > 0) {
            cout << setw(36) << "Bulk discount: " << setw(10) << -discount << "\n";
        }
        cout << setw(36) << "Tax (7%): " << setw(10) << tax << "\n";
        cout << setw(36) << "TOTAL: " << setw(10) << total << "\n";
        cout << setw(36) << "Paid by " + methodName + ": " << setw(10) << total + change << "\n";
        if (change > 0) {
            cout << setw(36) << "Change: " << setw(10) << change << "\n";
        }
        if (customer != nullptr) {
            cout << "\nLoyalty points balance: " << customer->getPoints() << "\n";
        }
        cout << "**************************************************\n";
        cout << "        Thank you for shopping with us!\n";
    }
};

//  PART 3: Main Function

int main() {
    Store store;                    // create one Store object
    int choice;

    printTitle("WELCOME TO THE DEPARTMENTAL STORE SYSTEM");

    // The main menu loop: keeps showing the menu until the user picks 0
    do {
        printTitle("MAIN MENU");
        cout << "  1.  View all products\n";
        cout << "  2.  View products sorted by price\n";
        cout << "  3.  New sale (checkout)\n";
        cout << "  4.  Register a customer\n";
        cout << "  5.  View customers\n";
        cout << "  6.  Add a new product\n";
        cout << "  7.  Restock a product\n";
        cout << "  8.  Low stock report\n";
        cout << "  9.  Sales report\n";
        cout << "  0.  Exit\n";
        printLine();
        choice = getInt("Enter your choice: ", 0, 9);

        // A switch chooses what to do based on the value of "choice"
        switch (choice) {
        case 1: store.viewAllProducts();   break;
        case 2: store.viewSortedByPrice(); break;
        case 3: store.makeSale();          break;
        case 4: store.registerCustomer();  break;
        case 5: store.viewCustomers();     break;
        case 6: store.addNewProduct();     break;
        case 7: store.restockProduct();    break;
        case 8: store.lowStockReport();    break;
        case 9: store.salesReport();       break;
        case 0:
            cout << "\nThank you for using the system. Goodbye!\n";
            break;
        }
        if (choice != 0) {
            waitForEnter();
        }

    } while (choice != 0);

    return 0;
}
