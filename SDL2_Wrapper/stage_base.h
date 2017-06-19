#pragma once

#include<memory>
#include "render_state.h"

namespace Render
{
	namespace Stage
	{
		class Base
		{	
		public:
			std::shared_ptr<Render::State> & mSceneState;
		protected:
			inline std::shared_ptr<Render::State> &
			sceneState()
			{
				return mSceneState;
			}
		public:
			explicit Base(std::shared_ptr<Render::State> & d) 
				: mSceneState(d) {}
			Base& operator=(const Base&) = delete;
			virtual ~Base() {}

			virtual void execute() = 0;
		};
	}
}