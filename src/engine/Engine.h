#ifndef __DX_PCISPH_FLUID_SIMULATOR_ENGINE__
#define __DX_PCISPH_FLUID_SIMULATOR_ENGINE__

#include <utils/STD.h>
#include <memory>

class Engine {
public:
	struct RenderSettings {
		bool showDomain = true;
		bool showFluidParticles = true;
		bool showFluidMesh = false;
		bool showBoundaryParticles = false;
		bool showBoundaryMeshes = true;
		bool showDebug = true;
	};

	Engine(NVGcontext *ctx, const Vector2i &size, const Vector2i &renderSize);

	const Camera &camera() const { return m_camera; }
		  Camera &camera()       { return m_camera; }

	const RenderSettings &renderSettings() const { return m_renderSettings; }
		  RenderSettings &renderSettings()       { return m_renderSettings; }

	const Scene &scene() const { return m_scene; }

	void loadScene(const std::string &path);
	void update(float dt);
	void updateStep();
	float time() const;

	const Mesh &fluidMesh() const { return m_fluidMesh; }
	void createFluidMesh();
	void clearFluidMesh();

	void render();

private:
	void renderDebugOverlay();

	NVGcontext *m_ctx;
	Vector2i m_size;
	Vector2i m_renderSize;
	renderSettings m_renderSettings;

	Camera m_camera;
	Scene m_scene;
	Mesh m_fluidMesh;

	std::unique_ptr<GridPainter> m_gridPainter;
	std::unique_ptr<BoxPainter> m_boxPainter;
	std::unique_ptr<ParticleSpherePainter> m_particleSpherePainter;
	std::unique_ptr<ParticleNormalPainter> m_particleNormalPainter;
	std::unique_ptr<MeshPainter> m_fluidMeshPainter;
	std::vector<std::unique_ptr<MeshPainter>> m_boundaryMeshPainters;
};

#endif




