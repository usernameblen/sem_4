#pragma once

template<typename Coef>
Polynomial<Coef>::Polynomial(const std::vector<std::string>& vars)
    : root_(new Node<Coef>()), variables_(vars)
{
}


template<typename Coef>
Polynomial<Coef>::Polynomial(const std::vector<std::string>& vars,
    const Coef& coeff,
    const std::map<std::string, int>& exponents)
    : root_(new Node<Coef>()), variables_(vars)
{
    addTerm(coeff, exponents);
}


template<typename Coef>
Polynomial<Coef>::Polynomial(const Polynomial<Coef>& other)
    : root_(other.root_->deepCopy()), variables_(other.variables_) {}

template<typename Coef>
Polynomial<Coef>::Polynomial(Polynomial<Coef>&& other) noexcept
    : root_(other.root_), variables_(std::move(other.variables_))
{
    other.root_ = nullptr;
}

template<typename Coef>
Polynomial<Coef>::~Polynomial() 
{
    delete root_;
}


template<typename Coef>
Polynomial<Coef>& Polynomial<Coef>::operator=(const Polynomial<Coef>& other) 
{
    if (this != &other) {
        Polynomial<Coef> tmp(other);
        std::swap(root_, tmp.root_);
        std::swap(variables_, tmp.variables_);
    }
    return *this;
}

template<typename Coef>
Polynomial<Coef>& Polynomial<Coef>::operator=(Polynomial<Coef>&& other) noexcept 
{
    if (this != &other) {
        delete root_;
        root_ = other.root_;
        variables_ = std::move(other.variables_);
        other.root_ = nullptr;
    }
    return *this;
}


template<typename Coef>
void Polynomial<Coef>::checkCompat(const Polynomial<Coef>& other) const 
{
    if (variables_ != other.variables_) {
        throw std::invalid_argument(
            "Polynomial: variable lists do not match");
    }
}


template<typename Coef>
void Polynomial<Coef>::collectSupport(
    const Node<Coef>* node,
    int varIdx,
    std::map<std::string, int>& current,
    std::vector<MonomType>& result) const
{
    if (node == nullptr) 
        return;

    if (node->isTerminal && !poly_detail::isZeroCoeff(node->coeff)) 
    {
        MonomType m;
        m.coeff = node->coeff;
        m.exponents = current;
        result.push_back(m);
    }

    if (varIdx >= static_cast<int>(variables_.size())) 
        return;

    const std::string& var = variables_[varIdx];
    for (auto& kv : node->children) {
        int exp = kv.first;
        if (exp != 0) {
            current[var] = exp;
        }
        else {
            current.erase(var);
        }
        collectSupport(kv.second, varIdx + 1, current, result);
        current.erase(var);
    }
}

//добавляем мономы в многочлен
template<typename Coef>
void Polynomial<Coef>::addSubtree(
    const Node<Coef>* src,
    int varIdx,
    std::map<std::string, int>& current,
    const Coef& factor)
{
    if (src == nullptr) 
        return;

    if (src->isTerminal) {
        addTerm(factor * src->coeff, current);
    }

    if (varIdx >= static_cast<int>(variables_.size())) 
        return;

    const std::string& var = variables_[varIdx];
    for (auto& kv : src->children) {
        current[var] = kv.first;
        addSubtree(kv.second, varIdx + 1, current, factor);
    }
    current.erase(var);
}

//добавляем моном в многочлен
template<typename Coef>
void Polynomial<Coef>::addTerm(const Coef& coeff,
    const std::map<std::string, int>& exponents)
{
    if (poly_detail::isZeroCoeff(coeff)) 
        return;

    // проверяем, что все переменные из exponents присутствуют в variables_
    for (auto& kv : exponents) {
        auto it = std::find(variables_.begin(), variables_.end(), kv.first);
        if (it == variables_.end()) {
            throw std::invalid_argument(
                "addTerm: variable '" + kv.first + "' not in variable list");
        }
    }

    Node<Coef>* cur = root_;
    int cumDegree = 0;

    for (const std::string& var : variables_) 
    {
        int exp = 0;
        auto it = exponents.find(var);
        if (it != exponents.end()) 
            exp = it->second;

        cumDegree += exp;

        // находим или создаём дочерний узел
        auto childIt = cur->children.find(exp);
        if (childIt == cur->children.end()) {
            Node<Coef>* child = new Node<Coef>();
            child->degree = cumDegree;
            cur->children[exp] = child;
            cur = child;
        }
        else {
            cur = childIt->second;
        }
    }

    // если узел терминальный - то есть уже существующий - складываем коэф
    if (cur->isTerminal) 
    {
        cur->coeff = cur->coeff + coeff;
    }
    else 
    {
        cur->coeff = coeff;
        cur->isTerminal = true;
    }

    if (poly_detail::isZeroCoeff(cur->coeff)) {
        cur->isTerminal = false;
        cur->coeff = Coef();
    }
}


template<typename Coef>
Coef Polynomial<Coef>::getCoeff(
    const std::map<std::string, int>& exponents) const
{
    const Node<Coef>* cur = root_;
    for (const std::string& var : variables_) 
    {
        int exp = 0;
        auto it = exponents.find(var);
        if (it != exponents.end()) 
            exp = it->second;

        auto childIt = cur->children.find(exp);
        if (childIt == cur->children.end()) 
            return Coef(); // моном отсутствует
        cur = childIt->second;
    }
    return cur->isTerminal ? cur->coeff : Coef();
}


namespace poly_detail {
    template<typename Coef>
    bool hasNonZero(const Node<Coef>* node) 
    {
        if (node == nullptr) 
            return false;
        if (node->isTerminal && !isZeroCoeff(node->coeff)) 
            return true;
        for (auto& kv : node->children) {
            if (hasNonZero(kv.second)) 
                return true;
        }
        return false;
    }
}

template<typename Coef>
bool Polynomial<Coef>::isZero() const 
{
    return !poly_detail::hasNonZero(root_);
}


template<typename Coef>
std::vector<Monomial<Coef>> Polynomial<Coef>::support() const //носитель
{
    std::vector<MonomType> result;
    std::map<std::string, int> current;
    collectSupport(root_, 0, current, result);
    return result;
}


template<typename Coef>
Coef Polynomial<Coef>::evaluate(
    const std::map<std::string, Coef>& point) const
{
    // проверяем, что все переменные заданы в точке
    for (const std::string& var : variables_) 
    {
        if (point.find(var) == point.end()) 
        {
            throw std::invalid_argument(
                "evaluate: variable '" + var + "' is missing in the point");
        }
    }

    Coef total(0);
    auto supp = support();
    for (const MonomType& m : supp) 
    {
        Coef term = m.coeff;
        for (const std::string& var : variables_) 
        {
            int exp = 0;
            auto it = m.exponents.find(var);
            if (it != m.exponents.end()) 
                exp = it->second;

            if (exp != 0) {
                term = term * poly_detail::power(point.at(var), exp);
            }
        }
        total = total + term;
    }
    return total;
}


//проверка однородности
template<typename Coef>
int Polynomial<Coef>::homogeneousDegree() const 
{
    auto supp = support();
    if (supp.empty()) return 0; // нулевой многочлен

    // вычисляем полную степень первого монома
    int ref = 0;
    for (auto& kv : supp[0].exponents) 
        ref += kv.second;

    for (size_t i = 1; i < supp.size(); ++i) 
    {
        int deg = 0;
        for (auto& kv : supp[i].exponents) 
            deg += kv.second;
        if (deg != ref) return -1;
    }
    return ref;
}


template<typename Coef>
Polynomial<Coef> Polynomial<Coef>::homogeneousPart(int degree) const 
{
    Polynomial<Coef> result(variables_);
    auto supp = support();
    for (const MonomType& m : supp) 
    {
        int deg = 0;
        for (auto& kv : m.exponents) 
            deg += kv.second;
        if (deg == degree) {
            result.addTerm(m.coeff, m.exponents);
        }
    }
    return result;
}


template<typename Coef>
Polynomial<Coef> operator+(const Polynomial<Coef>& a,
    const Polynomial<Coef>& b)
{
    a.checkCompat(b);
    Polynomial<Coef> result(a);  // глубокая копия a
    std::map<std::string, int> current;
    result.addSubtree(b.root_, 0, current, Coef(1));
    return result;
}

template<typename Coef>
Polynomial<Coef> operator-(const Polynomial<Coef>& a,
    const Polynomial<Coef>& b)
{
    a.checkCompat(b);
    Polynomial<Coef> result(a);
    std::map<std::string, int> current;
    result.addSubtree(b.root_, 0, current, Coef(-1));
    return result;
}


template<typename Coef>
Polynomial<Coef> operator*(const Polynomial<Coef>& a,
    const Polynomial<Coef>& b)
{
    a.checkCompat(b);
    Polynomial<Coef> result(a.variables_);
    auto suppA = a.support();
    auto suppB = b.support();

    for (const auto& ma : suppA) 
    {
        for (const auto& mb : suppB) 
        {
            Coef c = ma.coeff * mb.coeff;
            if (poly_detail::isZeroCoeff(c)) 
                continue;

            // складываем степени
            std::map<std::string, int> exp;
            for (auto& kv : ma.exponents) {
                if (kv.second != 0) 
                    exp[kv.first] += kv.second;
            }
            for (auto& kv : mb.exponents) {
                if (kv.second != 0) 
                    exp[kv.first] += kv.second;
            }
            result.addTerm(c, exp);
        }
    }
    return result;
}

template<typename Coef>
Polynomial<Coef>& Polynomial<Coef>::operator+=(const Polynomial<Coef>& other) {
    *this = *this + other;
    return *this;
}

template<typename Coef>
Polynomial<Coef>& Polynomial<Coef>::operator-=(const Polynomial<Coef>& other) {
    *this = *this - other;
    return *this;
}

template<typename Coef>
Polynomial<Coef>& Polynomial<Coef>::operator*=(const Polynomial<Coef>& other) {
    *this = *this * other;
    return *this;
}

template<typename Coef>
bool operator==(const Polynomial<Coef>& a, const Polynomial<Coef>& b) 
{
    if (a.variables_ != b.variables_) 
        return false;
    auto suppA = a.support();
    auto suppB = b.support();
    if (suppA.size() != suppB.size()) 
        return false;

    std::map<std::map<std::string, int>, Coef> mapB;
    for (auto& m : suppB) 
        mapB[m.exponents] = m.coeff;

    for (auto& m : suppA) {
        auto it = mapB.find(m.exponents);
        if (it == mapB.end()) 
            return false;
        Coef diff = m.coeff + (Coef(-1) * it->second);
        if (!poly_detail::isZeroCoeff(diff)) 
            return false;
    }
    return true;
}

template<typename Coef>
bool operator!=(const Polynomial<Coef>& a, const Polynomial<Coef>& b) {
    return !(a == b);
}


namespace poly_detail {

    template<typename T>
    std::string coeffToStr(const T& c) 
    {
        std::ostringstream oss;
        oss << c;
        return oss.str();
    }

    // Специализация для комплексных: выводим как (a+bi)
    template<>
    std::string coeffToStr<std::complex<double>>(
        const std::complex<double>& c) 
    {
        std::ostringstream oss;
        oss << "(" << c.real();
        if (c.imag() >= 0) oss << "+";
        oss << c.imag() << "i)";
        return oss.str();
    }

}

template<typename Coef>
std::string Polynomial<Coef>::monomToString(
    const Coef& coeff,
    const std::map<std::string, int>& exp)
{
    std::ostringstream oss;
    oss << poly_detail::coeffToStr(coeff);
    for (auto& kv : exp) {
        if (kv.second == 0) continue;
        oss << kv.first;
        if (kv.second != 1) oss << "^" << kv.second;
    }
    return oss.str();
}

template<typename Coef>
std::ostream& operator<<(std::ostream& os, const Polynomial<Coef>& p) 
{
    auto supp = p.support();
    if (supp.empty()) {
        os << "0";
        return os;
    }

    bool first = true;
    for (auto& m : supp) 
    {
        std::string term = Polynomial<Coef>::monomToString(m.coeff, m.exponents);
        if (first) {
            os << term;
            first = false;
        }
        else {
            if (term[0] == '-') {
                os << " - " << term.substr(1);
            }
            else {
                os << " + " << term;
            }
        }
    }
    return os;
}