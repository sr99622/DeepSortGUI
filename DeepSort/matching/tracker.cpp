#include <iostream>
#include "tracker.h"
#include "nn_matching.h"
#include "../feature/model.h"
#include "linear_assignment.h"

tracker::tracker(float max_cosine_distance, int nn_budget, float max_iou_distance, int max_age, int n_init)
{
    this->metric = new NearNeighborDisMetric(NearNeighborDisMetric::METRIC_TYPE::cosine, max_cosine_distance, nn_budget);
    this->max_iou_distance = max_iou_distance;
    this->max_age = max_age;
    this->n_init = n_init;

    this->kf = new KalmanFilter();
    this->tracks.clear();
    this->_next_idx = 1;
}

void tracker::predict()
{    
    for (Track& track : tracks) {
        track.predict(kf);
    }
}

void tracker::update(const DETECTIONS &detections)
{
    TRACKER_MATCHED res;
    _match(detections, res);

    std::vector<MATCH_DATA>& matches = res.matches;

    for (MATCH_DATA& data : matches) {
        int track_idx = data.first;
        int detection_idx = data.second;
        tracks[track_idx].update(this->kf, detections[detection_idx]);
    }

    std::vector<int>& unmatched_tracks = res.unmatched_tracks;
    for (int& track_idx : unmatched_tracks) {
        this->tracks[track_idx].mark_missed();
    }

    std::vector<int>& unmatched_detections = res.unmatched_detections;
    for (int& detection_idx : unmatched_detections) {
        this->_initiate_track(detections[detection_idx]);
    }

    std::vector<Track>::iterator it;
    for (it = tracks.begin(); it != tracks.end();) {

        if ((*it).is_deleted())
            it = tracks.erase(it);
        else
            ++it;
    }

    std::vector<int> active_targets;
    std::vector<TRACKER_DATA> tid_features;
    for (Track& track : tracks) {
        if (track.is_confirmed() == false)
            continue;

        active_targets.push_back(track.track_id);
        tid_features.push_back(std::make_pair(track.track_id, track.features));
        FEATURES t = FEATURES(0, 128);
        track.features = t;
    }

    this->metric->partial_fit(tid_features, active_targets);
}

void tracker::_match(const DETECTIONS &detections, TRACKER_MATCHED &res)
{
    std::vector<int> confirmed_tracks;
    std::vector<int> unconfirmed_tracks;
    int idx = 0;

    for (Track& t:tracks) {

        if (t.is_confirmed())
            confirmed_tracks.push_back(idx);
        else
            unconfirmed_tracks.push_back(idx);

        idx++;
    }

    TRACKER_MATCHED matcha = linear_assignment::getInstance()->matching_cascade(
                this,
                &tracker::gated_metric,
                this->metric->matching_threshold,
                this->max_age,
                this->tracks,
                detections,
                confirmed_tracks);

    std::vector<int> iou_track_candidates = unconfirmed_tracks;
    for (auto it = matcha.unmatched_tracks.begin(); it != matcha.unmatched_tracks.end();) {
        int idx = *it;
        if (tracks[idx].time_since_update == 1) {
            iou_track_candidates.push_back(idx);
            matcha.unmatched_tracks.erase(it);
            continue;
        }
        ++it;
    }

    /*
    size_t num_matches_a = matcha.matches.size();
    size_t num_unmatched_tracks_a = matcha.unmatched_tracks.size();
    size_t num_unmatched_detections_a = matcha.unmatched_detections.size();
    std::cout << " MATCH A"
              << " num_matches: " << num_matches_a
              << " num_unmatched_tracks: " << num_unmatched_tracks_a
              << " num_unmatched_detections: " << num_unmatched_detections_a << std::endl;
    */

    TRACKER_MATCHED matchb = linear_assignment::getInstance()->min_cost_matching(
                this,
                &tracker::iou_cost,
                this->max_iou_distance,
                this->tracks,
                detections,
                iou_track_candidates,
                matcha.unmatched_detections);

    /*
    size_t num_matches_b = matchb.matches.size();
    size_t num_unmatched_tracks_b = matchb.unmatched_tracks.size();
    size_t num_unmatched_detections_b = matchb.unmatched_detections.size();
    std::cout << " MATCH B"
              << " num_matches: " << num_matches_b
              << " num_unmatched_tracks: " << num_unmatched_tracks_b
              << " num_unmatched_detections: " << num_unmatched_detections_b << std::endl;
    */

    //get result:
    res.matches = matcha.matches;
    res.matches.insert(res.matches.end(), matchb.matches.begin(), matchb.matches.end());
    //unmatched_tracks;
    res.unmatched_tracks = matcha.unmatched_tracks;
    res.unmatched_tracks.insert(res.unmatched_tracks.end(), matchb.unmatched_tracks.begin(), matchb.unmatched_tracks.end());
    res.unmatched_detections = matchb.unmatched_detections;
}

void tracker::_initiate_track(const DETECTION_ROW &detection)
{
    KAL_DATA data = kf->initiate(detection.to_xyah());
    KAL_MEAN mean = data.first;
    KAL_COVA covariance = data.second;

    this->tracks.push_back(Track(mean, covariance, this->_next_idx, this->n_init, this->max_age, detection.feature));
    _next_idx += 1;
}

DYNAMICM tracker::gated_metric(std::vector<Track> &tracks,
                               const DETECTIONS &dets,
                               const std::vector<int>& track_indices,
                               const std::vector<int>& detection_indices)
{
    FEATURES features(detection_indices.size(), 128);
    int pos = 0;
    for (int i : detection_indices) {
        features.row(pos++) = dets[i].feature;
    }

    std::vector<int> targets;
    for (int i : track_indices) {
        targets.push_back(tracks[i].track_id);
    }

    DYNAMICM cost_matrix = this->metric->distance(features, targets);
    DYNAMICM res = linear_assignment::getInstance()->gate_cost_matrix(this->kf, cost_matrix, tracks, dets, track_indices, detection_indices);

    return res;
}

DYNAMICM tracker::iou_cost(std::vector<Track> &tracks,
                           const DETECTIONS &dets,
                           const std::vector<int>& track_indices,
                           const std::vector<int>& detection_indices)
{
    int rows = track_indices.size();
    int cols = detection_indices.size();
    DYNAMICM cost_matrix = Eigen::MatrixXf::Zero(rows, cols);

    for (int i = 0; i < rows; i++) {
        int track_idx = track_indices[i];
        if (tracks[track_idx].time_since_update > 1) {
            cost_matrix.row(i) = Eigen::RowVectorXf::Constant(cols, INFTY_COST);
            continue;
        }

        DETECTBOX bbox = tracks[track_idx].to_tlwh();
        int csize = detection_indices.size();
        DETECTBOXES candidates(csize, 4);

        for (int k = 0; k < csize; k++) {
            candidates.row(k) = dets[detection_indices[k]].tlwh;
        }

        Eigen::RowVectorXf rowV = (1. - iou(bbox, candidates).array()).matrix().transpose();
        cost_matrix.row(i) = rowV;
    }

    return cost_matrix;
}

Eigen::VectorXf tracker::iou(DETECTBOX& bbox, DETECTBOXES& candidates)
{
    float bbox_tl_1 = bbox[0];
    float bbox_tl_2 = bbox[1];
    float bbox_br_1 = bbox[0] + bbox[2];
    float bbox_br_2 = bbox[1] + bbox[3];
    float area_bbox = bbox[2] * bbox[3];

    Eigen::Matrix<float, -1, 2> candidates_tl;
    Eigen::Matrix<float, -1, 2> candidates_br;
    candidates_tl = candidates.leftCols(2) ;
    candidates_br = candidates.rightCols(2) + candidates_tl;

    int size = int(candidates.rows());

    Eigen::VectorXf res(size);
    for(int i = 0; i < size; i++) {
        float tl_1 = std::max(bbox_tl_1, candidates_tl(i, 0));
        float tl_2 = std::max(bbox_tl_2, candidates_tl(i, 1));
        float br_1 = std::min(bbox_br_1, candidates_br(i, 0));
        float br_2 = std::min(bbox_br_2, candidates_br(i, 1));

        float w = br_1 - tl_1; w = (w < 0 ? 0 : w);
        float h = br_2 - tl_2; h = (h < 0 ? 0 : h);
        float area_intersection = w * h;
        float area_candidates = candidates(i, 2) * candidates(i, 3);
        res[i] = area_intersection/(area_bbox + area_candidates - area_intersection);
    }

    return res;
}

