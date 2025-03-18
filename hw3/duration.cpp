#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
using namespace std;

struct Bond {
    int maturity_year;
    int offer_year;
    double price;
    double yield;
    double coupon;
};

// Compute Macaulay Duration
double computeMD(const Bond& bond, double r) {
    double duration = 0.0;
    double discount, c;
    int years = static_cast<int>(bond.maturity_year) - static_cast<int>(bond.offer_year);
    
    for (int t = 1; t <= years; t++) {
        c = (t == years) ? (bond.coupon + 100) : bond.coupon;
        discount = pow(1 + r, t);
        duration += (t * c / discount);
    }
    return duration / bond.price;
}

// Compute Modified Duration
double computeModifiedDuration(double macaulayDuration, double yield) {
    return macaulayDuration / (1 + yield);
}

// Percentage price change for a 1bp yield change
double computePriceChange(double modifiedDuration) {
    return modifiedDuration * 0.0001; // 1 basis point = 0.0001
}

int main() {
    ifstream file("bond_data.csv");
    string line;
    Bond bond;
    
    // Read CSV file (skip header)
    if (file.is_open()) {
        getline(file, line);
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            getline(ss, token, ','); // Issuer (skipped)
            getline(ss, token, ','); bond.maturity_year = stoi(token);
            getline(ss, token, ','); bond.offer_year = stoi(token);
            getline(ss, token, ','); bond.price = stod(token);
            getline(ss, token, ','); bond.yield = stod(token);
            getline(ss, token, ','); bond.coupon = stod(token);

            // // Print data
            // cout << "Maturity_year: " << bond.maturity_year << "\n";
            // cout << "Offer_year: " << bond.offer_year << "\n";
            // cout << "Price: " << bond.price << "\n";
            // cout << "Yield: " << bond.yield << "\n";
            // cout << "Coupon: " << bond.coupon << "\n";
        }
        file.close();
    }
    
    double originalDuration = computeMD(bond, bond.yield);
    double zeroYieldDuration = computeMD(bond, 0.0);
    double upYieldDuration = computeMD(bond, bond.yield + 0.1);
    double downYieldDuration = computeMD(bond, bond.yield - 0.1);
    double modifiedDuration = computeModifiedDuration(originalDuration, bond.yield);
    double priceChange = computePriceChange(modifiedDuration);
    
    // Print results
    cout << "Original Macaulay Duration: " << originalDuration << " years\n";
    cout << "Duration at zero yield: " << zeroYieldDuration << " years\n";
    cout << "Duration at +10% yield: " << upYieldDuration << " years\n";
    cout << "Duration at -10% yield: " << downYieldDuration << " years\n";
    cout << "Modified Duration: " << modifiedDuration << " years\n";
    cout << "Price change per basis point move: " << priceChange * 100 << "%\n";
    
    return 0;
}
