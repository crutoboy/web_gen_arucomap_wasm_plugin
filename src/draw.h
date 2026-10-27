#pragma once

#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>
#include <vector>

// Простая структура как старый cv::aruco::Board (публичные поля)
struct SimpleBoard {
    std::vector<std::vector<cv::Point3f>> objPoints;
    cv::Ptr<cv::aruco::Dictionary> dictionary;
    std::vector<int> ids;
};

void _drawPlanarBoard(SimpleBoard* board, cv::Size outSize, cv::OutputArray img,
                      int marginSize, int borderBits, bool drawAxis);