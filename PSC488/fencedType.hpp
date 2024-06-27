#ifndef FENCEDTYPE_HPP
#define FENCEDTYPE_HPP

#include <sstream>

enum class FencedType { VOLTAGE, CURRENT, };

template <FencedType X> 
struct FencedValue {

    FencedValue() : value(0) {}
    explicit FencedValue(double value) : value(value) {}

    // get rid of those weird value copy assignments
    template <typename T> 
    FencedValue &operator=(const T &other) = delete;
    FencedValue &operator=(const FencedValue &other) {
        this->value = other.value;
        return *this;
    };

    bool operator<(const FencedValue &other) const { return value < other.value; }
    bool operator==(const FencedValue &other) const { return value == other.value; }
    bool operator!=(const FencedValue &other) const { return value != other.value; }

    FencedValue operator-(const FencedValue &other) const { return FencedValue(value - other.value); }
    FencedValue operator+(const FencedValue &other) const { return FencedValue(value + other.value); }
    FencedValue operator-(double num) const { return FencedValue(value - num); }
    FencedValue operator+(double num) const { return FencedValue(value + num); }
    FencedValue operator*(double num) const { return FencedValue(value * num); }

    // conversion operator
    operator std::string() const { 
      std::ostringstream out;
      out.precision(3);
      out << std::fixed << value;
      return out.str();
    }

  private:
    double value;
};

using Voltage = FencedValue<FencedType::VOLTAGE>;
using Current = FencedValue<FencedType::CURRENT>;

#endif 
