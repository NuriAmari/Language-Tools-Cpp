//
// Created by Nuri Amari on 2020-07-04.
//

#ifndef CPP_UTILS_H
#define CPP_UTILS_H
#include <unordered_set>
#include <functional>
#include <vector>
#include <algorithm>
#include <boost/container_hash/hash.hpp>

template <class T>
struct UnorderedSetHasher {
    std::size_t operator()(const std::unordered_set<T>& setToHash) const {
        std::vector<T> keys;
        for (const T& key : setToHash) {
            keys.push_back(key);
        }
        std::sort(keys.begin(), keys.end());
        return boost::hash_range(keys.begin(), keys.end());
    }
};


#endif //CPP_UTILS_H
