// Union-Find disjoint set
struct Set {
    IV s;
    Set(int n) { for (int i=0; i < n; ++i) s.push_back(i); }
    int find(int i) { if (s[i] == i) return i; return s[i]=find(s[i]); }
    void merge(int i, int j) { s[find(i)] = find(j); }
};

//
// Graphs - Basic structures
//

// Adjacency lists
struct Graph {
    struct Edge { int v; Edge(int V) : v(V) {} };
    typedef list<Edge> EL;
    typedef vector<EL> ELV;
    ELV adj; int n;
    Graph(int N) : n(N) { adj.resize(n); }
    void add(int u, int v) { adj[u].push_back(Edge(v)); }
};

// Weighted adjacency lists
typedef int w_t;
struct Graph {
    struct Edge { int v; w_t w; Edge(int V, w_t W) : v(V), w(W) {} };
    typedef list<Edge> EL;
    typedef vector<EL> ELV;
    ELV adj; int n;
    Graph(int N) : n(N) { adj.resize(n); }
    void add(int u, int v, w_t w) { adj[u].push_back(Edge(v, w)); }
};

// List of edges
struct Graph {
    struct Edge {
        int u, v; w_t w;
        Edge(int U, int V, w_t W) : u(U), v(V), w(W) {}
        bool operator<(const Edge &e) const { return w < e.w; }
    };
    typedef vector<Edge> EV;
    EV edges;
    void add(int u, int v, w_t w) { edges.push_back(Edge(u, v, w)); }
};

//
// Graphs - Algorithms
//
struct Graph {
    int prim_mst(int src) {
        IIS q;
        IV dis(n, INF);
        BV flg(n);
        dis[src] = 0;
        q.insert(II(0, src));
        int mst = 0;
        while (! q.empty()) {
            int d = q.begin()->first;
            int v = q.begin()->second;
            q.erase(q.begin());
            if (flg[v]) continue;
            flg[v] = true;
            mst += d;
            For (EL, e, adj[v])
                if (!flg[e->v] && e->w < dis[e->v]) {
                    dis[e->v] = e->w;
                    q.insert(II(dis[e->v], e->v));
                }
        }
        return mst;
    }
    void dijkstra(int src, IV &dis) {
        set<II> q;
        dis = IV(n, INF);
        BV vis(n);
        q.insert(II(0, src));
        dis[src] = 0;
        while (! q.empty()) {
            II p = *(q.begin()); q.erase(q.begin());
            int d = p.first, v = p.second;
            if (vis[v]) continue;
            vis[v] = true;
            For (EL, e, adj[v]) {
                int d2 = d + e->w;
                if (d2 < dis[e->v]) {
                    dis[e->v] = d2;
                    q.insert(II(d2, e->v));
                }
            }
        }
    }
    void topo_sort(IV &in, IV &order) {
        IQ q;
        for (int i = 0; i < n; ++i) if (in[i] == 0) q.push(i);
        order.clear();
        while (! q.empty()) {
            int v = q.front(); q.pop();
            order.push_back(v);
            For (EL, e, adj[v])
                if (--in[e->v] == 0) q.push(e->v);
        }
    }

    // Kosaraju's algorithm
    struct Kos {
        Graph &g; IVV sccs; IV scc; IK vs; BV vis; ELV radj;
        Kos(Graph &G) : g(G) { vis = BV(g.n); radj.resize(g.n); }
        void dfs(int v) {
            vis[v] = true;
            For (EL, ep, g.adj[v]) {
                Edge e = *ep;
                int u = e.v; e.v = v;
                radj[u].push_back(e);
                if (! vis[u]) dfs(u);
            }
            vs.push(v);
        }
        void dfs2(int v) {
            vis[v] = true;
            scc.push_back(v);
            For (EL, e, radj[v]) if (! vis[e->v]) dfs2(e->v);
        }
    };
    void kosaraju_scc(IVV &sccs) {
        Kos k(*this);
        for (int v=0; v<n; ++v) if (! k.vis[v]) k.dfs(v);
        k.vis = BV(n);
        while (! k.vs.empty()) {
            int v = k.vs.top(); k.vs.pop();
            if (k.vis[v]) continue;
            k.scc = IV();
            k.dfs2(v);
            k.sccs.push_back(k.scc);
        }
        sccs = k.sccs;
    }
    // Tarjan
    struct Tarjan {
        IVV sccs; IS s; BV flg; IV low, idx; int cnt;
        Tarjan(int n) {
            flg = BV(n);
            low = IV(n);
            idx = IV(n);
            cnt = 1;
        }
    };
    void tarjan_scc(IVV &sccs) {
        Tarjan t(n);
        for (int i = 0; i < n; ++i)
            if (t.low[i] == 0) tarjan_visit(i, t);
        sccs = t.sccs;
    }
    void tarjan_visit(int v, Tarjan &t) {
        t.idx[v] = t.low[v] = t.cnt++;
        t.s.push(v); t.flg[v] = true;
        cFor (EL, e, adj[v]) {
            if (t.low[e->v] == 0) {
                tarjan_visit(e->v, t);
                t.low[v] = min(t.low[v], t.low[e->v]);
            }
            else if (t.flg[e->v])
                t.low[v] = min(t.low[v], t.idx[e->v]);
        }
        if (t.low[v] != t.idx[v]) return;
        IV scc;
        for (int u=-1; u != v;) {
            u=t.s.top(); t.s.pop();
            t.flg[u]=false;
            scc.push_back(u);
        }
        t.sccs.push_back(scc);
    }
    void scc_to_dag(const IVV &sccs, Graph &dag) {
        int ndag = sccs.size();
        IV vcomp(n);
        for (int i=0; i < ndag; ++i)
            for (int j=0, lim=sccs[i].size(); j < lim; ++j)
                vcomp[sccs[i][j]] = i;
        dag = Graph(ndag);
        for (int u=0; u < n; u++)
            cFor (EL, e, adj[u])
                if (vcomp[u] != vcomp[e->v])
                    dag.add(vcomp[u], vcomp[e->v], 0);
    }
    // Hopcroft-Karp for bipartite matching. Receives the vertices in G1,
    // and depends on vertex #0 being reserved as the NIL vertex
    struct HK {
        IV pair_g1, pair_g2, dist;
        Graph &g;
        IV &g1;
        HK(Graph &G, IV &G1) : g(G), g1(G1) {
            pair_g1 = IV(g.n); pair_g2 = IV(g.n); dist = IV(g.n); }
        bool bfs() {
            IQ q;
            cFor (IV, v, g1) {
                if (pair_g1[*v] == 0) {
                    dist[*v] = 0;
                    q.push(*v);
                }
                else
                    dist[*v] = INF;
            }
            dist[0] = INF;
            while (! q.empty()) {
                int v = q.front(); q.pop();
                cFor (EL, e, g.adj[v]) {
                    int p = pair_g2[e->v];
                    if (dist[p] != INF) continue;
                    dist[p] = dist[v] + 1;
                    q.push(p);
                }
            }
            return dist[0] != INF;
        }
        bool dfs(int v) {
            if (v == 0) return true;
            cFor (EL, e, g.adj[v]) {
                int p = pair_g2[e->v];
                if (dist[p] == dist[v] + 1 && dfs(p)) {
                    pair_g2[e->v] = v;
                    pair_g1[v] = e->v;
                    return true;
                }
            }
            dist[v] = INF;
            return false;
        }
    };
    int hopcroft(IV &g1) {
        HK hk(*this, g1);
        int m = 0;
        while (hk.bfs())
            cFor (IV, v, g1)
                if (hk.pair_g1[*v] == 0 && hk.dfs(*v))
                    ++m;
        return m;
    }
    // Articulations/bridges
    struct Artic {
        IV low, idx;
        BV is_art;
        IIV bridges;
        int cnt;
        Graph &g;
        Artic(Graph &G) : g(G) {
            low = IV(g.n);
            idx = IV(g.n);
            is_art = BV(g.n);
            cnt = 1;
        }
        void dfs(int u, int v) {
            int children = 0;
            low[v] = idx[v] = cnt++;
            For (EL, e, g.adj[v]) {
                if (idx[e->v] == 0) {
                    ++children;
                    dfs(v, e->v);
                    low[v] = min(low[v], low[e->v]);
                    if (low[e->v] >= idx[v] && u != v)
                        is_art[v] = true;
                    if (low[e->v] == idx[e->v])
                        bridges.push_back(II(v, e->v));
                }
                else if (e->v != u)
                    low[v] = min(low[v], idx[e->v]);
            }
            if (u == v && children > 1)
                is_art[v] = true;
        }
    };
    void articulations(IIV &bridges) {
        Artic a(*this);
        for (int i = 0; i < n; ++i)
            if (a.idx[i] == 0) a.dfs(i, i);
        bridges = a.bridges;
    }
    // Eulerian Trail
    struct Euler {
        ELV adj; IV t;
        Euler(ELV Adj) : adj(Adj) {}
        void build(int u) {
            while(! adj[u].empty()) {
                int v = adj[u].front().v;
                adj[u].erase(adj[u].begin());
                build(v);
            }
            t.push_back(u);
        }
    };
    bool eulerian_trail(IV &trail) {
        Euler e(adj);
        int odd = 0, s = 0;
        /*
        for (int v = 0; v < n; v++) {
            int diff = abs(in[v] - out[v]);
            if (diff > 1) return false;
            if (diff == 1) {
                if (++odd > 2) return false;
                if (out[v] > in[v]) start = v;
            }
        }
        */
        e.build(s);
        reverse(e.t.begin(), e.t.end());
        trail = e.t;
        return true;
    }

    // Minimum Spanning Tree
    void kruskal_mst(int n, int &ans) {
        sort(edges.begin(), edges.end());
        Set uf(n);
        int nedges = 0;
        ans = 0;
        EV mst;
        For (EV, e, edges) {
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
            For (EV, e, edges)
                if(dis[e->u] + e->w < dis[e->v])
                    dis[e->v] = dis[e->u] + e->w;
        For (EV, e, edges)
            if (dis[e->u] + e->w < dis[e->v])
                return true;
        return false;
    }

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
}

//
// 2-SAT
//
struct TwoSat {
    Graph g;
    int n;
    TwoSat(int N) : n(N) { g = Graph(2*N); }
    void add_cons(int a, bool ta, int b, bool tb) {
        int p = val(a, ta), q = val(b, tb);
        g.add(neg(p), q); g.add(neg(q), p);
    }
    int val(int v, bool t) { return 2*v + (t ? 0 : 1); }
    int neg(int p) { return p ^ 1; }
    bool solve(IV &sol) {
        IVV sccs;
        g.kosaraju_scc(sccs);
        IV vscc(n);
        sol.clear();
        for (int i = 0, I = sccs.size(); i < I; ++i) {
            for (int j=0, J=sccs[i].size(); j < J; ++j) {
                int p = sccs[i][j];
                int v = p/2;
                if (vscc[v] == i+1) return false;
                if (vscc[v] != 0) break;
                vscc[v] = i+1;
                if (p & 1) sol.push_back(v);
            }
        }
        return true;
    }
};
