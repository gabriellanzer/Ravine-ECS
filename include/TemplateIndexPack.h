#ifndef TEMPLATEINDEXPACK_H
#define TEMPLATEINDEXPACK_H

// Indexing pack structs
template <int...> struct seq
{
};

template <int N, int... S> struct gens : gens<N - 1, N - 1, S...>
{
};

template <int... S> struct gens<0, S...>
{
    typedef seq<S...> type;
};

#endif