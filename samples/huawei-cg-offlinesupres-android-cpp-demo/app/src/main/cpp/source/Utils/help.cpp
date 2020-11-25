/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: BufferDescriptor
 */
#include "help.h"
#include "OSRPluginCommon.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace CGKit;
using namespace std;

static constexpr int MAX_WIDTH = 4096;
static constexpr int MAX_HEIGHT = 4096;

static unsigned char *ReadPPM(const string &path, int &width, int &height)
{
    ifstream fIn(path, ios::in | ios::binary);
    if (!fIn.is_open()) {
        return nullptr;
    }
    unsigned char *pixels = nullptr;
    int pixelsSize = 0;
    string line;
    if (getline(fIn, line) && getline(fIn, line)) {
        stringstream ss(line);
        if ((ss >> width) && (width > 0) && (width <= MAX_WIDTH) &&
            (ss >> height) && (height > 0) && (height <= MAX_HEIGHT) &&
            getline(fIn, line)) {
            pixelsSize = width * height * CHANNELS_RGB;
            pixels = new(std::nothrow) unsigned char[pixelsSize];
            if (pixels == nullptr) {
                fIn.close();
                return nullptr;
            }
        } else {
            fIn.close();
            return nullptr;
        }
    } else {
        fIn.close();
        return nullptr;
    }

    if (fIn.read(reinterpret_cast<char *>(pixels), pixelsSize)) {
        fIn.close();
        return pixels;
    } else {
        delete[]pixels;
        fIn.close();
        return nullptr;
    }
}

static bool WritePPM(const string &path, const unsigned char *pixels, int width, int height)
{
    if (pixels == nullptr || width <= 0 || height <= 0 ||
        width > MAX_WIDTH || height > MAX_HEIGHT) {
        return false;
    }
    ofstream fOut(path, ios::out | ios::binary);
    if (!fOut.is_open()) {
        return false;
    }
    fOut << "P6\n" << width << ' ' << height << "\n255\n";
    fOut.write(reinterpret_cast<const char *>(pixels), width * height * CHANNELS_RGB);
    fOut.close();
    return true;
}

static int Format2Channel(PixelFormat format)
{
    if (format == PIXEL_FORMAT_R8G8B8_UNORM) {
        return CHANNELS_RGB;
    } else {
        return -1;
    }
}

static PixelFormat Channel2Format(int channel)
{
    if (channel == CHANNELS_RGB) {
        return PIXEL_FORMAT_R8G8B8_UNORM;
    } else {
        return PixelFormat(PIXEL_FORMAT_MAX);
    }
}

bool CreateBuffer(BufferDescriptor &buffer, int w, int h, PixelFormat format)
{
    int c = Format2Channel(format);
    if (w <= 0 || h <= 0 || c < 0) {
        return false;
    }
    unsigned char *pixels = new(std::nothrow) unsigned char[w * h * c];
    if (pixels == nullptr) {
        return false;
    }
    buffer.addr = static_cast<void *>(pixels);
    buffer.width = w;
    buffer.height = h;
    buffer.len = w * h * c;
    buffer.format = format;
    return true;
}

bool ReadBuffer(BufferDescriptor &buffer, const string &path)
{
    int w, h;
    unsigned char *pixels = ReadPPM(path, w, h);
    if (pixels == nullptr) {
        return false;
    }
    buffer.addr = static_cast<void *>(pixels);
    buffer.width = w;
    buffer.height = h;
    buffer.len = w * h * CHANNELS_RGB;
    buffer.format = PIXEL_FORMAT_R8G8B8_UNORM;
    return true;
}

bool WriteBuffer(const BufferDescriptor &buffer, const string &path)
{
    if (WritePPM(path, static_cast<unsigned char *>(buffer.addr), buffer.width, buffer.height)) {
        return true;
    } else {
        return false;
    }
}

void DeleteBuffer(BufferDescriptor &buffer)
{
    if (buffer.addr != nullptr) {
        delete[](static_cast<unsigned char *>(buffer.addr));
    }
    buffer.addr = nullptr;
    buffer.width = buffer.height = buffer.len = 0;
    buffer.format = PIXEL_FORMAT_MAX;
}

