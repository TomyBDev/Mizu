#pragma once

#include <Graphics/RenderTexture.h>

class InputManager;

class Application
{
public:
	Application(InputManager* input, class Graphics* gfx);
	~Application();

	void Update(float dt);
	void Render();
private:
	void Init();

	void HandleInput(float dt);

	void SolverPass(float dt);

	void SetRenderTexturePass(std::unique_ptr<RenderTexture>& renderTexture, ID3D11ShaderResourceView* srv);

	void Imgui();
	void Restart();

	InputManager* inputManager;
	Graphics* graphics;
	class Camera* camera;

	// Mesh
	class PlaneMesh* planeMesh;
	class OrthoMesh* orthoMesh;
	class CubeMesh* cubeMesh;
	class MaterialObject* model;
	class TextureObject* palmTree;
	MaterialObject* onsen;

	// Shaders
	class TextureShader* textureShader;
	class SolverShader* solverShader;
	class SolverShader2* solverShader2;
	class WaterShader* waterShader;
	class MaterialObjectShader* materialObjectShader;
	class TextureObjectShader* textureObjectShader;
	class SkyShader* skyShader;

	// Textures
	class Texture* startingConditionTexture;
	class TextureCube* skyTextureCube;
	Texture* palmTreeDiffuse;
	Texture* palmTreeNormal;
	Texture* palmTreeSpecular;
	class TextureArray* onsenDiff;
	class TextureArray* onsenBump;

	// Render Textures
	std::unique_ptr<RenderTexture> pass1RenderTexture;
	std::unique_ptr<RenderTexture> pass2RenderTexture;
	std::unique_ptr<RenderTexture> oldRenderTexture;

	// Sound
	class Sound* ambient;

	// Lights
	DirectionalLight light;

	// Matrices
	DirectX::XMMATRIX waterScale;

	// Frame rate to be displayed
	float frameRate = 0;

	// Camera Control
	float cameraSpeed = 10.f;

	// Water Resolution Control
	int resolution;
	int resolutionItem = 0;
	const std::unordered_map<int, int> resolutions = { {0,128}, {1,256}, {2,512}, {3,1024} };

	// Water Shader Control
	float strength = 145.f;
	float shallowColor[4] = { 0.1f, 0.37f, 0.55f, 0.85f };
	float deepColor[4] = { 0.004f, 0.15f, 0.3f, 1.f };
};