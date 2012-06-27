#include <cstdio>
#include <vector>
using namespace std;

#define crFor(t,v,c) \
    for(t::const_reverse_iterator v=c.rbegin(); v != c.rend(); ++v)

typedef long long i64;

typedef vector<int> IV;

typedef IV::iterator IVi;

// The UVa version requires big integers..... *sigh*

// Big Integer
#define BIBAS 1000
#define BIDIG 3
#define BIFMT "%03d"
struct Bigint {
    IV d; bool sgn;
    Bigint(i64 n=0) {
        if (n < 0) sgn = true, n = -n; else sgn = false;
        if (n < BIBAS) d.push_back(n);
        else while (n != 0) { d.push_back(n % BIBAS); n /= BIBAS; }
    }
    size_t len() const { return d.size(); }
    Bigint &operator*=(const Bigint &b) {
        int s1 = len(), s2 = b.len(), s3 = s1+s2;
        IV res(s3); int c = 0;
        for (int k=0; k < s3; ++k) {
            int sum = c;
            for (int i=max(0,k-s2+1), I=min(k+1, s1), j=k-i; i < I; i++, j--)
                sum += d[i] * b.d[j];
            if (sum >= BIBAS) { c = sum / BIBAS; sum %= BIBAS; } else c = 0;
            res[k] = sum;
        }
        d = res; sgn ^= b.sgn; clean();
        return *this;
    }
    Bigint &operator+=(const Bigint &b) {
        int s1 = len(), s2 = b.len(), s3 = max(s1, s2) + 1;
        IV res(s3); int c = 0;
        for (int i = 0; i < s3; ++i) {
            int sum = c;
            sum += i < s1 ? d[i] : 0;
            sum += i < s2 ? b.d[i] : 0;
            if (sum >= BIBAS) { c = sum / BIBAS; sum %= BIBAS; } else c = 0;
            res[i] = sum;
        }
        d = res; clean();
        return *this;
    }
    void clean() {
        IVi i; for (i=d.end()-1; *i == 0 && i != d.begin(); i--);
        d.erase(i+1, d.end());
        if (sgn && d.size() == 1 && d[0] == 0) sgn = false;
    }
    void println() {
        if (sgn) putchar('-');
        bool flg = true;
        crFor (IV, i, d) {
            if (flg) { printf("%d", *i); flg=false; }
            else printf(BIFMT, *i);
        } putchar('\n');
    }
};


i64 N;

// Finds the most significant bit set on n. The bit is left in b, and its
// zero-indexed position in p
void msb(i64 n, i64 &b, int &p)
{
    for (b = 1, p = 0, n >>= 1; n; b <<= 1, n >>= 1, ++p);
}

// Returns the number of adjacent bits from 0 to N
Bigint f(i64 n)
{
    if (n < 3) return 0;
    if (n == 3) return 1;

    i64 b;
    int p;
    msb(n, b, p);

    Bigint ans(p - 1);
    ans *= (1LL << (p - 2));
    if (n & (b >> 1))
        ans += Bigint((n & ~b & ~(b >> 1)) + 1);

    ans += f(n & ~b);
    return ans;
}

int main()
{
    int ncase = 0;
    while (true) {
        scanf("%lld", &N);
        if (N < 0) break;

        Bigint ans = f(N);
        printf("Case %d: ", ++ncase);
        ans.println();
    }

    return 0;
}
