#include "ContactManifold.h"

namespace Physics
{
	ContactManifold::ContactManifold()
	{
		mPoints.reserve(20000);
	}

	void
	ContactManifold::add(ManifoldPoint & point)
	{
		mPoints.push_back(point);
	}

	void
	ContactManifold::clear()
	{
		mPoints.clear();
	}

	int
	ContactManifold::size() const
	{
		return mPoints.size();
	}

	ManifoldPoint &
	ContactManifold::point(int index)
	{
		return mPoints[index];
	}
}
