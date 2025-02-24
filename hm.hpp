#ifndef HM_HPP
#define HM_HPP

#include <iostream>
#include <vector>
class LongNumber
{
private:
    std::vector<bool> array;
    int precision;
    bool sign;
    int point_position;
    LongNumber sum_of_positive(const LongNumber &a, const LongNumber &b) const;
    LongNumber diff_of_positive(const LongNumber &a, const LongNumber &b) const;

    LongNumber plus_bit(const LongNumber &number1, const LongNumber &number2) const;
    LongNumber minus_bit(const LongNumber &number1, const LongNumber &number2) const;
    friend std::vector<bool> getArray(const LongNumber &ln);
    std::vector<bool> multiplication(const std::vector<bool> &a, const std::vector<bool> &b) const;
    void make_norm();
    void setPrecision(int new_precision);
    void align(LongNumber &a, LongNumber &b) const;
    friend std::ostream &operator<<(std::ostream &out, const LongNumber &number);
    LongNumber negative(const LongNumber &number) const;

public:
    LongNumber abs() const;
    LongNumber();

    LongNumber(int value, int precision);


    LongNumber( double value, int precision);


    LongNumber(long double value, int precision);

    LongNumber(const LongNumber &other);

    ~LongNumber();

    LongNumber &operator=(const LongNumber &other);

    LongNumber operator+(const LongNumber &other) const;
    LongNumber operator-(const LongNumber &other) const;
    LongNumber operator/(const LongNumber &other) const;
    LongNumber operator*(const LongNumber &other) const;
    LongNumber operator>>(int shift) const;
    LongNumber operator<<(int shift) const;

    // bool isZero() const;
    bool operator==(const LongNumber &other) const;
    bool operator!=(const LongNumber &other) const;
    bool operator>(const LongNumber &other) const;
    bool operator>=(const LongNumber &other) const;
    bool operator<(const LongNumber &other) const;
    bool operator<=(const LongNumber &other) const;
};
LongNumber operator"" _longnum(long double number);

#endif
