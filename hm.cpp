#include "hm.hpp"
#include <iostream>
#include <cmath>
LongNumber::LongNumber()
{
    precision = 0;
    sign = false;
    array = std::vector<bool>();
    point_position = 0;
}
LongNumber::LongNumber(int value, int precision) : precision(precision), sign(value < 0)
{
    array.clear();
    value = std::abs(value);
    point_position = 32;
    for (int i = 0; i < 32; i++)
    {
        array.push_back((value & 1 << (32 - 1 - i)) != 0);
    }
    make_norm();
}
LongNumber::LongNumber(const LongNumber &other)
{
    precision = other.precision;
    sign = other.sign;
    array = other.array;
    point_position = other.point_position;
}
LongNumber &LongNumber::operator=(const LongNumber &other)
{
    if (&other != this)
    {
        precision = other.precision;
        sign = other.sign;
        array = other.array;
        point_position = other.point_position;
    }
    return *this;
}
LongNumber::LongNumber(long double number, int _precision) : precision(_precision)
{
    sign = (number < 0);
    point_position = 1;
    array.push_back(0);

    if (number == 0)
    {
        return;
    }

    number = std::abs(number);
    int exponent;
    long double mantissa = std::frexp(number, &exponent);
    while (mantissa != 0 && array.size() < precision)
    {
        mantissa *= 2;
        int bit = static_cast<int>(mantissa);
        array.push_back(bit);
        mantissa -= bit;
    }

    if (exponent != 0)
    {
        *this = (exponent < 0) ? this->operator>>(-exponent) : this->operator<<(exponent);
    }
    make_norm();
}

LongNumber::LongNumber(double number, int _precision) : precision(_precision)
{
    sign = (number < 0);
    point_position = 1;
    array.push_back(0);

    if (number == 0)
    {
        return;
    }

    number = std::abs(number);
    int exponent;
    double mantissa = std::frexp(number, &exponent);
    while (mantissa != 0 && array.size() < precision)
    {
        mantissa *= 2;
        int bit = static_cast<int>(mantissa);
        array.push_back(bit);
        mantissa -= bit;
    }

    if (exponent != 0)
    {
        *this = (exponent < 0) ? this->operator>>(-exponent) : this->operator<<(exponent);
    }
    make_norm();
}

void LongNumber::setPrecision(int new_precision)
{
    precision = new_precision;
    make_norm();
}
LongNumber operator""_longnum(long double number)
{
    return LongNumber(number, 1000);
}

LongNumber::~LongNumber() {}

bool LongNumber::operator==(const LongNumber &other) const
{
    return sign == other.sign && precision == other.precision && array == other.array && point_position == other.point_position;
}

bool LongNumber::operator!=(const LongNumber &other) const
{
    return !(*this == other);
}

LongNumber LongNumber::operator>>(int shift) const
{
    LongNumber result(*this);
    result.array.insert(result.array.begin(), shift, false);
    result.make_norm();
    return result;
}
LongNumber LongNumber::operator<<(int shift) const
{
    LongNumber result(*this);
    result.point_position += shift;
    result.array.insert(result.array.end(), shift, false);
    result.make_norm();
    return result;
}

bool LongNumber::operator<(const LongNumber &other) const
{
    if (sign != other.sign)
    {
        return sign;
    }
    if (point_position != other.point_position)
    {
        return (point_position < other.point_position);
    }
    unsigned int size = std::max(array.size(), other.array.size()); // 10000 < 1.11111
    for (int i = 0; i < size; i++)
    {
        bool arr1_bit = (i < array.size() ? array[i] : 0);
        bool arr2_bit = (i < other.array.size() ? other.array[i] : 0);
        if (arr1_bit != arr2_bit)
        {
            return arr1_bit < arr2_bit;
        }
    }
    return false;
}

bool LongNumber::operator>=(const LongNumber &other) const
{
    return !(*this < other);
}

bool LongNumber::operator<=(const LongNumber &other) const
{
    return (*this < other) || (*this == other);
}

bool LongNumber::operator>(const LongNumber &other) const
{
    return !(*this <= other);
}

LongNumber LongNumber::abs() const
{
    LongNumber rez = *this;
    rez.sign = false;
    return rez;
}

LongNumber LongNumber::plus_bit(const LongNumber &number1, const LongNumber &number2) const
{
    LongNumber first(number1), second(number2);
    align(first, second);
    std::vector<bool> a = first.array, b = second.array;
    unsigned int size = std::max(a.size(), b.size());
    std::vector<bool> rez(size + 1, 0);
    bool carry = 0;
    for (int i = size - 1; i >= 0; i--)
    {
        bool arr1_bit = a[i];
        bool arr2_bit = b[i];
        bool sum = arr1_bit ^ arr2_bit ^ carry;
        carry = (arr1_bit + arr2_bit + carry) >> 1;
        rez[i + 1] = sum;
    }
    rez[0] = carry;
    LongNumber ans(first);
    ans.point_position = first.point_position + 1;
    ans.array = rez;
    ans.make_norm();
    return ans;
}

LongNumber LongNumber::minus_bit(const LongNumber &number1, const LongNumber &number2) const
{
    LongNumber first(number1), second(number2);
    align(first, second);
    std::vector<bool> a = first.array, b = second.array;
    unsigned int size = std::max(a.size(), b.size());
    std::vector<bool> rez(size, 0);
    bool borrow = false;

    for (int i = (int)a.size() - 1; i >= 0; --i)
    {
        bool bit1 = a[i];
        bool bit2 = (i < b.size()) ? b[i] : 0;

        rez[i] = (bit1 ^ bit2 ^ borrow);

        if ((bit1 == false && bit2 == true) || (bit1 == false && borrow == true) || (bit2 == true && borrow == true))
            borrow = true;
        else
            borrow = false;
    }

    LongNumber ans(number1);
    ans.array = rez;
    ans.make_norm();

    return ans;
}

LongNumber LongNumber::operator+(const LongNumber &other) const
{
    if (!sign && !other.sign)
    {
        return plus_bit(*this, other);
    }
    if (sign && other.sign)
    {
        LongNumber ans = plus_bit(*this, other);
        ans.sign = !ans.sign;
        return ans;
    }

    if (!sign && other.sign)
    {
        if (*this > negative(other))
        {
            return minus_bit(*this, other);
        }
        else
        {
            return negative(minus_bit(other, *this));
        }
    }
    else
    {
        if (negative(*this) < other)
        {
            return minus_bit(other, *this);
        }
        else
        {
            return minus_bit(*this, other);
        }
    }
}

LongNumber LongNumber::negative(const LongNumber &number) const
{
    LongNumber ans(number);
    ans.sign = !ans.sign;
    return ans;
}
LongNumber LongNumber::operator-(const LongNumber &other) const
{
    LongNumber a = *this;
    LongNumber b = other;
    align(a, b);

    if (!sign && !other.sign)
    {
        if (*this > other)
        {
            return minus_bit(*this, other);
        }
        else
        {
            return negative(minus_bit(other, *this));
        }
    }

    if (sign && sign == other.sign)
    {
        return *this + negative(other);
    }

    if (!sign && other.sign)
    {
        return plus_bit(*this, other);
    }

    if (sign && !other.sign)
    {
        return negative(plus_bit(*this, other));
    }

    return *this;
}

std::vector<bool> getArray(const LongNumber &ln)
{
    return ln.array;
}

// LongNumber LongNumber ::operator*(const LongNumber &other) const
// {
//     LongNumber rez;
//     rez.precision = std::max(precision, other.precision);
//     rez.array = multiplication(array, other.array);
//     rez.sign = sign ^ other.sign;
//     rez.make_norm();
//     return rez;
// }

LongNumber LongNumber::operator*(const LongNumber &other) const
{
    int mx_prec = std::max(precision, other.precision);

    LongNumber res(0, mx_prec);
    LongNumber first(*this), second(other);

    align(first, second);
    
    first.sign = second.sign = false;

    for (int i = 0; i < (int)second.array.size(); ++i)
    {
        if (second.array[i] == false)
            continue;

        int shift = (second.point_position - i - 1);

        if (shift >= 0)
        {
            res = res + (first << shift);
        }
        else
        {
            shift = -shift;
            res = res + (first >> shift);
        }
    }

    res.sign = (sign != other.sign);
    res.make_norm();

    return res;
}
// std::vector<bool> LongNumber::multiplication(const std::vector<bool> &a, const std::vector<bool> &b) const
// {
//     size_t a_size = a.size();
//     size_t b_size = b.size();
//     std::vector<bool> result(a_size + b_size, false);

//     for (size_t i = 0; i < a_size; ++i)
//     {
//         bool carry = false;
//         for (size_t j = 0; j < b_size; ++j)
//         {
//             bool product = a[i] && b[j];
//             bool sum = result[i + j] ^ product ^ carry;
//             carry = (result[i + j] && product) || (carry && (result[i + j] ^ product));
//             result[i + j] = sum;
//         }

//         if (carry)
//         {
//             result[i + b_size] = result[i + b_size] ^ carry;
//         }
//     }

//     return result;
// }

void LongNumber::align(LongNumber &a, LongNumber &b) const
{
    int magn = std::max(a.point_position, b.point_position);
    int prec = std::max(a.precision, b.precision);

    a.array.insert(a.array.begin(), magn - a.point_position, false);
    a.point_position += magn - a.point_position;
    a.array.insert(a.array.end(), prec - (a.array.size() - a.point_position), false);

    b.array.insert(b.array.begin(), magn - b.point_position, false);
    b.point_position += magn - b.point_position;
    b.array.insert(b.array.end(), prec - (b.array.size() - b.point_position), false);
}

void LongNumber::make_norm()
{
    if (point_position > array.size())
        point_position = array.size();
    while (array.size() > 1 && point_position > 0 && array.front() == false)
    {
        array.erase(array.begin());
        point_position--;
    }
    while (array.size() > 1 && point_position < array.size() && (array.back() == false || precision < (int)array.size() - point_position))
    {
        array.pop_back();
    }
}

LongNumber LongNumber::operator/(const LongNumber &div) const
{

    LongNumber q(0, std::max(precision, div.precision));
    LongNumber d = *this;
    LongNumber normDiv = div;

    d = d << (normDiv.array.size() - normDiv.point_position);
    normDiv = normDiv << (normDiv.array.size() - normDiv.point_position);

    while (d.array.size() - d.point_position <= d.precision)
        d.array.push_back(false);

    q.sign = (d.sign != normDiv.sign);
    d.sign = false;
    normDiv.sign = false;

    LongNumber r(0, precision);

    q.array.clear();
    q.point_position = -1;

    for (int i = 0; i < (int)d.array.size(); ++i)
    {
        r = r << 1;
        r.array[(int)r.array.size() - 1] = d.array[i];

        if (r >= normDiv)
        {
            r = r - normDiv;
            q.array.push_back(true);
        }
        else
        {
            q.array.push_back(false);
        }
        if (i >= d.point_position && q.point_position == -1)
        {
            q.point_position = q.array.size() - 1;
        }
    }

    q.make_norm();

    return q;
}
std::ostream &operator<<(std::ostream &out, const LongNumber &number)
{
    if (number.sign)
    {
        out << "-";
    }

    int int_part_size = number.point_position;
    for (int i = 0; i < int_part_size; ++i)
    {
        out << (i < number.array.size() ? (number.array[i] ? "1" : "0") : "0");
    }

    out << ".";

    for (size_t i = int_part_size; i < number.precision; ++i)
    {
        out << (i < number.array.size() ? (number.array[i] ? "1" : "0") : "0");
    }

    return out;
}

