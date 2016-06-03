#pragma once

#include "cocos2d.h"

#include <queue>
#include <functional>
#include <chrono>

namespace ptask
{
	struct deferred {
		int frame_no;
		std::function<void(float)> callback;

		bool redeferred = false;
		float acc_dt = 0;

		bool operator < (const deferred& rhs) const {
			bool same_frame_priority = frame_no == rhs.frame_no;

			if (same_frame_priority)
				return acc_dt < rhs.acc_dt;
			else
				return frame_no > rhs.frame_no;
		}
	};

	int current_frame_no = 0;
	int delayed_count = 0;
	std::priority_queue<deferred> q;
	std::chrono::time_point<std::chrono::system_clock> last_updated;

	void _for(
		int from, int to, int slice, int interval,
		const std::function<void(int)> &callback) {

		int step = (to - from) / slice;
		int target_frame_no = current_frame_no + 1;
		for (int i = from; i < to; i += step) {
			auto df = deferred();
			df.callback = [i, step, callback](float) {
				for (int j = i; j < i + step; j++)
					callback(j);
			};
			df.frame_no = target_frame_no;
			q.push(df);

			target_frame_no += interval;
		}
	}
	void _for(
		int from, int to, int slice,
		const std::function<void(int)> &callback) {
		_for(from, to, slice, 1, callback);
	}
	void _for(
		int from, int to,
		const std::function<void(int)> &callback) {
		_for(from, to, ceil((float)(to-from) / 60), 1, callback);
	}

	float _dt(decltype(std::chrono::system_clock::now()) from) {
		return (float)std::chrono::duration_cast<std::chrono::milliseconds>(
			(std::chrono::system_clock::now() - from)).count()
			/ 1000;
	}
	void _abadon_current_frame(float acc) {
		while (
			q.empty() == false &&
			q.top().frame_no == current_frame_no)
		{
			auto top = std::move(q.top());
			q.pop();
			top.redeferred = true;
			top.frame_no++;
			top.acc_dt += acc;
			q.push(top);
		}
	}
	void _update() {
		auto begin = std::chrono::system_clock::now();
		float dt = _dt(last_updated);

		bool abandoned = false;
		bool delayed = false;
		while (
			q.empty() == false &&
			q.top().frame_no == current_frame_no)
		{
			auto &top = q.top();
			top.callback(top.acc_dt + dt + _dt(begin));
			q.pop();

			if (_dt(begin) > 1.0 / 60) {
				delayed = true;
				if (delayed_count < 11) {
					_abadon_current_frame(dt + _dt(begin));
					delayed_count++;
					abandoned = true;
					break;
				}
				else {
				}
			}
		}

		if (delayed && !abandoned)
			delayed_count = 0;
		else if(!delayed && delayed_count > 0)
			delayed_count--;

		last_updated = std::chrono::system_clock::now();
		current_frame_no++;
	}

	// cocos2d-x
	void _update_cocos2dx(float dt) {
		_update();
	}
	void _init_cocos2dx() {
		auto director = cocos2d::Director::getInstance();
		
		last_updated = std::chrono::system_clock::now();
		director->getScheduler()->schedule(
			_update_cocos2dx, nullptr, 1.0 / 10, false, "key");
	}

	void _defer_update(cocos2d::Node *node) {
		deferred df;
		df.frame_no = current_frame_no + 1;
		df.callback = [node](float dt) {
			node->update(dt);

			_defer_update(node);
		};
		q.push(df);
	}
}