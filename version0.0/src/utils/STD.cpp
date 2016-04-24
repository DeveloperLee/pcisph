#include "STD.h"


namespace cs224  {

Eigen::MatrixXf toMatrix(const std::vector<Vector3f> &data) {

    Eigen::MatrixXf ret;
    ret.resize(3, data.size());
    for (size_t i = 0; i < data.size(); ++ i) {
        ret.col(i) = data[i];
    }

    return std::move(ret);
}

}
