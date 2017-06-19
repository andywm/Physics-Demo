#include <memory>
#include <vector>
#include "geometry_stage.h"
#include "render_state.h"
//#include "scene_graph.h"
//#include "v_isVisible.h"

namespace Render
{
	namespace Stage
	{
		Geometry::Geometry(std::shared_ptr<Render::State> & d)
			:
			Base(d)
		{}

		void 
		Geometry::execute()
		{
			auto camera = mSceneState->scene()->camera();
			auto geoList = mSceneState->scene()->balls();
			auto terList = mSceneState->scene()->terrain();
			auto peerList = mSceneState->scene()->peers();

			sceneState()->context()->pipeline().projection(camera.projection());
			sceneState()->context()->pipeline().view(camera.view());
			

			sceneState()->context()->execute(ACTION::COMMIT_VP);
			sceneState()->context()->execute(ACTION::COMMIT_SHADER);

			for (auto & ter : terList)
			{
				//auto s = mSceneState->context()->pipeline().state();
				sceneState()->context()->pipeline().model(ter.computeTransform());
				//set the geometry and instruct the context to draw it.
				sceneState()->context()->pipeline().geometry(ter.mesh());
				sceneState()->context()->pipeline().material(ter.material());
				sceneState()->context()->pipeline().texture(ter.texture());
				sceneState()->context()->execute(ACTION::GEOMETRY);
			}
			for (auto & geo : geoList)
			{
				if (geo.visible())
				{
					//auto s = mSceneState->context()->pipeline().state();
					sceneState()->context()->pipeline().model(geo.computeTransform());
					//set the geometry and instruct the context to draw it.
					sceneState()->context()->pipeline().geometry(geo.mesh());
					sceneState()->context()->pipeline().material(geo.material());
					sceneState()->context()->pipeline().texture(geo.texture());
					sceneState()->context()->execute(ACTION::GEOMETRY);
				}
			}	
			for (auto & peer : peerList)
			{
				//auto s = mSceneState->context()->pipeline().state();
				sceneState()->context()->pipeline().model(peer.computeTransform());
				//set the geometry and instruct the context to draw it.
				sceneState()->context()->pipeline().geometry(peer.mesh());
				sceneState()->context()->pipeline().material(peer.material());
				sceneState()->context()->pipeline().texture(peer.texture());
				sceneState()->context()->execute(ACTION::GEOMETRY);
			}
		}
	}
}