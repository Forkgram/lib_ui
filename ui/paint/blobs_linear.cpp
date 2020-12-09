// This file is part of Desktop App Toolkit,
// a set of libraries for developing nice desktop applications.
//
// For license and copyright information please follow this link:
// https://github.com/desktop-app/legal/blob/master/LEGAL
//
#include "ui/paint/blobs_linear.h"

#include "ui/painter.h"

namespace Ui::Paint {

LinearBlobs::LinearBlobs(
	std::vector<BlobData> blobDatas,
	float levelDuration,
	float levelDuration2,
	float maxLevel)
: _maxLevel(maxLevel)
, _blobDatas(std::move(blobDatas))
, _levelValue(levelDuration)
, _levelValue2(levelDuration2) {
	init();
}

void LinearBlobs::init() {
	for (const auto &data : _blobDatas) {
		auto blob = Paint::LinearBlobBezier(
			data.segmentsCount,
			data.minScale);
		blob.setRadiuses({ 0, data.minRadius });
		blob.generateBlob();
		_blobs.push_back(std::move(blob));
	}
}

float LinearBlobs::maxRadius() const {
	const auto maxOfRadiuses = [](const BlobData data) {
		return std::max(data.maxRadius, data.minRadius);
	};
	const auto max = *ranges::max_element(
		_blobDatas,
		std::less<>(),
		maxOfRadiuses);
	return maxOfRadiuses(max);
}

int LinearBlobs::size() const {
	return _blobs.size();
}

void LinearBlobs::setRadiusesAt(
		rpl::producer<LinearBlobBezier::Radiuses> &&radiuses,
		int index) {
	Expects(index >= 0 && index < size());
	std::move(
		radiuses
	) | rpl::start_with_next([=](LinearBlobBezier::Radiuses r) {
		_blobs[index].setRadiuses(std::move(r));
	}, _lifetime);
}

LinearBlobBezier::Radiuses LinearBlobs::radiusesAt(int index) {
	Expects(index >= 0 && index < size());
	return _blobs[index].radiuses();
}

void LinearBlobs::setLevel(float value) {
	const auto to = std::min(_maxLevel, value) / _maxLevel;
	_levelValue.start(to);
	_levelValue2.start(to);
}

void LinearBlobs::paint(
		Painter &p,
		const QBrush &brush,
		const QRect &rect,
		float pinnedTop,
		float progressToPinned) {
	PainterHighQualityEnabler hq(p);
	const auto opacity = p.opacity();
	for (auto i = 0; i < _blobs.size(); i++) {
		auto r = rect;
		r.setTop(r.top() - _blobDatas[i].topOffset * _levelValue2.current());

		_blobs[i].update(_levelValue.current(), _blobDatas[i].speedScale);
		const auto alpha = _blobDatas[i].alpha;
		if (alpha != 1.) {
			p.setOpacity(opacity * alpha);
		}
		_blobs[i].paint(p, brush, r, pinnedTop, progressToPinned);
		if (alpha != 1.) {
			p.setOpacity(opacity);
		}
	}
}

void LinearBlobs::updateLevel(crl::time dt) {
	const auto d = (dt > 20) ? 17 : dt;
	_levelValue.update(d);
	_levelValue2.update(d);

	for (auto i = 0; i < _blobs.size(); i++) {
		const auto &data = _blobDatas[i];
		_blobs[i].setRadiuses({
			0,
			data.minRadius + data.maxRadius * (float)currentLevel() });
	}
}

float64 LinearBlobs::currentLevel() const {
	return _levelValue.current();
}

} // namespace Ui::Paint
