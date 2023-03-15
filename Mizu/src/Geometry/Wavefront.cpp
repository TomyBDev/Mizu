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

	// Used for temporarily storing values.
	char c;
	float val1, val2, val3;
	int int1, int2, int3;

	// Index for the while loop.
	int i = 0;

	// Create indices and data on the heap.
	unsigned long* indices = new unsigned long[indexCount];
	Data* data = new Data[vertexCount];

	// Move back to beginning of file.
	obj.clear();
	obj.seekg(0, obj.beg);

	while (std::getline(obj, s))
	{
		if (s[0] == '#' || s[0] == 'm' || s[0] == 's' || s[0] == 'g' || s[0] == 'u' || s.empty())
			continue;

		if (s[1] == 't')
		{
			std::istringstream iss(s);
			iss >> c >> c >> val1 >> val2;
			vt.push_back({ val1, 1.f - val2 });
		}
		else if (s[1] == 'n')
		{
			std::istringstream iss(s);
			iss >> c >> c >> val1 >> val2 >> val3;
			vn.push_back({ val1, val2, val3*-1.f });
		}
		else if (s[0] == 'v')
		{
			std::istringstream iss(s);
			iss >> c >> val1 >> val2 >> val3;
			v.push_back({ val1, val2, val3*-1.f });
		}
		else if (s[0] == 'f')
		{
			std::istringstream iss(s);

			iss >> c >> int1 >> c >> int2 >> c >> int3;
			data[i].position = v[int1 - 1];
			data[i].texture = vt[int2 - 1];
			data[i].normals = vn[int3 - 1];
			indices[i] = i;
			i++;

			iss >> int1 >> c >> int2 >> c >> int3;
			data[i].position = v[int1 - 1];
			data[i].texture = vt[int2 - 1];
			data[i].normals = vn[int3 - 1];
			indices[i] = i;
			i++;

			iss >> int1 >> c >> int2 >> c >> int3;
			data[i].position = v[int1 - 1];
			data[i].texture = vt[int2 - 1];
			data[i].normals = vn[int3 - 1];
			indices[i] = i;
			i++;
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