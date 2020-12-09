// This file is part of Desktop App Toolkit,
// a set of libraries for developing nice desktop applications.
//
// For license and copyright information please follow this link:
// https://github.com/desktop-app/legal/blob/master/LEGAL
//
#pragma once

class Painter;

namespace Ui::Paint {

class LinearBlobBezier final {
public:
	struct Radiuses {
		float min = 0.;
		float max = 0.;

		inline bool operator==(const Radiuses &other) const {
			return (min == other.min) && (max == other.max);
		}
		inline bool operator!=(const Radiuses &other) const {
			return !(min == other.min) && (max == other.max);
		}
	};

	LinearBlobBezier(
		int n,
		float minScale,
		float minSpeed = 0,
		float maxSpeed = 0);

	void paint(
		Painter &p,
		const QBrush &brush,
		const QRect &rect,
		float pinnedTop,
		float progressToPinned);
	void update(float level, float speedScale);
	void generateBlob();

	void setRadiuses(Radiuses values);
	Radiuses radiuses() const;

private:
	struct Segment {
		float radius = 0.;
		float radiusNext = 0.;
		float progress = 0.;
		float speed = 0.;
	};

	void generateBlob(float &radius, int i);

	const int _segmentsCount;
	const float _minSpeed;
	const float _maxSpeed;
	const QPen _pen;

	std::vector<Segment> _segments;

	float64 _scale = 0;
	Radiuses _radiuses;

};

} // namespace Ui::Paint
