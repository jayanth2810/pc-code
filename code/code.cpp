// Includes
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <list>
#include <utility>
#include <vector>
using namespace std;

// Macros
#define Sc(t,v) static_cast<t>(v)
#define Zero(v) memset(v, 0, sizeof(v))
#define Iter(t,v)  t::iterator v
#define cIter(t,v) t::const_iterator v
#define For(t,v,c)   for(t::iterator v=c.begin(); v != c.end(); ++v)
#define cFor(t,v,c)  for(t::const_iterator v=c.begin(); v != c.end(); ++v)
#define crFor(t,v,c) \
    for(t::const_reverse_iterator v=c.rbegin(); v != c.rend(); ++v)

// Typedefs
typedef unsigned int u32;
typedef long long i64;
typedef vector<int> IV;
typedef pair<int, int> II;
typedef set<II> IIS;

// I/O
#define BUF 65536
struct Reader {
    char buf[BUF]; char b; int bi, bz;
    Reader() { bi=bz=0; read(); }
    void read() {
        if (bi==bz) { bi=0; bz = fread(buf, 1, BUF, stdin); }
        b = bz ? buf[bi++] : 0; }
    void skip() { while (b > 0 && b <= 32) read(); }
    // Optional methods
    u32 next_u32() {
        u32 v = 0; for (skip(); b > 32; read()) v = v*10 + b-48; return v; }
    int next_int() {
        int v = 0; bool s = false;
        skip(); if (b == '-') { s = true; read(); }
        for (; b > 32; read()) v = v*10 + b-48; return s ? -v : v; }
    bool has_next() { skip(); return b > 0; }
    void next(char *s) { for (skip(); b > 32; read()) *s++ = b; *s = 0; }
    void next_line(char *s) {
        for (; b != 10 && b != 13; read()) *s++ = b; *s = 0;
        while (b == 10 || b == 13) read(); }
    void next_real_line(char *s) {
        for (; b != 10 && b != 13; read()) *s++ = b; *s = 0;
        char p = b; read();
        if ((p == 10 && b == 13) || (p == 13 && b == 10)) read(); }
};

// Union-Find disjoint set
struct Set {
    IV s;
    void init(int n) { s.clear(); for (int i=0; i <= n; ++i) s.push_back(i); }
    int find(int i) { if (s[i] == i) return i; return s[i]=find(s[i]); }
    void merge(int i, int j) { s[find(i)] = find(j); }
};

// Graphs
typedef int w_t;
struct Graph {
    struct Edge { int v; w_t w; Edge(int V, w_t W) : v(V), w(W) {} };
    typedef list<Edge> EL;
    typedef vector<EL> ELV;
    ELV adj;
    void init(int n) { adj.clear(); adj.resize(n); }
    void add(int u, int v, w_t w) { adj[u].push_back(Edge(v, w)); }

    void primMin(ELV &g, int src) {
        IIS q;
        int n = g.size();
        IV dis(n, INF);
        BV flg(n);
        dis[src] = 0;
        q.insert(II(0, src));
        while (! q.empty()) {
            int d = q.begin()->first;
            int v = q.begin()->second;
            q.erase(q.begin());
            if (flg[v]) continue;
            flg[v] = true;
            cFor (EL, e, g[v])
                if (!flg[e->v] && e->w < dis[e->v]) {
                    if (dis[e->v] != INF) q.erase(II(dis[e->v], e->v));
                    dis[e->v] = e->w;
                    q.insert(II(dis[e->v], e->v));
                }
        }
    }
    void dijkstra(ELV &g) {
        int n = g.size();
        IIS q;
        IV dis(n, INF);
        BV flg(n);
        dis[0] = 0;
        q.insert(II(0, 0));
        while (! q.empty()) {
            IISi it = q.begin();
            int d = it->first;
            int v = it->second;
            q.erase(it);
            if (flg[v]) continue;
            flg[v] = true;
            cFor (EL, e, g[v]) {
                int d = dis[v] + e->w;
                if (!flg[e->v] && d < dis[e->v]) {
                    if (dis[e->v] != INF) q.erase(II(dis[e->v], e->v));
                    dis[e->v] = d;
                    q.insert(II(dis[e->v], e->v));
                }
            }
        }
    }
};
struct Graph {
    struct Edge {
        int u, v, w;
        Edge(int U, int V, int W) : u(U), v(V), w(W) {}
        bool operator<(const Edge &e) const { return w < e.w; }
    };
    typedef vector<Edge> EV;
    EV edges;
    void init(size_t m=16) { edges.clear(); edges.reserve(m); }
    void add(int u, int v, int w) { edges.push_back(Edge(u, v, w)); }

    // Minimum Spanning Tree
    void kruskalMST(int n, int &ans) {
        sort(edges.begin(), edges.end());
        Set uf; uf.init(n);
        int nedges = 0;
        ans = 0;
        EV mst;
        cFor (EV, e, edges) {
            if (uf.find(e->u) == uf.find(e->v)) continue;
            mst.push_back(*e);
            uf.merge(e->u, e->v);
            ans += e->w;
            if (++nedges == n - 1) break;
        }
        if (nedges < n - 1) ans = -1;
        else edges = mst;
    }

    // Bellman-Ford
    bool bellman_ford_neg_cycle(int n) {
        IV dis(n);
        for (int i = 0; i < n-1; i++)
            cFor (EV, e, edges)
                if(dis[e->u] + e->w < dis[e->v])
                    dis[e->v] = dis[e->u] + e->w;
        cFor (EV, e, edges)
            if (dis[e->u] + e->w < dis[e->v])
                return true;
        return false;
    }
};

struct Graph {
    // Shortest paths
    void floyd(int **g, int N)
    {
        for (int k = 0; k < N; k++)
            for(int i = 0; i < N; i++)
                for (int j = 0; j < N; j++) {
                    int t = g[i][k] + g[k][j];
                    if (t < g[i][j]) g[i][j] = t;
                }
    }
};

// Number Theory
#define IsComp(n)  (_c[n>>6]&(1<<((n>>1)&31)))
#define SetComp(n) _c[n>>6]|=(1<<((n>>1)&31))
namespace Num
{
    const int MAX = 1000000;  // 10^6
    const int LMT =    1000;  // sqrt(MAX)
    int _c[(MAX>>6)+1];
    IV primes;
    void primeSieve() {
        for (int i = 3; i <= LMT; i += 2)
            if (!IsComp(i)) for (int j = i*i; j <= MAX; j+=i+i) SetComp(j);
        primes.push_back(2);
        for (int i=3; i <= MAX; i+=2) if (!IsComp(i)) primes.push_back(i);
    }
    void primeFactorization(int n, IIV &f) {
        int sn = sqrt(n);
        cFor (IV, p, primes) {
            int prime = *p;
            if (prime > sn) break; if (n % prime) continue;
            int e = 0; for (; n % prime == 0; e++, n /= prime);
            f.push_back(II(prime, e));
            sn = sqrt(n);
        }
        if (n > 1) f.push_back(II(n, 1));
    }
    void eulerPhi(int a[], int N) {
        for (int i = 1; i <= N; i++) a[i] = i;
        for (int i = 2; i <= N; i += 2) a[i] = i/2;
        for (int i = 3; i <= N; i += 2)
            if (a[i] == i) {
                a[i]--; for (int j = i+i; j <= N; j += i) a[j] -= a[j]/i;
            }
    }
    int mod_pow(int _b, i64 e, int m) {
        i64 res = 1;
        for (i64 b=_b; e; e >>= 1, b = b*b%m) if (e & 1) res = res*b%m;
        return res;
    }
    int ipow(int b, int e) {
        int res = 1; for (; e; e >>= 1, b *= b) if (e & 1) res *= b;
        return res;
    }
    int gcd(int a, int b) { for (int c = a%b; c; a=b,b=c,c=a%b); return b; }
    void extEuclid(int a, int b, int &x, int &y, int &gcd) {
        x = 0; y = 1; gcd = b;
        int m, n, q, r;
        for (int u=1, v=0; a != 0; gcd=a, a=r) {
            q = gcd / a; r = gcd % a;
            m = x-u*q; n = y-v*q;
            x=u; y=v; u=m; v=n;
        }
    }
    int modInv(int n, int m) {
        int x, y, gcd;
        extEuclid(n, m, x, y, gcd);
        if (gcd == 1) return x % m;
        return 0;
    }
    // Calculates the highest exponent of prime p that divides n!
    int powDivFact(int n, int p) {
        int sd = 0; for (int N=n; N; N /= p) sd += N % p; return (n-sd)/(p-1);
    }

    // Binomial coefficients
#define BCSIZE 1000
    int bc[BCSIZE + 1][BCSIZE + 1];
    void chooseTable() {
        for (int n = 1; n <= BCSIZE; n++) { bc[n][n] = 1; bc[n][1] = n; }
        for (int n = 3; n <= BCSIZE; n++)
            for (int k = 2; k < n; k++)
                bc[n][k] = (bc[n-1][k-1] + bc[n-1][k]) % MOD;
    }
    int choose(int n, int k) {
        if (k > n) return 0;
        i64 r=1;
        for (int d = 1; d <= k; d++) {
            r *= n--;
            r /= d;
        }
        return r;
    }
}

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
    Bigint(const char *s) {
        if (*s == '-') sgn = true, s++; else sgn = false;
        for (int end = strlen(s), i = max(0, end-BIDIG); true;) {
            int n = 0; for (int j=i; j != end; j++) n = n*10 + s[j] - '0';
            d.push_back(n); if (i == 0) break;
            end = i, i = max(0, i-BIDIG);
        } clean();
    }
    size_t len() const { return d.size(); }
    Bigint &operator*=(const Bigint &b) {
        int s1 = len(), s2 = b.len(), s3 = s1+s2;
        IV res(s3); int c = 0;
        for (int k=0; k < s3; k++) {
            int sum = c;
            for (int i=max(0,k-s2+1), I=min(k+1, s1), j=k-i; i < I; i++, j--)
                sum += d[i] * b.d[j];
            if (sum >= BIBAS) { c = sum / BIBAS; sum %= BIBAS; } else c = 0;
            res[k] = sum;
        }
        d = res; sgn ^= b.sgn; clean();
        return *this;
    }
    void clean() {
        Iter(IV, i); for (i=d.end()-1; *i == 0 && i != d.begin(); i--);
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
    int toInt() {
        int res = 0, p = 1;
        for (int i=0, I=len(); i < I; i++) { res += d[i] * p; p *= BIBAS; }
        return sgn ? -res : res;
    }
    string toString() {
        char buf[BIDIG+1]; string str;
        if (sgn) str.push_back('-');
        bool flg = true;
        crFor (IV, i, d) {
            if (flg) { sprintf(buf, "%d", *i); flg=false; }
            else sprintf(buf, BIFMT, *i);
            str.append(buf);
        }
        return str;
    }
};

// Fraction
struct Fraction {
    int p, q;

    Fraction(int P, int Q) : p(P), q(Q) { simplify(); }
    Fraction(int P) : p(P), q(1) {}
    void simplify() {
        int g = gcd(p, q);
        p /= g;
        q /= g;
    }
    Fraction operator+(const Fraction &f) const {
        return Fraction(p * f.q + q * f.p, q * f.q);
    }
    Fraction operator-(const Fraction &f) const {
        return Fraction(p * f.q - q * f.p, q * f.q);
    }
    Fraction operator*(const Fraction &f) const {
        return Fraction(p * f.p, q * f.q);
    }
    Fraction operator/(const Fraction &f) const {
        return Fraction(p * f.q, q * f.p);
    }
    Fraction operator%(int m) const {
        return Fraction(p % (m*q), q);
    }
};

// Geometry
namespace Geometry {
    const double Eps = 1e-6;
    double circle_angle(double a) { return a >= 0 ? a : Pi2 + a; }
    bool eps_eq(double a, double b) { return abs(a-b) <= Eps; }
}

typedef double p_t;
struct Point {
    p_t x, y;
    Point() { x=y=0; }
    Point(p_t X, p_t Y) : x(X), y(Y) {}
    p_t distance(Point p) {
        p_t dx = p.x - x, dy = p.y - y; return sqrt(dx*dx + dy*dy);
    }
    bool operator<(const Point &p) const {
        return x < p.x || (x == p.x && y < p.y);
    }
    Point operator-(const Point &b) const { return Point(x - b.x, y - b.y); }
};
struct Vector {
    p_t x, y;
    Vector(p_t X, p_t Y) : x(X), y(Y) {}
    Vector(const Point &p) : x(p.x), y(p.y) {}
    double norm() { return sqrt(x*x + y*y); }
    double angle(const Vector &p) const {
        return Geometry::circle_angle(atan2(p.y, p.x) - atan2(y, x));
    }
    void rotate(double a) {
        p_t px = x, py = y;
        x = px*cos(a) - py*sin(a);
        y = px*sin(a) + py*cos(a);
    }
    double distance_line_point(Point a, Point p) {
        return fabs((p.x-a.x)*y - (p.y-a.y)*x) / sqrt(x*x + y*y);
    }
};
typedef vector<Point> PV;
namespace Geometry {
    p_t cross(const Point &o, const Point &a, const Point &b) {
        return (a.x-o.x)*(b.y-o.y) - (a.y-o.y)*(b.x-o.x);
    }
    void convex_hull(PV &p, PV &h) {
        // Post-cond: p.size() > 1 => h.front() == h.back()
        int n = p.size(), k = 0;
        h.resize(2*n);
        sort(p.begin(), p.end());
        for (int i = 0; i < n; ++i) {
            while (k >= 2 && cross(h[k-2], h[k-1], p[i]) <= 0) k--;
            h[k++] = p[i];
        }
        for (int i = n-2, t=k+1; i >= 0; --i) {
            while (k >= t && cross(h[k-2], h[k-1], p[i]) <= 0) k--;
            h[k++] = p[i];
        }
        h.resize(k);
    }
}

// Trie
struct Trie {
    struct Node {
        int ch[26];
        int n;
        Node() { n=0; Zero(ch); }
    };
    int sz;
    vector<Node> nodes;
    void init() { nodes.clear(); nodes.resize(1); sz = 1; }
    void insert(const char *s) {
        int idx, cur = 0;

        while (*s) {
            idx = *s - 'A';
            if (! nodes[cur].ch[idx]) {
                nodes[cur].ch[idx] = sz++;
                nodes.resize(sz);
            }

            cur = nodes[cur].ch[idx];
            ++s;
        }
    }
};