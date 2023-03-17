#include <mzpch.h>
#include <Geometry/Wavefront.h>

Wavefront::Wavefront(Microsoft::WRL::ComPtr<ID3D11Device> device, const char* filename)
{
	// Format filename
	std::string s = contentPathS "Content/";
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

	// Vectors to store the wavefront data.
	std::vector<DirectX::XMFLOAT3> v;
	std::vector<DirectX::XMFLOAT2> vt;
	std::vector<DirectX::XMFLOAT3> vn;
	std::unordered_map<std::string, DirectX::XMFLOAT3> materialDict;
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
			data[i].materials = materialDict[currentMat];
			indices[i] = i;
			i++;

			data[i].position = v[vIndex.y - 1];
			data[i].texture = vt[vtIndex.y - 1];
			data[i].normals = vn[vnIndex.y - 1];
			data[i].materials = materialDict[currentMat];
			indices[i] = i;
			i++;

			data[i].position = v[vIndex.x - 1];
			data[i].texture = vt[vtIndex.x - 1];
			data[i].normals = vn[vnIndex.x - 1];
			data[i].materials = materialDict[currentMat];
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
			ReadMaterials(materialDict,mtlFilename.c_str());
		}
	}

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0u;
	vertexBufferDesc.MiscFlags = 0u;
	vertexBufferDesc.ByteWidth = sizeof(Data) * vertexCount;
	vertexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = data;

	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);


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
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
}

void Wavefront::SendData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext)
{
	const UINT stride = sizeof(MaterialData);
	const UINT offset = 0u;
	deviceContext->IASetVertexBuffers(0u, 1u, vertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Wavefront::ReadMaterials(std::unordered_map<std::string, DirectX::XMFLOAT3>& dict, const char* filename)
{
	std::string s = contentPathS "Content/";
	s.append(filename);

	// Input file.
	std::ifstream obj(s);
	std::string name;

	while (std::getline(obj, s))
	{
		if (s[0] == '#' || s.empty()) // Comment line or Empty line
			continue;

		if (s.substr(0, 2) == "Kd") // Diffuse Color
		{
			DirectX::XMFLOAT3 material;
			sscanf_s(s.c_str(), "%*s %f %f %f\n", &material.x, &material.y, &material.z);
			dict[name] = material;
			return;
		}

		if (s.size() < 6) // Line too small for next check
			continue;

		if (s.substr(0, 6) == "newmtl") // New material name
		{
			name = s.substr(7, s.size() - 7);
		}
	}
}
