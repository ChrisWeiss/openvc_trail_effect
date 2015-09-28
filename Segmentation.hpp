/*
 * openvc_trail_effect - experiments about video trail effects
 *
 * Copyright (C) 2015  Antonio Ospite <ao2@ao2.it>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SEGMENTATION_HPP
#define SEGMENTATION_HPP

#include <opencv2/opencv.hpp>

class Segmentation {
public:
	virtual cv::Mat getForegroundMask(const cv::Mat& frame) = 0;
	virtual ~Segmentation() {}
};

class DummySegmentation : public Segmentation {
public:
	cv::Mat getForegroundMask(const cv::Mat& frame)
	{
		return cv::Mat(frame.size(), CV_8UC1, cv::Scalar(1));
	}
};

class ThresholdSegmentation : public Segmentation {
private:
	int threshold;

public:
	ThresholdSegmentation(int threshold_value)
	{
		threshold = threshold_value;
	}

	cv::Mat getForegroundMask(const cv::Mat& frame)
	{
		cv::Mat gray_frame;
		cv::Mat frame_mask;

		cvtColor(frame, gray_frame, CV_RGB2GRAY);
		cv::threshold(gray_frame, frame_mask, threshold, 255, CV_THRESH_TOZERO);

		return frame_mask;
	}
};

/* 
 * Some background on... background subtraction can be found here:
 * http://docs.opencv.org/master/d1/dc5/tutorial_background_subtraction.html
 */

class MOG2Segmentation : public Segmentation, public cv::BackgroundSubtractorMOG2 {
public:
	MOG2Segmentation(cv::VideoCapture& inputVideo, int learning_frames) :
		cv::BackgroundSubtractorMOG2()
	{
		cv::Mat background;
		cv::Mat foreground_mask;

		for (int i = 0; i < learning_frames; i++) {
			inputVideo >> background;
			this->operator()(background, foreground_mask);
		}
	}

	cv::Mat getForegroundMask(const cv::Mat& frame)
	{
		cv::Mat foreground_mask;

		this->operator()(frame, foreground_mask, 0);
		cv::erode(foreground_mask, foreground_mask, cv::Mat());
		cv::dilate(foreground_mask, foreground_mask, cv::Mat());
		cv::threshold(foreground_mask, foreground_mask, 0, 255, CV_THRESH_OTSU);
		cv::medianBlur(foreground_mask, foreground_mask, 9);

		return foreground_mask;
	}
};

#endif // SEGMENTATION_HPP
