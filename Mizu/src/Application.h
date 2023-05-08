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

	void LWSolverPass(float dt);
	void MCSolverPass(float dt);

	void SetRenderTexturePass(std::unique_ptr<RenderTexture>& renderTexture, ID3D11ShaderResourceView* srv);

	void Imgui();

	enum Solver { LaxWendroff, MacCormack };

	InputManager* inputManager;
	Graphics* graphics;
	class Camera* camera;

	// Mesh
	class PlaneMesh* planeMesh;
	class OrthoMesh* orthoMesh;
	class CubeMesh* cubeMesh;
	class MaterialObject* onsen;

	// Shaders
	class TextureShader* textureShader;
	class SolverShader* LW1SolverShader; //Lax Wendroff Step Solver
	class SolverShader2* LW2SolverShader; //Lax Wendroff Full Step Solver
	SolverShader* MC1SolverShader; //MacCormack Half Step Solver
	SolverShader2* MC2SolverShader; //MacCormack Full Step Solver
	class WaterShader* waterShader;
	class MaterialObjectShader* materialObjectShader;
	class SkyShader* skyShader;

	// Textures
	class Texture* startingConditionTexture;
	class TextureCube* skyTextureCube;
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
	std::vector<DirectX::XMMATRIX> waterMats;

	// Frame rate to be displayed
	float frameRate = 0;
	float timeElapsed = 0;

	// Camera Control
	float cameraSpeed = 10.f;

	//Solver Control
	int currentStartingCon = 0;
	int currentSolver = 0;
	std::pair<int, int> resolution = { 160, 108 }; // Set to size of starting condition texture;

	// Water Shader Control
	float strength = 145.f; // Controls the strength for changing between the shallow and depth colour.
	float shallowColor[4] = { 0.1f, 0.37f, 0.55f, 0.85f }; // The colour that will be more prominent on shallow patches of water.
	float deepColor[4] = { 0.004f, 0.15f, 0.3f, 0.77f }; // The colour that will be more prominent on deep patches of water.

	bool waterReflections = true; // Whether first order water reflections are enabled or disabled.

	bool onlyRenderOne = false; // Controls if the program should only render one patch of water (If having performance issues set this true)
	int numToRender = 4;

	int currentTessellation = 0; // Amount of tessellation the water shader will use.
};