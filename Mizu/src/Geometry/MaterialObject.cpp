#include <mzpch.h>
#include <Geometry/MaterialObject.h>
#include <Graphics/TextureArray.h>
#include <Graphics/Graphics.h>

MaterialObject::MaterialObject(Graphics* gfx, const char* filename, TextureArray** diffTex , TextureArray** bumpTex)
{
	// Format filename
	std::string s =  "Content/Models/";
	s.append(filename);

	// Input file.
	std::ifstream obj(s);

	// Determine the amount of vertices
	while (std::getline(obj, s))
	{
		if (s[0] == 'f')
			indexCount += 3;
	}
	vertexCount = indexCount;

	// Vectors to store the MaterialObject data.
	std::vector<DirectX::XMFLOAT3> v;
	std::vector<DirectX::XMFLOAT2> vt;
	std::vector<DirectX::XMFLOAT3> vn;
	std::unordered_map<std::string, Material> materialDict;
	std::string currentMat = "";

	// Index for the while loop.
	int i = 0;

	// Create indices and data on the heap.
	unsigned long* indices = new unsigned long[indexCount];
	MaterialData* data = new MaterialData[vertexCount];

	// Move back to beginning of file.
	obj.clear();
	obj.seekg(0, obj.beg);

	while (std::getline(obj, s))
	{
		if (s[0] == '#') // Comment Line
			continue;

		if (s[0] == 's') // Smoothing setting (1 on, 0 off)
			continue;

		if (s[0] == 'g') // Group name
			continue;

		if (s[0] == 'o') // Object name
			continue;

		if (s.empty()) // Empty line
			continue;

		if (s.substr(0, 2) == "vt") // Texture Coordinate
		{
			DirectX::XMFLOAT2 tex;
			sscanf_s(s.c_str(), "%*s %f %f\n", &tex.x, &tex.y);
			tex.y = 1.f - tex.y;
			vt.push_back(tex);
			continue;
		}

		if (s.substr(0, 2) == "vn") // Normals
		{
			DirectX::XMFLOAT3 normal;
			sscanf_s(s.c_str(), "%*s %f %f %f\n", &normal.x, &normal.y, &normal.z);
			normal.z *= -1.f;
			vn.push_back(normal);
			continue;
		}

		if (s.substr(0, 2) == "v ") // Vertices
		{
			DirectX::XMFLOAT3 vertex;
			sscanf_s(s.c_str(), "%*s %f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertex.z *= 1.f;
			v.push_back(vertex);
			continue;
		}

		if (s.substr(0, 2) == "f ") // Faces
		{
			DirectX::XMINT3 vIndex, vtIndex, vnIndex;
			sscanf_s(s.c_str(), "%*s %i %*c %i %*c %i %i %*c %i %*c %i %i %*c %i %*c %i\n", &vIndex.x, &vtIndex.x, &vnIndex.x, &vIndex.y, &vtIndex.y, &vnIndex.y, &vIndex.z, &vtIndex.z, &vnIndex.z);

			data[i].position = v[vIndex.z - 1];
			data[i].texture = vt[vtIndex.z - 1];
			data[i].normals = vn[vnIndex.z - 1];
			data[i].ambient = materialDict[currentMat].ambient;
			data[i].diffuse = materialDict[currentMat].diffuse;
			data[i].emissive = materialDict[currentMat].emissive;
			data[i].specular = materialDict[currentMat].specular;
			data[i].specExponent = materialDict[currentMat].specExponent;
			data[i].diffID = materialDict[currentMat].diffID;
			data[i].bumpID = materialDict[currentMat].bumpID;
			data[i].bumpMult = materialDict[currentMat].bumpMult;
			indices[i] = i;
			i++;

			data[i].position = v[vIndex.y - 1];
			data[i].texture = vt[vtIndex.y - 1];
			data[i].normals = vn[vnIndex.y - 1];
			data[i].ambient = materialDict[currentMat].ambient;
			data[i].diffuse = materialDict[currentMat].diffuse;
			data[i].emissive = materialDict[currentMat].emissive;
			data[i].specular = materialDict[currentMat].specular;
			data[i].specExponent = materialDict[currentMat].specExponent;
			data[i].diffID = materialDict[currentMat].diffID;
			data[i].bumpID = materialDict[currentMat].bumpID;
			data[i].bumpMult = materialDict[currentMat].bumpMult;
			indices[i] = i;
			i++;

			data[i].position = v[vIndex.x - 1];
			data[i].texture = vt[vtIndex.x - 1];
			data[i].normals = vn[vnIndex.x - 1];
			data[i].ambient = materialDict[currentMat].ambient;
			data[i].diffuse = materialDict[currentMat].diffuse;
			data[i].emissive = materialDict[currentMat].emissive;
			data[i].specular = materialDict[currentMat].specular;
			data[i].specExponent = materialDict[currentMat].specExponent;
			data[i].diffID = materialDict[currentMat].diffID;
			data[i].bumpID = materialDict[currentMat].bumpID;
			data[i].bumpMult = materialDict[currentMat].bumpMult;
			indices[i] = i;
			i++;

			continue;
		}

		if (s.size() < 6) //String too small for next checks
			continue;

		if (s.substr(0,6) == "usemtl") // Sets which texture to use.
		{
			currentMat = s.substr(7, s.size() - 7);
			continue;
		}

		if (s.substr(0, 6) == "mtllib") // MTL filename
		{
			std::string mtlFilename = s.substr(7, s.size() - 7);
			ReadMaterials(materialDict,mtlFilename.c_str(), filename, gfx, diffTex, bumpTex);
		}
	}

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0u;
	vertexBufferDesc.MiscFlags = 0u;
	vertexBufferDesc.ByteWidth = sizeof(MaterialData) * vertexCount;
	vertexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = data;

	gfx->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);


	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	gfx->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
}

void MaterialObject::SendData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, D3D_PRIMITIVE_TOPOLOGY topology)
{
	const UINT stride = sizeof(MaterialData);
	const UINT offset = 0u;
	deviceContext->IASetVertexBuffers(0u, 1u, vertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(topology);
}

void MaterialObject::ReadMaterials(std::unordered_map<std::string, Material>& dict, const char* filename, std::string originalFilename, Graphics* gfx, TextureArray** diffTex, TextureArray** bumpTex)
{
	auto it = originalFilename.find('/');

	int diffId = 0;
	int bumpId = 0;

	std::string s =  "Content/Models/";
	s.append(originalFilename.substr(0, it+1));
	s.append(filename);

	std::unordered_map<std::string, int> loadedDiffTex;
	std::unordered_map<std::string, int> loadedBumpTex;

	// Input file.
	std::ifstream obj(s);
	std::string name;

	while (std::getline(obj, s))
	{
		if (s[0] == '#') // Comment line
			continue;

		if (s[0] == s.empty()) // Empty line
			continue;

		if (s.substr(0, 2) == "Ni") // No Refraction :( (Optical Density)
			continue;

		if (s[0] == 'd') // No transparency (Dissolve)
			continue;

		if (s.substr(0, 5) == "illum") // No illumination model
			continue;

		if (s.substr(0, 2) == "Ka") // Ambient Color
		{
			DirectX::XMFLOAT3 ambient;
			sscanf_s(s.c_str(), "%*s %f %f %f\n", &ambient.x, &ambient.y, &ambient.z);
			dict[name].ambient = ambient;
			continue;
		}

		if (s.substr(0, 2) == "Kd") // Diffuse Color
		{
			DirectX::XMFLOAT3 diffuse;
			sscanf_s(s.c_str(), "%*s %f %f %f\n", &diffuse.x, &diffuse.y, &diffuse.z);
			dict[name].diffuse = diffuse;
			continue;
		}

		if (s.substr(0, 2) == "Ke")
		{
			DirectX::XMFLOAT3 emissive;
			sscanf_s(s.c_str(), "%*s %f %f %f\n", &emissive.x, &emissive.y, &emissive.z);
			dict[name].emissive = emissive;
			continue;
		}

		if (s.substr(0, 2) == "Ks") // Specular Color
		{
			DirectX::XMFLOAT3 specular;
			sscanf_s(s.c_str(), "%*s %f %f %f\n", &specular.x, &specular.y, &specular.z);
			dict[name].specular = specular;
			continue;
		}

		if (s.substr(0, 2) == "Ns") // Specular Exponent
		{
			float specExponent;
			sscanf_s(s.c_str(), "%*s %f\n", &specExponent);
			dict[name].specExponent = specExponent;
			continue;
		}

		if (s.size() < 6) // Line too small for next check
			continue;

		if (s.substr(0, 6) == "newmtl") // New material name
		{
			name = s.substr(7, s.size() - 7);
			dict[name] = { {-1.f, -1.f, -1.f}, {-1.f, -1.f, -1.f}, {0.f, 0.f, 0.f},{-1.f, -1.f, -1.f}, -1.f, -1, -1, -1.f };
			continue;
		}

		if (s.substr(0, 6) == "map_Kd") // Color texture file
		{
			std::string fullFilename = originalFilename.substr(0, it + 1);
			fullFilename.append(s.substr(7, s.size() - 7));

			if (!loadedDiffTex.contains(fullFilename))
			{
				loadedDiffTex[fullFilename] = diffId;
				diffId++;
			}

			dict[name].diffID = loadedDiffTex[fullFilename];
		}

		if (s.substr(0, 8) == "map_Bump") // Color texture file
		{
			std::string num, str;
			std::string fullFilename = originalFilename.substr(0, it + 1);
			if (s.substr(9, 3) == "-bm")
			{
				num = s.substr(13, 8);
				str = s.substr(22, s.size() - 22);
			}
			else
			{
				num = "1.0";
				str = s.substr(9, s.size() - 9);
			}
			fullFilename.append(str);
			
			if (!loadedBumpTex.contains(fullFilename))
			{
				loadedBumpTex[fullFilename] = bumpId;
				bumpId++;
			}

			dict[name].bumpID = loadedBumpTex[fullFilename];
			dict[name].bumpMult = std::stoi(num);
		}
	}

	std::vector<std::wstring> diffTextures;
	std::vector<std::wstring> bumpTextures;

	for (auto& it : loadedDiffTex)
	{
		diffTextures.push_back(StringConverter::StringToWide(it.first));
	}

	for (auto& it : loadedBumpTex)
	{
		bumpTextures.push_back(StringConverter::StringToWide(it.first));
	}

	*diffTex = new TextureArray(gfx->GetDevice(), gfx->GetDeviceContext(), &diffTextures);
	*bumpTex = new TextureArray(gfx->GetDevice(), gfx->GetDeviceContext(), &bumpTextures);
}
