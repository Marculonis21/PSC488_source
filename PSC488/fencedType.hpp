#ifndef FENCEDTYPE_HPP
#define FENCEDTYPE_HPP

#include <sstream>

/*
 * PLEASE PLEASE PRETTY PLEASE,
 * This struct is created in very specific way so that there cannot be a
 * confusion between values which should be understood as Voltage or as Current.
 * There is a reason why it is very hard (but still possible) to extract the
 * value itself! You should have a very specific reason for getting out the
 * value (UI update, printing out, etc) - these reasons should be covered by the
 * std::string operator implemented below. PLEASE, don't CHANGE this struct to
 * allow for direct value extraction.
 *
 * Thank you, Marek B.
 */

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
    }

    bool operator<(const FencedValue &other) const { return value < other.value; }
    bool operator<=(const FencedValue &other) const { return value <= other.value; }
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
