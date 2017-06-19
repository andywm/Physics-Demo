#include "UI.h"

namespace UI
{
	Statistics::Statistics()
		:
		mBallCount(0),
		mContestedCount(0),
		mOwnedCount(0),
		mTargetPhysics(0),
		mTargetNetwork(0),
		mTargetGraphics(0),
		mTimescale(0),
		mTimestepAvg(0),
		mWellForce(0),
		mElasticity(0),
		mFriction(0)
	{
		mBar = TwNewBar("Statistics");
		TwDefine(" Statistics refresh=0.2 ");

		TwAddVarRO(mBar,
			"Total",
			TW_TYPE_INT32,
			&mBallCount,
			"group='Ball Statistics'");

		TwAddVarRO(mBar,
			"Owned",
			TW_TYPE_INT32,
			&mOwnedCount,
			"group='Ball Statistics'");

		TwAddVarRO(mBar,
			"Contested",
			TW_TYPE_INT32,
			&mContestedCount,
			"group='Ball Statistics'");

		TwAddSeparator(mBar, "sep", nullptr);

		TwAddVarRO(mBar,
			"Well Force",
			TW_TYPE_FLOAT,
			&mWellForce,
			"group='Physics'");

		TwAddVarRO(mBar,
			"Timescale",
			TW_TYPE_FLOAT,
			&mTimescale,
			"group='Physics'");

		TwAddVarRO(mBar,
			"Timestep Avg",
			TW_TYPE_FLOAT,
			&mTimestepAvg,
			"group='Physics'");

		TwAddVarRO(mBar,
			"Elasticity",
			TW_TYPE_FLOAT,
			&mElasticity,
			"group='Physics'");

		TwAddVarRO(mBar,
			"Friction",
			TW_TYPE_FLOAT,
			&mFriction,
			"group='Physics'");

		TwAddSeparator(mBar, "sep2", nullptr);

		TwAddVarRO(mBar,
			"Physics ",
			TW_TYPE_INT32,
			&mTargetPhysics,
			"group='Target Freq'");

		TwAddVarRO(mBar,
			"Network",
			TW_TYPE_INT32,
			&mTargetNetwork,
			"group='Target Freq'");

		TwAddVarRO(mBar,
			"Graphics",
			TW_TYPE_INT32,
			&mTargetGraphics,
			"group='Target Freq'");
	}
}