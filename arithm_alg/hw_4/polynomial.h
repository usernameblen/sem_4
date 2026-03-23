#pragma once

#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <complex>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <type_traits>


namespace poly_detail 
{

    //побитовое возведение 
    template<typename T>
    T power(const T& base, int exp) 
    {
        if (exp < 0) throw std::invalid_argument("Negative exponent in power()");
        if (exp == 0) return T(1);
        T result(1);
        T b = base;
        int e = exp;
        while (e > 0) {
            if (e & 1) 
                result = result * b;
            b = b * b;
            e >>= 1;
        }
        return result;
    }

    //побитовое возведение может накапливать ошибки округления
    template<>
    double power<double>(const double& base, int exp) 
    {
        return std::pow(base, static_cast<double>(exp));
    }


    template<>
    std::complex<double> power<std::complex<double>>(
        const std::complex<double>& base, int exp) 
    {
        return std::pow(base, exp);
    }

    template<typename T>
    bool isZeroCoeff(const T& v) 
    { 
        return v == T(0); 
    }

    template<>
    bool isZeroCoeff<double>(const double& v) 
    {
        return std::abs(v) < 1e-15;
    }

    template<>
    inline bool isZeroCoeff<std::complex<double>>(const std::complex<double>& v) 
    {
        return std::abs(v) < 1e-15;
    }

}

template<typename Coef>
struct Monomial 
{
    Coef coeff;
    std::map<std::string, int> exponents;
};

template<typename Coef>
struct Node 
{
    std::map<int, Node*> children;

    Coef coeff;

    //конечный узел
    bool isTerminal;

    int degree;

    Node() : coeff(Coef()), isTerminal(false), degree(0) {}

    ~Node() {
        for (auto& kv : children) {
            delete kv.second;
        }
    }

    Node* deepCopy() const 
    {
        Node* copy = new Node();
        copy->coeff = coeff;
        copy->isTerminal = isTerminal;
        copy->degree = degree;
        for (auto& kv : children) 
        {
            copy->children[kv.first] = kv.second->deepCopy();
        }
        return copy;
    }

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
};


template<typename Coef>
class Polynomial {

public:
    //псевдоним типа
    using MonomType = Monomial<Coef>;

    explicit Polynomial(const std::vector<std::string>& vars);

    Polynomial(const std::vector<std::string>& vars,
        const Coef& coeff,
        const std::map<std::string, int>& exponents);

    Polynomial(const Polynomial& other);

    Polynomial(Polynomial&& other) noexcept;

    ~Polynomial();

    Polynomial& operator=(const Polynomial& other);

    Polynomial& operator=(Polynomial&& other) noexcept;

    //операции
    void addTerm(const Coef& coeff,
        const std::map<std::string, int>& exponents);

    Coef getCoeff(const std::map<std::string, int>& exponents) const;

    bool isZero() const;

    std::vector<MonomType> support() const;

    Coef evaluate(const std::map<std::string, Coef>& point) const;

    int homogeneousDegree() const;

    Polynomial<Coef> homogeneousPart(int degree) const;

    const std::vector<std::string>& variables() const { return variables_; }


    //арифметические операции
    template<typename C>
    friend Polynomial<C> operator+(const Polynomial<C>& a,
        const Polynomial<C>& b);

    template<typename C>
    friend Polynomial<C> operator-(const Polynomial<C>& a,
        const Polynomial<C>& b);

    template<typename C>
    friend Polynomial<C> operator*(const Polynomial<C>& a,
        const Polynomial<C>& b);

    Polynomial& operator+=(const Polynomial& other);
    Polynomial& operator-=(const Polynomial& other);
    Polynomial& operator*=(const Polynomial& other);

    template<typename C>
    friend bool operator==(const Polynomial<C>& a, const Polynomial<C>& b);

    template<typename C>
    friend bool operator!=(const Polynomial<C>& a, const Polynomial<C>& b);

    template<typename C>
    friend std::ostream& operator<<(std::ostream& os, const Polynomial<C>& p);

private:
    Node<Coef>* root_;
    std::vector<std::string> variables_;


    void collectSupport(const Node<Coef>* node,
        int varIdx,
        std::map<std::string, int>& current,
        std::vector<MonomType>& result) const;

    // Рекурсивное добавление всех мономов из поддерева src в *this
    // с умножением коэффициентов на factor
    void addSubtree(const Node<Coef>* src,
        int varIdx,
        std::map<std::string, int>& current,
        const Coef& factor);

    // Проверка совместимости переменных двух многочленов
    void checkCompat(const Polynomial& other) const;

    static std::string monomToString(const Coef& coeff,
        const std::map<std::string, int>& exp);
};

#include "Polynomial.hpp"