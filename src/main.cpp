#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/aruco.hpp>

#include "draw.h"

#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>

using namespace emscripten;
using namespace cv;
using namespace cv::aruco;

struct MarkerEntry {
    int id = 0;
    float size = 0.f;
    float x = 0.f, y = 0.f, z = 0.f;
    float yaw = 0.f, pitch = 0.f, roll = 0.f;
};

static std::vector<MarkerEntry> parseMap(const std::string& text) {
    std::vector<MarkerEntry> markers;
    std::istringstream iss(text);
    std::string line;

    while (std::getline(iss, line)) {
        auto start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        if (line[start] == '#') continue;

        std::istringstream ls(line.substr(start));
        MarkerEntry m;
        if (!(ls >> m.id >> m.size >> m.x >> m.y >> m.z)) continue;
        ls >> m.yaw >> m.pitch >> m.roll;
        if (m.size <= 0.f) continue;
        markers.push_back(m);
    }
    return markers;
}

static std::vector<Point3f> markerCorners(const MarkerEntry& m) {
    const float h = m.size * 0.5f;
    // TL, TR, BR, BL
    Point3f local[4] = {
        {-h,  h, 0.f},
        { h,  h, 0.f},
        { h, -h, 0.f},
        {-h, -h, 0.f}
    };

    const float c = std::cos(m.yaw);
    const float s = std::sin(m.yaw);

    std::vector<Point3f> out(4);
    for (int i = 0; i < 4; ++i) {
        float lx = local[i].x;
        float ly = local[i].y;
        out[i] = Point3f(c * lx - s * ly + m.x,
                         s * lx + c * ly + m.y,
                         m.z);
    }
    return out;
}

/**
 * @param mapText       содержимое map.txt
 * @param dictionaryId  id словаря OpenCV
 * @param imageWidth    ширина картинки
 * @param imageHeight   высота картинки
 * @param marginSize    отступ
 * @param drawAxis      рисовать оси
 */
val generateMapImage(
    const std::string& mapText,
    int dictionaryId,
    int imageWidth,
    int imageHeight,
    int marginSize,
    bool drawAxis
) {
    try {
        auto markers = parseMap(mapText);
        if (markers.empty() || imageWidth <= 0 || imageHeight <= 0) {
            return val::null();
        }

        SimpleBoard board;
        board.dictionary = makePtr<Dictionary>(getPredefinedDictionary(dictionaryId));

        for (const auto& m : markers) {
            board.objPoints.push_back(markerCorners(m));
            board.ids.push_back(m.id);
        }

        Mat image;
        _drawPlanarBoard(&board, Size(imageWidth, imageHeight), image,
                         marginSize, 1, drawAxis);

        Mat rgba;
        if (image.channels() == 1) {
            cvtColor(image, rgba, COLOR_GRAY2RGBA);
        } else if (image.channels() == 3) {
            cvtColor(image, rgba, COLOR_BGR2RGBA);
        } else {
            rgba = image;
        }

        size_t nbytes = rgba.total() * rgba.elemSize();
        val data = val::global("Uint8Array").new_(nbytes);
        data.call<void>("set", val(typed_memory_view(nbytes, rgba.data)));

        val result = val::object();
        result.set("data", data);
        result.set("width", imageWidth);
        result.set("height", imageHeight);
        return result;

    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV: " << e.what() << std::endl;
        return val::null();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return val::null();
    }
}

EMSCRIPTEN_BINDINGS(web_gen_arucomap) {
    function("generateMapImage", &generateMapImage);
}