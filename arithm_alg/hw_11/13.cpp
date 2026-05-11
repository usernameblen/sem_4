#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

typedef boost::multiprecision::cpp_int cpp_int;
typedef boost::multiprecision::cpp_dec_float_50 cpp_dec_float_50;

//бинарное возведение в степень
cpp_int int_pow(cpp_int base, int exp, unsigned long long& ops) {
    cpp_int result = 1;
    cpp_int b = base;
    int e = exp;
    while (e > 0) {
        if (e & 1) {
            result *= b; 
            ++ops;
        }

        b *= b; 
        ++ops;
        e >>= 1;
    }
    return result;
}

cpp_dec_float_50 dec_pow(cpp_dec_float_50 base, int exp, unsigned long long& ops) {
    cpp_dec_float_50 result = 1;
    cpp_dec_float_50 b = base;
    int e = exp;
    while (e > 0) {
        if (e & 1) { 
            result *= b; 
            ++ops; 
        }

        b *= b; 
        ++ops;
        e >>= 1;
    }
    return result;
}

cpp_dec_float_50 binom_float(int n, int k, unsigned long long& ops) {
    if (k < 0 || k > n) 
        return cpp_dec_float_50(0);

    if (k == 0 || k == n) 
        return cpp_dec_float_50(1);

    if (k > n - k) 
        k = n - k; //(n, k) = (n, n-k)

    cpp_dec_float_50 result = 1;
    for (int i = 0; i < k; ++i) {
        result *= cpp_dec_float_50(n - i); 
        ++ops;

        result /= cpp_dec_float_50(i + 1); 
        ++ops;
    }
    return result;
}

void bernoulli_numbers(int p, std::vector<cpp_dec_float_50>& B, unsigned long long& ops) {
    B.resize(p + 1);
    B[0] = cpp_dec_float_50(1);

    for (int j = 1; j <= p; ++j) {

        cpp_dec_float_50 s = 0;
        for (int k = 1; k <= j; ++k) {
            unsigned long long lo = 0;

            cpp_dec_float_50 bv = binom_float(j + 1, k + 1, lo);
            ops += lo;

            s += bv * B[j - k]; 
            ops += 2;
        }
        B[j] = -s / cpp_dec_float_50(j + 1); 
        ops += 2;
    }
}

cpp_int sum_direct(int n, int p, unsigned long long& ops) {
    cpp_int result = 0;
    for (int k = 1; k <= n; ++k) {
        result += int_pow(cpp_int(k), p, ops); 
        ++ops;
    }
    return result;
}

cpp_int sum_asymptotic(int n, int p, unsigned long long& ops) {
    cpp_dec_float_50 np1 = dec_pow(cpp_dec_float_50(n), p + 1, ops); //n^(p+1)
    cpp_dec_float_50 np  = dec_pow(cpp_dec_float_50(n), p,     ops);//n^p

    cpp_dec_float_50 t1  = np1 / cpp_dec_float_50(p + 1); //n^(p+1)/(p+1)
    ++ops;

    cpp_dec_float_50 t2  = np  / cpp_dec_float_50(2); //n^p/2
    ++ops;

    cpp_dec_float_50 res = t1 + t2;
    ++ops;

    return cpp_int(res + cpp_dec_float_50("0.5"));
}

cpp_int sum_bernoulli(int n, int p, unsigned long long& ops) {
    std::vector<cpp_dec_float_50> B;
    bernoulli_numbers(p, B, ops);

    cpp_dec_float_50 dn = cpp_dec_float_50(n);
    cpp_dec_float_50 s = 0;

    for (int j = 0; j <= p; ++j) {
        cpp_dec_float_50 bv = binom_float(p + 1, j, ops);
        cpp_dec_float_50 npower = dec_pow(dn, p + 1 - j, ops);
        cpp_dec_float_50 term = bv * B[j] * npower; 
        ops += 2;

        s += term; 
        ++ops;
    }
    //cpp_dec_float_50 corr = binom_float(p + 1, 1, ops) * dec_pow(dn, p, ops);
    cpp_dec_float_50 corr = cpp_dec_float_50(p+1) * dec_pow(dn, p, ops); //поправка из за B1 = 1/2 (дб -1/2)
    ops += 2;

    s += corr;
    ops += 1;

    s /= cpp_dec_float_50(p + 1); 
    ++ops;

    return cpp_int(s + cpp_dec_float_50("0.5"));
}

int main() {
    int n_min = 1, n_max = 100, step_n = 1;
    int p_fixed = 5, n_fixed = 100, p_min = 1, p_max = 10;

    
    std::ofstream fout1("fixed_p.csv");
    fout1 << "n,ops_direct,ops_asympt,ops_bernoulli,value_direct,value_asympt,value_bernoulli\n";
    for (int n = n_min; n <= n_max; n += step_n) {
        unsigned long long o1=0, o2=0, o3=0;
        cpp_int v1 = sum_direct(n, p_fixed, o1);
        cpp_int v2 = sum_asymptotic(n, p_fixed, o2);
        cpp_int v3 = sum_bernoulli(n, p_fixed, o3);
        fout1 << n << "," << o1 << "," << o2 << "," << o3 << ","
             << v1.str() << "," << v2.str() << "," << v3.str() << "\n";
    }
    

    std::ofstream fout2("fixed_n.csv");
    fout2 << "p,ops_direct,ops_asympt,ops_bernoulli,value_direct,value_asympt,value_bernoulli\n";
    for (int p = p_min; p <= p_max; ++p) {
        unsigned long long o1=0, o2=0, o3=0;
        cpp_int v1 = sum_direct(n_fixed, p, o1);
        cpp_int v2 = sum_asymptotic(n_fixed, p, o2);
        cpp_int v3 = sum_bernoulli(n_fixed, p, o3);
        fout2 << p << "," << o1 << "," << o2 << "," << o3 << ","
             << v1.str() << "," << v2.str() << "," << v3.str() << "\n";
    }
    

    std::cout << "Done. Files: fixed_p.csv, fixed_n.csv\n";
    std::cout << "Parameters: p_fixed=" << p_fixed
              << ", n=[" << n_min << ".." << n_max << "] step=" << step_n
              << "; n_fixed=" << n_fixed
              << ", p=[" << p_min << ".." << p_max << "]\n";

    std::cout << "\nVerification:\n";
    unsigned long long tmp = 0;
    std::cout << "  S(5,3)  direct  " << sum_direct(5,3,tmp).str() << " (expect 225)\n"; tmp=0;
    std::cout << "  S(5,3)  bern    " << sum_bernoulli(5,3,tmp).str() << " (expect 225)\n"; tmp=0;
    std::cout << "  S(5,1)  direct  " << sum_direct(5,1,tmp).str() << " (expect 15)\n"; tmp=0;
    std::cout << "  S(5,1)  bern    " << sum_bernoulli(5,1,tmp).str() << " (expect 15)\n"; tmp=0;
    std::cout << "  S(10,2) direct  " << sum_direct(10,2,tmp).str() << " (expect 385)\n"; tmp=0;
    std::cout << "  S(10,2) bern    " << sum_bernoulli(10,2,tmp).str() << " (expect 385)\n"; tmp=0;
    std::cout << "  S(5,3)  asympt  " << sum_asymptotic(5,3,tmp).str() << " (expect 225)\n";
    return 0;
}