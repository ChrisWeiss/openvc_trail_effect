#ifndef TRAIL_HPP
#define TRAIL_HPP

#include <opencv2/opencv.hpp>
#include "Frame.hpp"

typedef std::list<Frame *> trail_t;

class Trail {
private:
	virtual void iterate(cv::Mat& frame, void (*action)(cv::Mat&, Frame *, int, int)) = 0;

	cv::Mat *background = NULL;
	bool redraw_current_frame;
	bool persistent;

	void (*drawStrategy)(cv::Mat&, Frame *, int, int);

	static void frameCopy(cv::Mat& destination, Frame *frame, int frame_index, int trail_size)
	{
		(void) frame_index;
		(void) trail_size;
		frame->copyTo(destination);
	}

	static void frameAccumulate(cv::Mat& destination, Frame *frame, int frame_index, int trail_size)
	{
		(void) frame_index;
		(void) trail_size;
		cv::accumulate(frame->getData(), destination, frame->getMask());
	}

	static void frameFadeCopy(cv::Mat& destination, Frame *frame, int frame_index, int trail_size)
	{
		double weight = ((double) frame_index) / trail_size;
		cv::Mat weighted_frame;

		frame->getData().convertTo(weighted_frame, destination.type(), weight);
		weighted_frame.copyTo(destination, frame->getMask());
	}

	static void frameFadeAccumulate(cv::Mat& destination, Frame *frame, int frame_index, int trail_size)
	{
		double weight = ((double) frame_index) / trail_size;
		cv::accumulateWeighted(frame->getData(), destination, weight, frame->getMask());
	}

	static void frameAverage(cv::Mat& destination, Frame *frame, int frame_index, int trail_size)
	{
		(void) frame_index;
		double weight = 1. / trail_size;
		cv::accumulateWeighted(frame->getData(), destination, weight, frame->getMask());
	}

protected:
	trail_t trail;

public:
	Trail(int trail_length, cv::Size frame_size)
	{
		redraw_current_frame = false;
		drawStrategy = frameCopy;
		persistent = false;

		if (trail_length < 0) {
			persistent = true;
			background = new cv::Mat(frame_size, CV_8UC3);
		}

		if (trail_length < 1)
			trail_length = 1;

		for (int i = 0; i < trail_length; i++)
			trail.push_back(new Frame(frame_size));
	}

	virtual ~Trail()
	{
		trail_t::iterator trail_it;
		for (trail_it = trail.begin(); trail_it != trail.end(); trail_it++) {
			delete(*trail_it);
		}
		trail.clear();
		delete background;
	}

	void setBackground(cv::Mat trail_background)
	{
		delete background;
		background = new cv::Mat(trail_background);
	}

	int setDrawingMethod(std::string drawing_method)
	{
		if (drawing_method == "copy") {
			drawStrategy = frameCopy;
		} else if (drawing_method == "accumulate") {
			drawStrategy = frameAccumulate;
		} else if (drawing_method == "fadecopy") {
			drawStrategy = frameFadeCopy;
		} else if (drawing_method == "fadeaccumulate") {
			drawStrategy = frameFadeAccumulate;
		} else if (drawing_method == "average") {
			drawStrategy = frameAverage;
		} else {
			return -1;
		}
		return 0;
	}

	void setRedrawCurrentFrame(bool do_redraw_current_frame)
	{
		redraw_current_frame = do_redraw_current_frame;
	}

	void update(Frame *frame)
	{
		Frame *front = trail.front();
		trail.pop_front();
		delete front;

		trail.push_back(frame);
	}

	void draw(cv::Mat& destination)
	{
		cv::Mat float_destination;

		if (background)
			background->convertTo(float_destination, CV_32FC3);
		else
			destination.convertTo(float_destination, CV_32FC3);

		iterate(float_destination, drawStrategy);
		float_destination.convertTo(destination, destination.type());

		if (redraw_current_frame) {
			Frame *current_frame = trail.back();
			current_frame->getData().copyTo(destination, current_frame->getMask());
		}

		/* remember the current trail in the background image */
		if (persistent)
			float_destination.convertTo(*background, background->type());
	}
};

class ForwardTrail: public Trail {
	void iterate(cv::Mat& destination, void (*action)(cv::Mat&, Frame *, int, int))
	{
		int index = 0;
		trail_t::iterator trail_it;
		for (trail_it = trail.begin(); trail_it != trail.end(); trail_it++) {
			action(destination, *trail_it, ++index, trail.size());
		}
	}

public:
	ForwardTrail(int trail_lenght, cv::Size frame_size) : Trail(trail_lenght, frame_size)
	{
	}
};

class ReverseTrail: public Trail {
	void iterate(cv::Mat& destination, void (*action)(cv::Mat&, Frame *, int, int))
	{
		int index = 0;
		trail_t::reverse_iterator trail_it;
		for (trail_it = trail.rbegin(); trail_it != trail.rend(); trail_it++) {
			action(destination, *trail_it, ++index, trail.size());
		}
	}

public:
	ReverseTrail(int trail_lenght, cv::Size frame_size) : Trail(trail_lenght, frame_size)
	{
	}
};

#endif // TRAIL_HPP
