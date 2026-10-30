#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <stdexcept>
#include <limits>
using namespace std;

// Abstract class
class Advisory {
public:
    virtual void giveAdvice() const = 0;
    virtual ~Advisory() {}
};

// Base class
class Crop {
protected:
    string name;
    double area;
    string season;
    double yieldPerAcre;

    static int cropCount;

public:

    // Default constructor
    Crop() {
        name = "Unknown";
        area = 1;
        season = "Unknown";
        yieldPerAcre = 0;
        cropCount++;
    }

    // Parameterized constructor
    Crop(string n, double a, string s, double y) {
        if (a <= 0 || y < 0)
            throw invalid_argument("Invalid area or yield!");

        name = n;
        area = a;
        season = s;
        yieldPerAcre = y;
        cropCount++;
    }

    // Virtual function for runtime polymorphism
    virtual double calculateYield() const {
        return area * yieldPerAcre;
    }

    virtual void display() const {
        cout << "\nCrop Name: " << name;
        cout << "\nArea: " << area << " acres";
        cout << "\nSeason: " << season;
        cout << "\nYield per Acre: " << yieldPerAcre;
        cout << "\nTotal Yield: " << calculateYield() << endl;
    }

    // Operator overloading
    bool operator>(const Crop& c) const {
        return calculateYield() > c.calculateYield();
    }

    // Static member function
    static int getCropCount() {
        return cropCount;
    }

    // Getters
    string getName() const { return name; }
    double getArea() const { return area; }
    string getSeason() const { return season; }
    double getYieldPerAcre() const { return yieldPerAcre; }

    virtual ~Crop() {}
};

int Crop::cropCount = 0;


// Derived class 1
class CerealCrop : public Crop, public Advisory {
private:
    string fertilizerType;

public:

    // Constructor overloading
    CerealCrop(string n, double a, string s,
               double y, string f)
        : Crop(n, a, s, y) {
        fertilizerType = f;
    }

    CerealCrop(string n, double a, double y)
        : Crop(n, a, "Winter", y) {
        fertilizerType = "Organic";
    }

    void display() const override {
        cout << "\n--- Cereal Crop Details ---";
        Crop::display();
        cout << "Fertilizer Type: "
             << fertilizerType << endl;
    }

    void giveAdvice() const override {
        cout << "Advice: Use balanced fertilizers "
             << "and maintain proper soil moisture for "
             << name << "." << endl;
    }

    string getExtra() const {
        return fertilizerType;
    }
};


// Derived class 2
class VegetableCrop : public Crop, public Advisory {
private:
    int irrigationFrequency;

public:

    // Constructor
    VegetableCrop(string n, double a, string s,
                  double y, int irrigation)
        : Crop(n, a, s, y) {

        if (irrigation <= 0)
            throw invalid_argument("Invalid irrigation frequency!");

        irrigationFrequency = irrigation;
    }

    VegetableCrop(string n, double a, double y)
        : Crop(n, a, "Summer", y) {
        irrigationFrequency = 3;
    }

    void display() const override {
        cout << "\n--- Vegetable Crop Details ---";
        Crop::display();
        cout << "Irrigation Frequency: "
             << irrigationFrequency
             << " times per week" << endl;
    }

    void giveAdvice() const override {
        cout << "Advice: Ensure regular irrigation "
             << "and monitor pests for "
             << name << "." << endl;
    }

    int getExtra() const {
        return irrigationFrequency;
    }
};


// Function to save crop data
void saveData(const vector<unique_ptr<Crop>>& crops) {

    ofstream file("farmdata.txt");

    if (!file) {
        cout << "Error opening file!" << endl;
        return;
    }

    for (const auto& c : crops) {

        if (dynamic_cast<CerealCrop*>(c.get())) {
            CerealCrop* p =
                dynamic_cast<CerealCrop*>(c.get());

            file << "C " << quoted(p->getName()) << " "
                 << p->getArea() << " "
                 << quoted(p->getSeason()) << " "
                 << p->getYieldPerAcre() << " "
                 << quoted(p->getExtra()) << endl;
        }

        else if (dynamic_cast<VegetableCrop*>(c.get())) {
            VegetableCrop* p =
                dynamic_cast<VegetableCrop*>(c.get());

            file << "V " << quoted(p->getName()) << " "
                 << p->getArea() << " "
                 << quoted(p->getSeason()) << " "
                 << p->getYieldPerAcre() << " "
                 << p->getExtra() << endl;
        }
    }

    file.close();
    cout << "\nCrop data saved successfully!" << endl;
}


// Function to retrieve crop data
void loadData(vector<unique_ptr<Crop>>& crops) {

    ifstream file("farmdata.txt");

    if (!file) {
        cout << "No saved data found!" << endl;
        return;
    }

    char type;
    string n, s, extra;
    double a, y;
    int irrigation;

    int loaded = 0;

    try {
        while (file >> type >> quoted(n) >> a
                    >> quoted(s) >> y) {

            if (type == 'C' &&
                file >> quoted(extra)) {

                crops.push_back(make_unique<CerealCrop>(
                    n, a, s, y, extra));

                loaded++;
            }

            else if (type == 'V' &&
                     file >> irrigation) {

                crops.push_back(make_unique<VegetableCrop>(
                    n, a, s, y, irrigation));

                loaded++;
            }
        }
    }
    catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    file.close();

    cout << loaded << " crop records loaded." << endl;
}


// Main function
int main() {

    vector<unique_ptr<Crop>> crops;
    int choice;

    do {

        cout << "\n===== SMART FARM MANAGEMENT SYSTEM =====";
        cout << "\n1. Add Cereal Crop";
        cout << "\n2. Add Vegetable Crop";
        cout << "\n3. Display All Crops";
        cout << "\n4. Calculate Total Yield";
        cout << "\n5. Compare Two Crops";
        cout << "\n6. Get Farming Advice";
        cout << "\n7. Save Crop Data";
        cout << "\n8. Load Crop Data";
        cout << "\n9. Display Crop Count";
        cout << "\n0. Exit";

        cout << "\nEnter your choice: ";
        cin >> choice;

        try {

            if (choice == 1 || choice == 2) {

                string n, s, f;
                double a, y;
                int irrigation;

                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                cout << "Enter crop name: ";
                getline(cin, n);

                cout << "Enter area in acres: ";
                if (!(cin >> a))
                    throw invalid_argument("Invalid area input!");

                cout << "Enter season: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, s);

                cout << "Enter yield per acre: ";
                if (!(cin >> y))
                    throw invalid_argument("Invalid yield input!");

                if (choice == 1) {

                    cout << "Enter fertilizer type: ";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    getline(cin, f);

                    crops.push_back(make_unique<CerealCrop>(
                        n, a, s, y, f));

                }

                else {

                    cout << "Enter irrigation frequency per week: ";

                    if (!(cin >> irrigation))
                        throw invalid_argument("Invalid irrigation input!");

                    crops.push_back(make_unique<VegetableCrop>(
                        n, a, s, y, irrigation));
                }

                cout << "\nCrop added successfully!";
            }

            else if (choice == 3) {

                if (crops.empty()) {
                    cout << "\nNo crop records available.";
                }

                for (const auto& c : crops)
                    c->display();
            }

            else if (choice == 4) {

                double total = 0;

                for (const auto& c : crops)
                    total += c->calculateYield();

                cout << "\nTotal Farm Yield = "
                     << total << " units" << endl;
            }

            else if (choice == 5) {

                if (crops.size() < 2) {
                    cout << "Add at least two crops first.";
                }

                else {

                    int i, j;

                    cout << "Enter first crop index (0 to "
                         << crops.size()-1 << "): ";
                    cin >> i;

                    cout << "Enter second crop index: ";
                    cin >> j;

                    if (i < 0 || j < 0 ||
                        i >= (int)crops.size() ||
                        j >= (int)crops.size() || i == j) {

                        throw invalid_argument("Invalid crop index!");
                    }

                    if (*crops[i] > *crops[j])
                        cout << crops[i]->getName()
                             << " has a higher yield.";

                    else if (*crops[j] > *crops[i])
                        cout << crops[j]->getName()
                             << " has a higher yield.";

                    else
                        cout << "Both crops have equal yield.";
                }
            }

            else if (choice == 6) {

                for (const auto& c : crops) {

                    Advisory* advice =
                        dynamic_cast<Advisory*>(c.get());

                    if (advice != nullptr)
                        advice->giveAdvice();
                }
            }

            else if (choice == 7) {
                saveData(crops);
            }

            else if (choice == 8) {
                loadData(crops);
            }

            else if (choice == 9) {

                cout << "\nTotal Crops Recorded: "
                     << Crop::getCropCount();
            }

            else if (choice != 0) {
                cout << "\nInvalid choice!";
            }
        }

        catch (const exception& e) {

            cout << "\nException: " << e.what() << endl;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }

    } while (choice != 0);

    cout << "\nThank you for using Smart Farm Management System!";

    return 0;
}