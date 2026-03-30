#pragma once

template<typename CoefType>
Polynomial<CoefType>::Polynomial(const std::vector<std::string>& vars)
    : root_(new Node<CoefType>()), variables_(vars)
{
    std::sort(variables_.begin(), variables_.end());
}


template<typename CoefType>
Polynomial<CoefType>::Polynomial(const std::vector<std::string>& vars,
    const CoefType& coeff,
    const std::map<std::string, int>& exponents)
    : root_(new Node<CoefType>()), variables_(vars)
{
    std::sort(variables_.begin(), variables_.end());
    addTerm(coeff, exponents);
}


template<typename CoefType>
Polynomial<CoefType>::Polynomial(const Polynomial<CoefType>& other)
    : root_(other.root_->deepCopy()), variables_(other.variables_) {}

template<typename CoefType>
Polynomial<CoefType>::Polynomial(Polynomial<CoefType>&& other) noexcept
    : root_(other.root_), variables_(std::move(other.variables_))
{
    other.root_ = nullptr;
}

template<typename CoefType>
Polynomial<CoefType>::~Polynomial() 
{
    delete root_;
}


template<typename CoefType>
Polynomial<CoefType>& Polynomial<CoefType>::operator=(const Polynomial<CoefType>& other) 
{
    if (this != &other) {
        Polynomial<CoefType> tmp(other);
        std::swap(root_, tmp.root_);
        std::swap(variables_, tmp.variables_);
    }
    return *this;
}

template<typename CoefType>
Polynomial<CoefType>& Polynomial<CoefType>::operator=(Polynomial<CoefType>&& other) noexcept 
{
    if (this != &other) {
        delete root_;
        root_ = other.root_;
        variables_ = std::move(other.variables_);
        other.root_ = nullptr;
    }
    return *this;
}


template<typename CoefType>
void Polynomial<CoefType>::checkCompat(const Polynomial<CoefType>& other) const 
{
    if (variables_ != other.variables_) {
        throw std::invalid_argument(
            "Polynomial: variable lists do not match");
    }
}

//работает с одним мономом
template<typename CoefType>
void Polynomial<CoefType>::collectSupport(
    const Node<CoefType>* node,
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
template<typename CoefType>
void Polynomial<CoefType>::addSubtree(
    const Node<CoefType>* src,
    int varIdx,
    std::map<std::string, int>& current,
    const CoefType& factor)
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
template<typename CoefType>
void Polynomial<CoefType>::addTerm(const CoefType& coeff,
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

    Node<CoefType>* cur = root_;
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
            Node<CoefType>* child = new Node<CoefType>();
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
        cur->coeff = CoefType();
    }
}


template<typename CoefType>
CoefType Polynomial<CoefType>::getCoeff(
    const std::map<std::string, int>& exponents) const
{
    const Node<CoefType>* cur = root_;
    for (const std::string& var : variables_) 
    {
        int exp = 0;
        auto it = exponents.find(var);
        if (it != exponents.end()) 
            exp = it->second;

        auto childIt = cur->children.find(exp);
        if (childIt == cur->children.end()) 
            return CoefType(); // моном отсутствует
        cur = childIt->second;
    }
    return cur->isTerminal ? cur->coeff : CoefType();
}


namespace poly_detail {
    template<typename CoefType>
    bool hasNonZero(const Node<CoefType>* node) 
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

template<typename CoefType>
bool Polynomial<CoefType>::isZero() const 
{
    return !poly_detail::hasNonZero(root_);
}


template<typename CoefType>
std::vector<Monomial<CoefType>> Polynomial<CoefType>::support() const //носитель
{
    std::vector<MonomType> result;
    std::map<std::string, int> current;
    collectSupport(root_, 0, current, result);
    return result;
}


template<typename CoefType>
CoefType Polynomial<CoefType>::evaluateNode(
    const Node<CoefType>* node,
    int varIdx,
    CoefType accumulated,
    const std::map<std::string, CoefType>& point) const
{
    if (node == nullptr)
        return CoefType(0);

    CoefType total(0);

    if (node->isTerminal && !poly_detail::isZeroCoeff(node->coeff)) {
        total += (accumulated * node->coeff);
    }

    if (varIdx >= static_cast<int>(variables_.size()))
        return total;

    const std::string& var = variables_[varIdx];
    const CoefType& val = point.at(var);

    for (auto& kv : node->children) {
        int exp = kv.first;
        CoefType newAcc = accumulated * poly_detail::power(val, exp);
        total = total + evaluateNode(kv.second, varIdx + 1, newAcc, point);
    }
    return total;
}

template<typename CoefType>
CoefType Polynomial<CoefType>::evaluate(
    const std::map<std::string, CoefType>& point) const
{
    for (const std::string& var : variables_)
    {
        if (point.find(var) == point.end())
        {
            throw std::invalid_argument(
                "evaluate: variable '" + var + "' is missing in the point");
        }
    }
    return evaluateNode(root_, 0, CoefType(1), point);
}


//проверка однородности
template<typename CoefType>
int Polynomial<CoefType>::homogeneousDegree() const 
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


template<typename CoefType>
Polynomial<CoefType> Polynomial<CoefType>::homogeneousPart(int degree) const 
{
    Polynomial<CoefType> result(variables_);
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


template<typename CoefType>
Polynomial<CoefType>& Polynomial<CoefType>::operator+=(const Polynomial<CoefType>& other)
{
    checkCompat(other);
    std::map<std::string, int> current;
    addSubtree(other.root_, 0, current, CoefType(1));
    return *this;
}

template<typename CoefType>
Polynomial<CoefType>& Polynomial<CoefType>::operator-=(const Polynomial<CoefType>& other)
{
    checkCompat(other);
    std::map<std::string, int> current;
    addSubtree(other.root_, 0, current, CoefType(-1));
    return *this;
}

template<typename CoefType>
Polynomial<CoefType>& Polynomial<CoefType>::operator*=(const Polynomial<CoefType>& other)
{
    checkCompat(other);
    Polynomial<CoefType> result(variables_);
    std::map<std::string, int> current;
    multiplyTrees(root_, other.root_, 0, current, result);
    *this = std::move(result);
    return *this;
}

template<typename CoefType>
Polynomial<CoefType> operator+(const Polynomial<CoefType>& a,
    const Polynomial<CoefType>& b)
{
    Polynomial<CoefType> result(a);
    result += b;
    return result;
}

template<typename CoefType>
Polynomial<CoefType> operator-(const Polynomial<CoefType>& a,
    const Polynomial<CoefType>& b)
{
    Polynomial<CoefType> result(a);
    result -= b;
    return result;
}

template<typename CoefType>
Polynomial<CoefType> operator*(const Polynomial<CoefType>& a,
    const Polynomial<CoefType>& b)
{
    Polynomial<CoefType> result(a);
    result *= b;
    return result;
}



template<typename CoefType>
void Polynomial<CoefType>::multiplyTrees(
    const Node<CoefType>* nodeA,
    const Node<CoefType>* nodeB,
    int varIdx,
    std::map<std::string, int>& current,
    Polynomial<CoefType>& result) const
{
    if (nodeA == nullptr || nodeB == nullptr)
        return;

    if (varIdx >= variables_.size()) {
        if (nodeA->isTerminal && nodeB->isTerminal) {
            CoefType c = nodeA->coeff * nodeB->coeff;
            if (!poly_detail::isZeroCoeff(c))
                result.addTerm(c, current);
        }
        return;
    }

    const std::string& var = variables_[varIdx];

    for (auto& kvA : nodeA->children) {
        for (auto& kvB : nodeB->children) {
            int exp = kvA.first + kvB.first;
            if (exp != 0) {
                current[var] = exp;
            }
            else {
                current.erase(var);
            }
            multiplyTrees(kvA.second, kvB.second, varIdx + 1, current, result);
        }
    }
    current.erase(var);
}

template<typename CoefType>
bool operator==(const Polynomial<CoefType>& a, const Polynomial<CoefType>& b) 
{
    if (a.variables_ != b.variables_) 
        return false;
    auto suppA = a.support();
    auto suppB = b.support();
    if (suppA.size() != suppB.size()) 
        return false;

    std::map<std::map<std::string, int>, CoefType> mapB;
    for (auto& m : suppB) 
        mapB[m.exponents] = m.coeff;

    for (auto& m : suppA) {
        auto it = mapB.find(m.exponents);
        if (it == mapB.end()) 
            return false;
        CoefType diff = m.coeff + (CoefType(-1) * it->second);
        if (!poly_detail::isZeroCoeff(diff)) 
            return false;
    }
    return true;
}

template<typename CoefType>
bool operator!=(const Polynomial<CoefType>& a, const Polynomial<CoefType>& b) {
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

template<typename CoefType>
std::string Polynomial<CoefType>::monomToString(
    const CoefType& coeff,
    const std::map<std::string, int>& exp)
{
    std::ostringstream oss;
    if(coeff != 1)
        oss << poly_detail::coeffToStr(coeff);

    for (auto& kv : exp) {
        if (kv.second == 0) 
            continue;
        oss << kv.first;
        if (kv.second != 1) oss << "^" << kv.second;
    }
    return oss.str();
}

template<typename CoefType>
std::ostream& operator<<(std::ostream& os, const Polynomial<CoefType>& p) 
{
    auto supp = p.support();
    if (supp.empty()) {
        os << "0";
        return os;
    }

    bool first = true;
    for (auto& m : supp) 
    {
        std::string term = Polynomial<CoefType>::monomToString(m.coeff, m.exponents);
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




//hw 5
template<typename CoefType>
bool compareMonomials(const Monomial<CoefType>& a,
    const Monomial<CoefType>& b,
    const std::vector<std::string>& variables,
    MonomialOrder order)
{
    const int n = static_cast<int>(variables.size());
    std::vector<int> ea(n, 0), eb(n, 0); //векторы со степенями
    for (int i = 0; i < n; ++i) {
        auto itA = a.exponents.find(variables[i]);
        auto itB = b.exponents.find(variables[i]);
        if (itA != a.exponents.end()) 
            ea[i] = itA->second;
        if (itB != b.exponents.end()) 
            eb[i] = itB->second;
    }

    int da = 0, db = 0;
    for (int i = 0; i < n; ++i) { 
        da += ea[i]; 
        db += eb[i]; 
    }

    switch (order) {
    case MonomialOrder::LEX:
        for (int i = 0; i < n; ++i)
            if (ea[i] != eb[i]) 
                return ea[i] > eb[i];
        return false;

    case MonomialOrder::GRLEX:
        if (da != db) 
            return da > db;
        for (int i = 0; i < n; ++i)
            if (ea[i] != eb[i]) 
                return ea[i] > eb[i];
        return false;

    case MonomialOrder::GREVLEX:
        if (da != db) 
            return da > db;
        for (int i = n - 1; i >= 0; --i)
            if (ea[i] != eb[i]) 
                return ea[i] < eb[i];
        return false;

    case MonomialOrder::INVLEX:
        for (int i = n - 1; i >= 0; --i)
            if (ea[i] != eb[i]) 
                return ea[i] > eb[i];
        return false;

    case MonomialOrder::RINVLEX:
        if (da != db) 
            return da > db;
        for (int i = 0; i < n; ++i)
            if (ea[i] != eb[i]) 
                return ea[i] < eb[i];
        return false;
    }
    return false;
}


template<typename CoefType>
Monomial<CoefType> Polynomial<CoefType>::leadingMonomial(MonomialOrder order) const
{
    auto supp = support();

    if (supp.empty()) {
        MonomType zero;
        zero.coeff = CoefType(0);
        return zero;
    }

    std::sort(supp.begin(), supp.end(),
        [&](const MonomType& a, const MonomType& b) {
            return compareMonomials(a, b, variables_, order);
        });

    MonomType lm = supp[0];
    lm.coeff = CoefType(1);
    return lm;
}


template<typename CoefType>
Monomial<CoefType> Polynomial<CoefType>::leadingTerm(MonomialOrder order) const
{
    auto supp = support();

    if (supp.empty()) {
        MonomType zero;
        zero.coeff = CoefType(0);
        return zero;
    }

    std::sort(supp.begin(), supp.end(),
        [&](const MonomType& a, const MonomType& b) {
            return compareMonomials(a, b, variables_, order);
        });

    return supp[0];
}

template<typename CoefType>
CoefType Polynomial<CoefType>::leadingCoefficient(MonomialOrder order) const
{
    return leadingTerm(order).coeff;
}


template<typename CoefType>
std::vector<int> Polynomial<CoefType>::multiDegree(MonomialOrder order) const
{
    MonomType lm = leadingMonomial(order);

    std::vector<int> deg;
    deg.reserve(variables_.size());

    for (const std::string& var : variables_) {
        auto it = lm.exponents.find(var);
        if (it != lm.exponents.end())
            deg.push_back(it->second);
        else
            deg.push_back(0);
    }

    return deg;
}


template<typename CoefType>
typename Polynomial<CoefType>::DivisionResult
Polynomial<CoefType>::divideBy(const std::vector<Polynomial>& F,
    MonomialOrder order) const
{
    int s = static_cast<int>(F.size());

    for (const auto& fi : F)
        checkCompat(fi);

    DivisionResult result(variables_, s);

    Polynomial p(*this); // копия делимого

    while (!p.isZero()) {

        MonomType lt_p = p.leadingTerm(order);

        bool divided = false;

        for (int i = 0; i < s; ++i) {

            MonomType lt_fi = F[i].leadingTerm(order);

            // проверка делимости
            bool divisible = true;
            std::map<std::string, int> shift;

            for (const auto& var : variables_) {

                int ep = 0, ef = 0;

                auto itp = lt_p.exponents.find(var);
                if (itp != lt_p.exponents.end()) 
                    ep = itp->second;

                auto itf = lt_fi.exponents.find(var);
                if (itf != lt_fi.exponents.end()) 
                    ef = itf->second;

                if (ep < ef) {
                    divisible = false;
                    break;
                }

                if (ep - ef > 0)
                    shift[var] = ep - ef;
            }

            if (!divisible) //если хотя бы по одной перем не делится то след
                continue;

            // коэффициент
            CoefType coeff = poly_detail::divCoeff(lt_p.coeff, lt_fi.coeff);

            // q_i += coeff * x^shift
            result.quotients[i].addTerm(coeff, shift);

            // p -= coeff * x^shift * f_i
            for (const auto& m : F[i].support()) {

                std::map<std::string, int> newExp = shift;

                for (auto& kv : m.exponents)
                    newExp[kv.first] += kv.second; //складываем степени

                p.addTerm(-coeff * m.coeff, newExp);
            }

            divided = true;
            break;
        }

        if (!divided) {
            // в остаток
            result.remainder.addTerm(lt_p.coeff, lt_p.exponents);

            // убрать из p
            p.addTerm(-lt_p.coeff, lt_p.exponents);
        }
    }

    return result;
}
