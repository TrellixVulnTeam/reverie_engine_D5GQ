#pragma once 

/** @file GUnits.h 
    Defines a class representing a set of units
*/

#include <array>
#include <type_traits>

namespace rev {

enum class AngularUnits {
    kRadians = 0,
    kDegrees,
    COUNT
};

enum class DistanceUnits {
    kMeters=0,
    kKilometers,
    kCentimeters,
    kInches,
    kFeet,
    kMiles,
    COUNT
};

enum class TimeUnits {
    kSeconds = 0,
    kMinutes,
    kHours,
    kDays,
    kWeeks,
    kJulianYears,
    kMilliseconds,
    kMicroseconds,
    COUNT
};

enum class MassUnits {
    kKilograms = 0,
    kPounds,
    COUNT
};

/// @class Units
/// @brief Class containing helper methods unit conversions
class Units {
public:
    /// @name Static Methods
    /// @{

    template<auto From, auto To, typename T>
    static double Convert(T v) {
        return (double)Convert_impl<decltype(From), From, To>((double)v);
    }

    template<auto From, typename T>
    static double ConvertToSI(T v) {
        return (double)ConvertToSI_impl<decltype(From), From>((double)v);
    }

    template<auto To, typename T>
    static double ConvertFromSI(T v) {
        return (double)ConvertFromSI_impl<decltype(To), To>((double)v);
    }

    /// @}

protected:

    /// @name Methods
    /// @{

    template<typename EnumType, EnumType From, EnumType To, typename T>
    static T Convert_impl(T v) {
        if constexpr (To == From) {
            // No conversion required
            return v;
        }
        else if constexpr (To == EnumType(0)) {
            // Converting to SI units
            return ConvertToSI_impl<EnumType, From>(v);
        }
        //else if constexpr (From == EnumType(0)) {
        //    // Converting from SI units
        //    return ConvertFromSI_impl<EnumType, To>(v);
        //}
        else {
            // Convert to SI, and then from SI to the final output
            /// @note There are precision issues with compile-time multiplication if the desired output is an integer
            constexpr auto combinedFactor = ToSIFactor<EnumType, From>() * FromSIFactor<EnumType, To>();
            return T(v * combinedFactor);
            //double p1 = ConvertToSI_impl<EnumType, From>(v);
            //double p2 = FromSIFactor<EnumType, To>();
            //return  p1 * p2;
        }
    }
    

    template<typename EnumType, EnumType From, typename T>
    static T ConvertToSI_impl(T v) {
        return (T)(v * ToSIFactor<EnumType, From>());
    }

    template<typename EnumType, EnumType From>
    static constexpr double ToSIFactor() {
        if constexpr (From == EnumType(0)) {
            // Converting from SI type to SI
            return 1.0;
        }
        else {
            if constexpr (std::is_same_v<EnumType, AngularUnits>) {
                constexpr double factor = s_angularConversionsToSI[(size_t)From];
                return factor;
            }
            else if constexpr (std::is_same_v<EnumType, DistanceUnits>) {
                constexpr double factor = s_distanceConversionsToSI[(size_t)From];
                return factor;
            }
            else if constexpr (std::is_same_v<EnumType, TimeUnits>) {
                constexpr double factor = s_timeConversionsToSI[(size_t)From];
                return factor;
            }
            else if constexpr (std::is_same_v<EnumType, MassUnits>) {
                constexpr double factor = s_massConversionsToSI[(size_t)From];
                return factor;
            }
            else {
                Logger::Throw("Unrecognized enum type");
                return 0.0;
            }
        }
    }

    template<typename EnumType, EnumType To, typename T>
    static T ConvertFromSI_impl(T v) {
        return (T)(v * FromSIFactor<EnumType, To>());
    }

    template<typename EnumType, EnumType To>
    static constexpr double FromSIFactor() {
        if constexpr (To == EnumType(0)) {
            // Converting from SI type to SI
            return 1.0;
        }
        else {
            if constexpr (std::is_same_v<EnumType, AngularUnits>) {
                constexpr double factor = 1.0 / s_angularConversionsToSI[(size_t)To];
                return factor;
            }
            else if constexpr (std::is_same_v<EnumType, DistanceUnits>) {
                constexpr double factor = 1.0 / s_distanceConversionsToSI[(size_t)To];
                return factor;
            }
            else if constexpr (std::is_same_v<EnumType, TimeUnits>) {
                constexpr double factor = 1.0 / s_timeConversionsToSI[(size_t)To];
                return factor;
            }
            else if constexpr (std::is_same_v<EnumType, MassUnits>) {
                constexpr double factor = 1.0 / s_massConversionsToSI[(size_t)To];
                return factor;
            }
            else {
                Logger::Throw("Unrecognized enum type");
                return 0.0;
            }
        }
    }

    template<typename T>
    static constexpr size_t round(double x, double tolerance = 1e-8) {
        static_assert(std::is_integral_v<T>, "T must be an integral type");
        return T(x + tolerance);
    }

    /// @}

    /// @name Members
    /// @{

    static constexpr std::array<double, (size_t)AngularUnits::COUNT> s_angularConversionsToSI
    {
        1.0, // Radians to radians
        3.141592653589793238462643383279 /180.0 // Degrees to radians
    };

    static constexpr std::array<double, (size_t)DistanceUnits::COUNT> s_distanceConversionsToSI
    {
        1.0, // Meters to meters
        1000, // Kilometers to meters
        0.01, // Centimeters to meters
        0.0254, // Inches to meters
        0.3048, // Feet to meters
        1609.344 // Miles to meters
    };

    static constexpr std::array<double, (size_t)TimeUnits::COUNT> s_timeConversionsToSI
    {
        1.0, // Seconds to seconds
        60.0, // Minutes to seconds
        3600.0, // Hours to seconds
        24.0 * 3600.0, // Days to seconds
        7.0 * 24.0 * 3600.0, // Weeks to seconds
        365.25 * 24.0 * 3600.0, // Julian years to seconds
        1e-3, // Millseconds to seconds
        1e-6 // Microseconds to seconds
    };

    static constexpr std::array<double, (size_t)MassUnits::COUNT> s_massConversionsToSI
    {
        1.0, // kg to kg
        0.45359237 // pounds to kg
    };

    /// @}

};

/// @brief Define aliases for commonly used conversion functions
constexpr auto RadToDeg = Units::Convert<AngularUnits::kRadians, AngularUnits::kDegrees, double>;
constexpr auto MicroSecToSec = Units::Convert<TimeUnits::kMilliseconds, TimeUnits::kSeconds, double>;

} /// end rev namespacing
