#ifndef NN_MATCHING_H
#define NN_MATCHING_H

#include "../feature/dataType.h"

#include <map>

class NearNeighborDisMetric
{
public:
    enum METRIC_TYPE{euclidean=1, cosine};
    NearNeighborDisMetric(METRIC_TYPE metric, float matching_threshold, int budget);
    DYNAMICM distance(const FEATURES& features, const std::vector<int> &targets);
    void partial_fit(std::vector<TRACKER_DATA>& tid_feats, std::vector<int>& active_targets);
    float mating_threshold;

private:
    typedef Eigen::VectorXf (NearNeighborDisMetric::*PTRFUN)(const FEATURES&, const FEATURES&);

    Eigen::VectorXf _nncosine_distance(const FEATURES& x, const FEATURES& y);
    Eigen::VectorXf _nneuclidean_distance(const FEATURES& x, const FEATURES& y);
    Eigen::MatrixXf _pdist(const FEATURES& x, const FEATURES& y);
    Eigen::MatrixXf _cosine_distance(const FEATURES& a, const FEATURES& b, bool data_is_normalized = false);

private:
    PTRFUN _metric;
    int budget;
    std::map<int, FEATURES> samples;

};

#endif // NN_MATCHING_H
