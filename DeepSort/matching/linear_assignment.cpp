#include "linear_assignment.h"
#include "../thirdPart/hungarianoper.h"
#include <map>

linear_assignment *linear_assignment::instance = NULL;
linear_assignment::linear_assignment()
{
}

linear_assignment *linear_assignment::getInstance()
{
    if(instance == NULL) instance = new linear_assignment();
    return instance;
}

TRACKER_MATCHED
linear_assignment::matching_cascade(
        tracker *distance_metric,
        tracker::GATED_METRIC_FUNC distance_metric_func,
        float max_distance,
        int cascade_depth,
        std::vector<Track> &tracks,
        const DETECTIONS &detections,
        std::vector<int>& track_indices)
{

    TRACKER_MATCHED result;
    std::vector<int> unmatched_detections;
    for(size_t i = 0; i < detections.size(); i++) {
        unmatched_detections.push_back(int(i));
    }

    std::vector<int> level_track_indices;

    std::map<int, int> matches_trackid;
    for (int level = 0; level < cascade_depth; level++) {
        if (unmatched_detections.size() == 0)
            break;

        level_track_indices.clear();
        for (int k : track_indices) {
            if (tracks[k].time_since_update == level + 1)
                level_track_indices.push_back(k);
        }

        if (level_track_indices.size() == 0)
            continue;

        TRACKER_MATCHED tmp = min_cost_matching(
                    distance_metric,
                    distance_metric_func,
                    max_distance,
                    tracks,
                    detections,
                    level_track_indices,
                    unmatched_detections);

        unmatched_detections = tmp.unmatched_detections;

        for (size_t i = 0; i < tmp.matches.size(); i++) {
            MATCH_DATA pa = tmp.matches[i];
            result.matches.push_back(pa);
            matches_trackid.insert(pa);
        }
    }

    result.unmatched_detections = unmatched_detections;

    for (size_t i = 0; i < track_indices.size(); i++) {
        int tid = track_indices[i];
        if (matches_trackid.find(tid) == matches_trackid.end())
            result.unmatched_tracks.push_back(tid);
    }

    return result;
}

TRACKER_MATCHED
linear_assignment::min_cost_matching(
        tracker *distance_metric,
        tracker::GATED_METRIC_FUNC distance_metric_func,
        float max_distance,
        std::vector<Track> &tracks,
        const DETECTIONS &detections,
        std::vector<int> &track_indices,
        std::vector<int> &detection_indices)
{

    TRACKER_MATCHED result;
    if ((detection_indices.size() == 0) || (track_indices.size() == 0)) {
        result.matches.clear();
        result.unmatched_tracks = track_indices;
        result.unmatched_detections = detection_indices;
        return result;
    }

    DYNAMICM cost_matrix = (distance_metric->*(distance_metric_func))(
                tracks, detections, track_indices, detection_indices);

    for (int i = 0; i < cost_matrix.rows(); i++) {
        for (int j = 0; j < cost_matrix.cols(); j++) {
            float tmp = cost_matrix(i,j);
            if (tmp > max_distance)
                cost_matrix(i,j) = max_distance + 1e-5;
        }
    }

    Eigen::Matrix<float, -1, 2, Eigen::RowMajor> indices = HungarianOper::Solve(cost_matrix);

    result.matches.clear();
    result.unmatched_tracks.clear();
    result.unmatched_detections.clear();

    for (size_t col = 0; col < detection_indices.size(); col++) {
        bool flag = false;
        for (int i = 0; i < indices.rows(); i++) {
            if (indices(i, 1) == col) {
                flag = true;
                break;
            }
        }

        if (flag == false)
            result.unmatched_detections.push_back(detection_indices[col]);
    }

    for (size_t row = 0; row < track_indices.size(); row++) {
        bool flag = false;
        for(int i = 0; i < indices.rows(); i++) {
            if(indices(i, 0) == row) {
                flag = true;
                break;
            }
        }
        if (flag == false)
            result.unmatched_tracks.push_back(track_indices[row]);
    }

    for (int i = 0; i < indices.rows(); i++) {
        int row = indices(i, 0);
        int col = indices(i, 1);

        int track_idx = track_indices[row];
        int detection_idx = detection_indices[col];
        if (cost_matrix(row, col) > max_distance) {
            result.unmatched_tracks.push_back(track_idx);
            result.unmatched_detections.push_back(detection_idx);
        }
        else {
            result.matches.push_back(std::make_pair(track_idx, detection_idx));
        }
    }

    return result;
}

DYNAMICM
linear_assignment::gate_cost_matrix(
        KalmanFilter *kf,
        DYNAMICM &cost_matrix,
        std::vector<Track> &tracks,
        const DETECTIONS &detections,
        const std::vector<int> &track_indices,
        const std::vector<int> &detection_indices,
        float gated_cost,
        bool only_position)
{
    int gating_dim = (only_position == true ? 2 : 4);
    double gating_threshold = KalmanFilter::chi2inv95[gating_dim];

    std::vector<DETECTBOX> measurements;
    for (int i : detection_indices) {
        DETECTION_ROW t = detections[i];
        measurements.push_back(t.to_xyah());
    }

    for (size_t i = 0; i < track_indices.size(); i++) {
        Track& track = tracks[track_indices[i]];

        Eigen::Matrix<float, 1, -1> gating_distance = kf->gating_distance(
                    track.mean, track.covariance, measurements, only_position);

        for (int j = 0; j < gating_distance.cols(); j++) {
            if (gating_distance(0, j) > gating_threshold)
                cost_matrix(i, j) = gated_cost;
        }
    }

    return cost_matrix;
}

