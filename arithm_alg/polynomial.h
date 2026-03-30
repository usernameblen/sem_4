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

    //побитовое возведение в степень
    template<typename T>
    T power(const T& base, int exp) 
    {
        if (exp < 0) 
            throw std::invalid_argument("Negative exponent in power()");
        if (exp == 0) 
            return T(1);

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

    template<typename T>
    T divCoeff(const T& a, const T& b)
    {
        if (isZeroCoeff(b)) {
            throw std::runtime_error("Division by zero coefficient");
        }
        return a / b;
    }

    template<>
    inline int divCoeff<int>(const int& a, const int& b)
    {
        if (b == 0) {
            throw std::runtime_error("Division by zero coefficient");
        }

        if (a % b != 0) {
            throw std::runtime_error("Non-exact division in integer coefficients");
        }

        return a / b;
    }

}

template<typename CoefType>
struct Monomial 
{
    CoefType coeff;
    std::map<std::string, int> exponents;
};

enum class MonomialOrder { LEX, GRLEX, GREVLEX, INVLEX, RINVLEX };

template<typename CoefType>
bool compareMonomials(const Monomial<CoefType>& a,
    const Monomial<CoefType>& b,
    const std::vector<std::string>& variables,
    MonomialOrder order);


template<typename CoefType>
struct Node 
{
    std::map<int, Node*> children;

    CoefType coeff;

    //конечный узел
    bool isTerminal;

    int degree;

    Node() : coeff(CoefType()), isTerminal(false), degree(0) {}

    ~Node() 
    {
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


template<typename CoefType>
class Polynomial {

public:
    //псевдоним типа
    using MonomType = Monomial<CoefType>;

    explicit Polynomial(const std::vector<std::string>& vars);

    //многочлен из одного монома
    Polynomial(const std::vector<std::string>& vars,
        const CoefType& coeff,
        const std::map<std::string, int>& exponents);

    Polynomial(const Polynomial& other);

    Polynomial(Polynomial&& other) noexcept;

    ~Polynomial();

    Polynomial& operator=(const Polynomial& other);

    Polynomial& operator=(Polynomial&& other) noexcept;

    //операции
    void addTerm(const CoefType& coeff,
        const std::map<std::string, int>& exponents);

    CoefType getCoeff(const std::map<std::string, int>& exponents) const;

    bool isZero() const;

    std::vector<MonomType> support() const;

    CoefType evaluate(const std::map<std::string, CoefType>& point) const;

    //однородность
    int homogeneousDegree() const;

    Polynomial<CoefType> homogeneousPart(int degree) const;

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

    static std::string monomToString(const CoefType& coeff,
        const std::map<std::string, int>& exp);
private:
    Node<CoefType>* root_;
    std::vector<std::string> variables_;


    void collectSupport(const Node<CoefType>* node,
        int varIdx,
        std::map<std::string, int>& current,
        std::vector<MonomType>& result) const;

    // рекурсивное добавление всех мономов из поддерева src в *this
    // с умножением коэффициентов на factor
    void addSubtree(const Node<CoefType>* src,
        int varIdx,
        std::map<std::string, int>& current,
        const CoefType& factor);

    // Проверка совместимости переменных двух многочленов
    void checkCompat(const Polynomial& other) const;

    


    void multiplyTrees(
        const Node<CoefType>* nodeA,
        const Node<CoefType>* nodeB,
        int varIdx,
        std::map<std::string, int>& current,
        Polynomial<CoefType>& result) const;

    CoefType evaluateNode(
        const Node<CoefType>* node,
        int varIdx,
        CoefType accumulated,
        const std::map<std::string, CoefType>& point) const;




    //hw 5
public:
    MonomType leadingMonomial(MonomialOrder order) const;
    MonomType leadingTerm(MonomialOrder order) const;
    CoefType  leadingCoefficient(MonomialOrder order) const;
    std::vector<int> multiDegree(MonomialOrder order) const;


    struct DivisionResult 
    {
        std::vector<Polynomial> quotients;
        Polynomial remainder;

        DivisionResult(const std::vector<std::string>& vars, int s)
            : quotients(s, Polynomial(vars)), remainder(vars) {
        }
    };

    DivisionResult divideBy(const std::vector<Polynomial>& F,
        MonomialOrder order) const;

};




#include "Polynomial.hpp"