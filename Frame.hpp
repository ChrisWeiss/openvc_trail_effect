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

#ifndef FRAME_HPP
#define FRAME_HPP

#include <opencv2/opencv.hpp>

class Frame {
private:
	cv::Mat data;
	cv::Mat mask;

public:
	Frame(cv::Size size)
	{
		data = cv::Mat::zeros(size, CV_8UC3);
		mask = cv::Mat::zeros(size, CV_8UC1);
	}

	Frame(const cv::Mat& frame_data)
	{
		frame_data.copyTo(data);
	}

	Frame(const cv::Mat& frame_data, const cv::Mat& frame_mask)
	{
		frame_data.copyTo(data);
		frame_mask.copyTo(mask);
	}

	~Frame()
	{
	}

	void copyTo(cv::Mat& destination)
	{
		cv::Mat converted_data;

		data.convertTo(converted_data, destination.type());
		converted_data.copyTo(destination, mask);
	}

	void zero()
	{
		data.setTo(cv::Scalar(0));
		mask.setTo(cv::Scalar(0));
	}

	cv::Mat getMasked()
	{
		cv::Mat masked_frame;

		data.copyTo(masked_frame, mask);
		return masked_frame;
	}

	cv::Mat getData() { return data; }
	cv::Mat getMask() { return mask; }
};

#endif // FRAME_HPP
