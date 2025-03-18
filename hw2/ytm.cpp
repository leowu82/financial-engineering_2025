#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

// Split string by delimiter
vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream ss(s);
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Calc days between two dates (30/360)
int daysBetween(const vector<string>& date1, const vector<string>& date2) {
    int y1 = stoi(date1[0]), m1 = stoi(date1[1]), d1 = stoi(date1[2]);
    int y2 = stoi(date2[0]), m2 = stoi(date2[1]), d2 = stoi(date2[2]);

    if (d1 > 30) d1 = 30;
    if (d2 > 30) d2 = 30;
    
    return 360 * (y2 - y1) + 30 * (m2 - m1) + (d2 - d1);
}

// Calc periods between two dates
int getPeriods(const vector<string>& maturityDate, const vector<string>& deliveryDate) {
    return daysBetween(deliveryDate, maturityDate) / 180;  // 180 days = 6 months
}

// Calc bond price given a YTM estimate
double bondPrice(double ytm, double faceValue, double coupon_rate, double price, int periods) {
    double coupon = (coupon_rate / 2) * faceValue;  // Semiannual coupon payment
    double pv = 0;
    
    for (int t = 1; t <= periods; t++) {
        pv += coupon / pow(1 + ytm / 2, t);
    }
    pv += faceValue / pow(1 + ytm / 2, periods);
    
    return pv - price;  // Return difference from actual price
}

// Newton's method to calc YTM
double calcYTM(double faceValue, double coupon_rate, double price, int periods, 
               double tol = 1e-6, int max_iter = 100) {
    double ytm = 0.05;  // Initial guess (5%)
    for (int i = 0; i < max_iter; i++) {
        double f = bondPrice(ytm, faceValue, coupon_rate, price, periods);
        double df = (bondPrice(ytm + 1e-5, faceValue, coupon_rate, price, periods) - f) / 1e-5;  // Approximate derivative
        
        ytm -= f / df;  // Newton's step
        if (fabs(f) < tol) return ytm * 100;  // Convert to percentage
    }
    return -1;  // Return -1 if no convergence
}

// Accrued interest
double accruedInterest(const vector<string>& deliveryDate, const vector<string>& maturityDate, 
                       double coupon_rate, double faceValue) {
    int daysToNextCoupon = 180 - (daysBetween(deliveryDate, maturityDate) % 180);
    double w = static_cast<double>(daysToNextCoupon) / 180;
    double semiannualCoupon = coupon_rate * faceValue / 2;
    
    return semiannualCoupon * (1 - w);
}

int main() {
    double faceValue = 100;
    vector<double> prices;
    vector<double> couponRates;
    vector<double> offeringYields;
    vector<vector<string> > maturityDates;
    vector<vector<string> > settlementDates;
    
    ifstream file("bond_data.csv");
    if (!file) {
        cerr << "Error opening file!" << '\n';
        return 1;
    }
    
    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        stringstream ss(line);
        string issuer_id, maturity_date, offering_date, delivery_date;
        double offering_yield, coupon_rate, price;
        
        getline(ss, issuer_id, ',');
        getline(ss, maturity_date, ',');
        getline(ss, offering_date, ',');
        ss >> price; ss.ignore();
        ss >> offering_yield; ss.ignore();
        getline(ss, delivery_date, ',');
        ss >> coupon_rate;
        
        // Parse date
        vector<string> parsed_maturity_date = split(maturity_date, '/');
        vector<string> parsed_settlement_date = split(delivery_date, '/');

        // // Print data
        // cout << issuer_id << '\t';
        // cout << parsed_maturity_date[0] << '/' << parsed_maturity_date[1] << '/' << parsed_maturity_date[2] << '\t';
        // cout << offering_date << '\t';
        // cout << price << '\t';
        // cout << offering_yield << '\t';
        // cout << parsed_settlement_date[0] << '/' << parsed_settlement_date[1] << '/' << parsed_settlement_date[2] << '\t';
        // cout << coupon_rate << '\n';
        
        // Store data
        prices.push_back(price);
        couponRates.push_back(coupon_rate);
        offeringYields.push_back(offering_yield);
        maturityDates.push_back(parsed_maturity_date);
        settlementDates.push_back(parsed_settlement_date);
    }
    file.close();
    
    cout << "===============================================\n";
    for (size_t i = 0; i < couponRates.size(); i++) {
        // Calculate YTM
        int periods = getPeriods(maturityDates[i], settlementDates[i]);
        double ytm = calcYTM(faceValue, couponRates[i] / 100, prices[i], periods);
        cout << "Bond NO.\tOFFERING_YIELD\tCALCULATED_YTM\n";
        cout << "Bond " << i + 1 << ": \t" << offeringYields[i] << "\t\t" << ytm << '\n';
        
        // Calculate Clean/Dirty prices
        double accrued = accruedInterest(settlementDates[i], maturityDates[i], couponRates[i] / 100, faceValue);
        double cleanPrice = prices[i];
        double dirtyPrice = prices[i] + accrued;
        cout << "Clean Price: " << cleanPrice << "\n";
        cout << "Dirty Price: " << dirtyPrice << '\n';
        cout << "-----------------------------------------------\n";
    }
    
    return 0;
}
