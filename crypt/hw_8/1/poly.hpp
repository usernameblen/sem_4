#pragma once

#include <vector>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <cstdint>
#include <string>
#include <utility>

using gf_elem = uint64_t;

inline gf_elem gf_mul(gf_elem a, gf_elem b, gf_elem mod, int n) {
    gf_elem result = 0;
    gf_elem high_bit = gf_elem(1) << n;
    gf_elem mask = high_bit - 1;

    while (b) {
        if (b & 1) 
            result ^= a;
        b >>= 1;
        a <<= 1;
        if (a & high_bit) 
            a ^= mod;
    }
    return result & mask;
}

//бинарное возведенеи в степень
inline gf_elem gf_pow(gf_elem base, uint64_t exp, gf_elem mod, int n) {
    gf_elem result = 1;
    gf_elem mask = (gf_elem(1) << n) - 1;
    base &= mask;
    while (exp) {
        if (exp & 1) 
            result = gf_mul(result, base, mod, n);
        base = gf_mul(base, base, mod, n);
        exp >>= 1;
    }
    return result;
}

inline gf_elem gf_inv(gf_elem a, gf_elem mod, int n) {
    if (a == 0) 
        return 0;
    return gf_pow(a, (uint64_t(1) << n) - 2, mod, n);
}

class GF2nPoly {
public:
    GF2nPoly(int n, gf_elem field_mod){
        if (n <= 0 || n > 63)
            throw std::invalid_argument("n must be in range 1..63 for uint64_t implementation");
        n_ = n;

        if ((field_mod >> n_) != 1)
            throw std::invalid_argument("field_mod must be degree n irreducible polynomial");

        fm_ = field_mod;
    }

    GF2nPoly(int n, gf_elem field_mod, std::vector<gf_elem> coeffs)
        : GF2nPoly(n, field_mod)  // делегирующий конструктор
    {
        c_ = std::move(coeffs);
        normalize();
    }

    GF2nPoly(const GF2nPoly&) = default;
    GF2nPoly& operator=(const GF2nPoly&) = default;
    GF2nPoly(GF2nPoly&&) noexcept = default;
    GF2nPoly& operator=(GF2nPoly&&) noexcept = default;

    bool is_zero() const {
        return c_.empty();
    }

    int degree() const {
        return is_zero() ? -1 : static_cast<int>(c_.size()) - 1;
    }

    gf_elem coeff(int i) const {
        if (i < 0)
            throw std::invalid_argument("index must be >=0");
        if (i >= static_cast<int>(c_.size())) 
            return 0;
        return c_[i];
    }

    void set_coeff(int i, gf_elem val) {
        if (i < 0) 
            throw std::invalid_argument("index must be >=0");

        if (val == 0 && i >= static_cast<int>(c_.size())) 
            return;

        if (i >= static_cast<int>(c_.size())) 
            c_.resize(i + 1, 0);
        c_[i] = val;
        normalize();
    }

    void check_compat(const GF2nPoly& other) const {
        if (n_ != other.n_ || fm_ != other.fm_)
            throw std::invalid_argument("incompatible fields");
    }

    GF2nPoly& multiply(const GF2nPoly& other, const GF2nPoly& modulus_poly) {
        check_compat(other);
        check_compat(modulus_poly);

        GF2nPoly tmp = raw_mul(*this, other);
        tmp %= modulus_poly;
        c_ = std::move(tmp.c_);
        return *this;
    }

    //рае
    GF2nPoly inverse(const GF2nPoly& modulus_poly) const {
        if (is_zero()) 
            return GF2nPoly(n_, fm_);

        GF2nPoly r0 = modulus_poly;
        GF2nPoly r1 = *this;
        GF2nPoly s0(n_, fm_);
        GF2nPoly s1(n_, fm_, { 1 });

        while (!r1.is_zero()) {
            std::pair<GF2nPoly, GF2nPoly> qr = divmod(r0, r1);
            GF2nPoly qs1 = raw_mul(qr.first, s1);
            GF2nPoly new_s = s0;
            new_s += qs1;
            s0 = std::move(s1);
            s1 = std::move(new_s);
            r0 = std::move(r1);
            r1 = std::move(qr.second);
        }

        if (r0.is_zero() || r0.degree() != 0 || r0.c_.back() == 0)
            throw std::domain_error("polynomial is not invertible");

        gf_elem lc_inv = gf_inv(r0.c_.back(), fm_, n_);
        for (auto& coef : s0.c_) 
            coef = gf_mul(coef, lc_inv, fm_, n_);
        s0.normalize();
        return s0;
    }

    bool is_irreducible() const {
        int d = degree();
        if (d <= 0) 
            return false;

        GF2nPoly x(n_, fm_, { 0, 1 }); //x
        GF2nPoly xpow(n_, fm_, { 0, 1 }); //x^q^k, q = 2^n, k = 1, 2,..., d/2
        uint64_t qn = uint64_t(1) << n_;

        for (int k = 1; 2 * k <= d; ++k) {
            xpow = powmod(xpow, qn, *this);
            GF2nPoly diff = xpow;
            diff += x;
            GF2nPoly g = poly_gcd(diff, *this);
            if (g.degree() > 0) //g != 1
                return false;
        }
        return true;
    }

    GF2nPoly& operator+=(const GF2nPoly& other) {
        check_compat(other);
        size_t sz = std::max(c_.size(), other.c_.size());
        c_.resize(sz, 0);
        for (size_t i = 0; i < other.c_.size(); ++i) 
            c_[i] ^= other.c_[i];
        normalize();
        return *this;
    }

    GF2nPoly operator+(const GF2nPoly& other) const {
        GF2nPoly tmp(*this);
        tmp += other;
        return tmp;
    }

    GF2nPoly& operator%=(const GF2nPoly& mod) {
        check_compat(mod);
        std::pair<GF2nPoly, GF2nPoly> qr = divmod(*this, mod);
        c_ = std::move(qr.second.c_);
        return *this;
    }

    std::string to_string() const {
        if (is_zero()) return "0";
        std::ostringstream oss;
        bool first = true;
        for (int i = degree(); i >= 0; --i) {
            if (c_[i] == 0) continue;
            if (!first) oss << " + ";
            oss << c_[i];
            if (i > 0) oss << "*x^" << i;
            first = false;
        }
        return oss.str();
    }

private:
    int n_;
    gf_elem fm_;
    std::vector<gf_elem> c_;

    void normalize() {
        while (!c_.empty() && c_.back() == 0) 
            c_.pop_back();
    }

    static GF2nPoly raw_mul(const GF2nPoly& a, const GF2nPoly& b) {
        if (a.is_zero() || b.is_zero()) 
            return GF2nPoly(a.n_, a.fm_);

        int da = a.degree(), db = b.degree();
        std::vector<gf_elem> res(da + db + 1, 0);

        for (int i = 0; i <= da; ++i) {
            if (a.c_[i] == 0) 
                continue;
            for (int j = 0; j <= db; ++j)
                res[i + j] ^= gf_mul(a.c_[i], b.c_[j], a.fm_, a.n_);
        }
        return GF2nPoly(a.n_, a.fm_, std::move(res));
    }

    //деление уголком
    static std::pair<GF2nPoly, GF2nPoly> divmod(const GF2nPoly& a, const GF2nPoly& b) {
        if (b.is_zero()) 
            throw std::domain_error("division by zero polynomial");
        if (a.degree() < b.degree()) 
            return { GF2nPoly(a.n_, a.fm_), a };

        GF2nPoly remainder = a;
        std::vector<gf_elem> qc(a.degree() - b.degree() + 1, 0);
        gf_elem lc_b_inv = gf_inv(b.c_.back(), a.fm_, a.n_);

        while (!remainder.is_zero() && remainder.degree() >= b.degree()) {
            int shift = remainder.degree() - b.degree();
            gf_elem lc = gf_mul(remainder.c_.back(), lc_b_inv, a.fm_, a.n_);
            qc[shift] = lc;

            if (remainder.c_.size() < b.degree() + shift + 1)
                remainder.c_.resize(b.degree() + shift + 1, 0);
            for (int i = 0; i <= b.degree(); ++i)
                remainder.c_[i + shift] ^= gf_mul(lc, b.c_[i], a.fm_, a.n_);
            remainder.normalize();
        }

        return { GF2nPoly(a.n_, a.fm_, std::move(qc)), remainder };
    }

    static GF2nPoly powmod(const GF2nPoly& base, uint64_t exp, const GF2nPoly& mod) {
        base.check_compat(mod);
        GF2nPoly result(base.n_, base.fm_, { 1 });
        GF2nPoly b = divmod(base, mod).second;
        while (exp) {
            if (exp & 1) { result = raw_mul(result, b); result = divmod(result, mod).second; }
            b = raw_mul(b, b);
            b = divmod(b, mod).second;
            exp >>= 1;
        }
        return result;
    }

    static GF2nPoly poly_gcd(GF2nPoly a, GF2nPoly b) {
        while (!b.is_zero()) {
            GF2nPoly r = divmod(a, b).second;
            a = std::move(b);
            b = std::move(r);
        }
        if (!a.is_zero()) {
            gf_elem lc_inv = gf_inv(a.c_.back(), a.fm_, a.n_);
            for (auto& coef : a.c_) coef = gf_mul(coef, lc_inv, a.fm_, a.n_);
            a.normalize();
        }
        return a;
    }
};
