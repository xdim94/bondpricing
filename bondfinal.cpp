#include <iostream>
#include <cmath>
#include <iomanip>

class Bond {
public:
    double face_value;
    double coupon_rate;
    double market_price;
    int remaining_years; // Remaining years to maturity
    int payment_frequency;
    double required_yield;

    Bond(double face_value, double coupon_rate, double market_price, int remaining_years, int payment_frequency, double required_yield = -1.0)
        : face_value(face_value), coupon_rate(coupon_rate), market_price(market_price), remaining_years(remaining_years), payment_frequency(payment_frequency), required_yield(required_yield) {}

    double calculate_coupon() const {
        return face_value * coupon_rate / payment_frequency;
    }

    double calculate_present_value(double rate) const {
        double pv = 0.0;
        double coupon = calculate_coupon();
        int periods = remaining_years * payment_frequency;

        for (int t = 1; t <= periods; ++t) {
            pv += coupon / pow(1 + rate / payment_frequency, t);
        }
        pv += face_value / pow(1 + rate / payment_frequency, periods);
        return pv;
    }

    double calculate_ytm(double tol = 1e-6, int max_iter = 1000) const {
        double low = 0.0, high = 1.0;
        double mid = 0.0;

        for (int i = 0; i < max_iter; ++i) {
            mid = (low + high) / 2.0;
            double pv = calculate_present_value(mid);
            double diff = market_price - pv;

            if (fabs(diff) < tol) {
                return mid;
            }

            if (pv < market_price) {
                high = mid;
            } else {
                low = mid;
            }
        }

        return mid;
    }

    double calculate_macaulay_duration() const {
        double duration = 0.0;
        double coupon = calculate_coupon();
        int periods = remaining_years * payment_frequency;

        for (int t = 1; t <= periods; ++t) {
            duration += t * coupon / pow(1 + required_yield / payment_frequency, t);
        }
        duration += periods * face_value / pow(1 + required_yield / payment_frequency, periods);
        return duration / market_price;
    }

    double calculate_modified_duration() const {
        double macaulay_duration = calculate_macaulay_duration();
        return macaulay_duration / (1 + required_yield / payment_frequency);
    }

    double calculate_convexity() const {
        double convexity = 0.0;
        double coupon = calculate_coupon();
        int periods = remaining_years * payment_frequency;

        for (int t = 1; t <= periods; ++t) {
            convexity += t * (t + 1) * coupon / pow(1 + required_yield / payment_frequency, t + 2);
        }
        convexity += periods * (periods + 1) * face_value / pow(1 + required_yield / payment_frequency, periods + 2);
        return convexity / market_price;
    }

    double calculate_current_yield() const {
        return calculate_coupon() / market_price;
    }

    void display_price_sensitivity() const {
        double yield_change = 0.005; // ±0.5%
        std::cout << "Price Sensitivity Analysis:\n";
        for (double i = -2 * yield_change; i <= 2 * yield_change; i += yield_change) {
            double new_yield = required_yield + i;
            double new_price = calculate_present_value(new_yield);
            std::cout << "Yield: " << std::fixed << std::setprecision(4) << new_yield << " | Price: " << new_price << std::endl;
        }
    }

    double calculate_break_even_yield(double reference_price) const {
        double low = 0.0, high = 1.0, mid;
        while (high - low > 1e-6) {
            mid = (low + high) / 2.0;
            double pv = calculate_present_value(mid);
            if (pv < reference_price) low = mid;
            else high = mid;
        }
        return mid;
    }

    void display_scenario_analysis() const {
        double scenarios[] = {-0.02, -0.01, 0.0, 0.01, 0.02}; // ±2%, ±1%
        std::cout << "Scenario Analysis:\n";
        for (double delta : scenarios) {
            double new_yield = required_yield + delta;
            std::cout << "Yield: " << std::fixed << std::setprecision(4) << new_yield << std::endl;
            std::cout << "Price: " << calculate_present_value(new_yield) << std::endl;
            std::cout << "Macaulay Duration: " << calculate_macaulay_duration() << std::endl;
            std::cout << "Modified Duration: " << calculate_modified_duration() << std::endl;
            std::cout << "Convexity: " << calculate_convexity() << std::endl;
            std::cout << std::endl;
        }
    }

    void display_frequency_analysis() const {
        int frequencies[] = {1, 2, 4}; // Annual, Semi-Annual, Quarterly
        std::cout << "Frequency Analysis:\n";
        for (int freq : frequencies) {
            Bond temp_bond = *this;
            temp_bond.payment_frequency = freq;
            std::cout << "Payment Frequency: " << (freq == 1 ? "Annual" : freq == 2 ? "Semi-Annual" : "Quarterly") << std::endl;
            std::cout << "Price: " << temp_bond.calculate_present_value(required_yield) << std::endl;
            std::cout << "Macaulay Duration: " << temp_bond.calculate_macaulay_duration() << std::endl;
            std::cout << "Modified Duration: " << temp_bond.calculate_modified_duration() << std::endl;
            std::cout << "Convexity: " << temp_bond.calculate_convexity() << std::endl;
            std::cout << std::endl;
        }
    }

    void display_amortization_schedule() const {
        std::cout << "Amortization Schedule:\n";
        double coupon = calculate_coupon();
        int periods = remaining_years * payment_frequency;
        for (int t = 1; t <= periods; ++t) {
            double payment_time = static_cast<double>(t) / payment_frequency;
            double payment = (t == periods) ? (coupon + face_value) : coupon;
            std::cout << "Period: " << t << " | Payment Time: " << payment_time << " | Payment: " << payment << std::endl;
        }
    }

    void calculate_required_yield() {
        if (required_yield == -1.0) {
            std::cout << "Calculating required yield (YTM) based on the market price..." << std::endl;
            required_yield = calculate_ytm();
        }
    }
};

int main() {
    double face_value;
    double coupon_rate;
    double market_price;
    int remaining_years;
    int payment_frequency;
    double required_yield;

    // Prompt user for bond parameters
    std::cout << "Enter Face Value (e.g. 1000): ";
    std::cin >> face_value;

    std::cout << "Enter Coupon Rate (e.g. 0.05 for 5%): ";
    std::cin >> coupon_rate;

    std::cout << "Enter Market Price (e.g. 950): ";
    std::cin >> market_price;

    std::cout << "Enter Remaining Maturity in Years (e.g. 8): ";
    std::cin >> remaining_years;

    std::cout << "Enter Payment Frequency (1 for annual, 2 for semi-annual): ";
    std::cin >> payment_frequency;

    std::cout << "Enter Required Yield (e.g. 0.06 for 6%, or -1 if you want it to be calculated based on price): ";
    std::cin >> required_yield;

    // Create Bond object
    Bond bond(face_value, coupon_rate, market_price, remaining_years, payment_frequency, required_yield);

    // If required yield not provided, calculate it based on the market price
    if (required_yield == -1.0) {
        bond.calculate_required_yield();
    }

    // Calculate and display bond metrics
    std::cout << "\nBond Analysis:\n";
    std::cout << "Present Value (Price): " << bond.calculate_present_value(bond.required_yield) << std::endl;
    std::cout << "Yield to Maturity (YTM): " << bond.calculate_ytm() << std::endl;
    std::cout << "Macaulay Duration: " << bond.calculate_macaulay_duration() << std::endl;
    std::cout << "Modified Duration: " << bond.calculate_modified_duration() << std::endl;
    std::cout << "Convexity: " << bond.calculate_convexity() << std::endl;
    std::cout << "Current Yield: " << bond.calculate_current_yield() << std::endl;

    bond.display_price_sensitivity();
    std::cout << "Break-Even Yield: " << bond.calculate_break_even_yield(market_price) << std::endl;
    bond.display_scenario_analysis();
    bond.display_frequency_analysis();
    bond.display_amortization_schedule();

    system("pause");
    return 0;
}
