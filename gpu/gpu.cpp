/**
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Dominik Harmim <harmim6@gmail.com>
 *
 * This file contains implementation of simplyfied virtual GPU.
 * Students do not have to look here.
 */


#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <tuple>
#include <vector>

#include <student/fwd.h>
#include <student/buffer.h>
#include <student/gpu.h>
#include <student/linearAlgebra.h>
#include <student/program.h>
#include <student/uniforms.h>
#include <student/vertexPuller.h>


#if !defined(__func__)
#define __func__ __FUNCTION__
#endif


template<typename TYPE>
std::string fceArgError2Str(const TYPE &value, const std::string &fceName)
{
	std::stringstream ss;
	ss << "ERROR: " << fceName << "(..., " << value << ", ...) failed: ";
	return ss.str();
}


template<typename TYPE>
std::string fceArgWarning2Str(const TYPE &value, const std::string &fceName)
{
	std::stringstream ss;
	ss << "WARNING: " << fceName << "(..., " << value << ", ...): ";
	return ss.str();
}


void printAttribIndexError(size_t attribIndex, const std::string &fceName)
{
	std::cerr << fceArgError2Str(attribIndex, fceName)
		<< "attribute index cannot be >= " << MAX_ATTRIBUTES << std::endl;
}


std::string uniformType2Str(const UniformType &type)
{
	switch (type)
	{
		case UNIFORM_FLOAT:
			return "UNIFORM_FLOAT";
		case UNIFORM_VEC2:
			return "UNIFORM_VEC2";
		case UNIFORM_VEC3:
			return "UNIFORM_VEC3";
		case UNIFORM_VEC4:
			return "UNIFORM_VEC4";
		case UNIFORM_UINT:
			return "UNIFORM_UINT";
		case UNIFORM_MAT4:
			return "UNIFORM_MAT4";
		default:
			return "unknown";
	}
}


std::string attribType2Str(const AttributeType &type)
{
	switch (type)
	{
		case ATTRIB_FLOAT:
			return "ATTRIB_FLOAT";
		case ATTRIB_VEC2:
			return "ATTRIB_VEC2";
		case ATTRIB_VEC3:
			return "ATTRIB_VEC3";
		case ATTRIB_VEC4:
			return "ATTRIB_VEC4";
		case ATTRIB_EMPTY:
			return "ATTRIB_EMPTY";
		default:
			return "unknown";
	}
}


class UniformImplementation
{
public:
	UniformType type;
	std::shared_ptr<uint8_t> data;


	UniformImplementation(const UniformType &t, uint8_t *const &d)
		: type(t), data(
		d, [](uint8_t *const &ptr)
		{ delete[] ptr; }
	)
	{}
};


class GpuImplementation;


using PullerAttribTuple = std::tuple<VertexPullerID, AttribIndex>;


class PullerAttrib : PullerAttribTuple
{
public:
	PullerAttrib(const VertexPullerID &puller, const AttribIndex &attrib)
		: PullerAttribTuple(puller, attrib)
	{}


	enum Parts
	{
		PULLER = 0,
		ATTRIB = 1,
	};


	bool operator<(const PullerAttrib &other) const
	{
		if (std::get<PULLER>(*this) < std::get<PULLER>(other))
		{ return true; }
		if (std::get<PULLER>(*this) > std::get<PULLER>(other))
		{ return false; }
		if (std::get<ATTRIB>(*this) < std::get<ATTRIB>(other))
		{ return true; }
		if (std::get<ATTRIB>(*this) > std::get<ATTRIB>(other))
		{ return false; }
		return false;
	}


	VertexPullerID getPuller() const
	{
		return std::get<PULLER>(*this);
	}


	size_t getAttrib() const
	{
		return std::get<ATTRIB>(*this);
	}
};


using AttribBuffers         = std::map<AttribIndex, BufferID>;
using PullerReferencesTuple = std::tuple<BufferID, AttribBuffers>;


class PullerReferences : PullerReferencesTuple
{
public:
	PullerReferences(
		const BufferID &indexBuffer = 0,
		const AttribBuffers &attribBuffers = AttribBuffers{}
	)
		: PullerReferencesTuple(indexBuffer, attribBuffers)
	{}


	enum Parts
	{
		INDEX_BUFFER = 0,
		ATTRIB_BUFFERS = 1,
	};


	const BufferID &getIndexBuffer() const
	{
		return std::get<INDEX_BUFFER>(*this);
	}


	const AttribBuffers &getAttribBuffers() const
	{
		return std::get<ATTRIB_BUFFERS>(*this);
	}


	BufferID &getIndexBuffer()
	{
		return std::get<INDEX_BUFFER>(*this);
	}


	AttribBuffers &getAttribBuffers()
	{
		return std::get<ATTRIB_BUFFERS>(*this);
	}


	const BufferID &getAttribBuffer(const AttribIndex &index) const
	{
		return this->getAttribBuffers().at(index);
	}


	BufferID &getAttribBuffer(const AttribIndex &index)
	{
		return this->getAttribBuffers().at(index);
	}


	bool hasAttribBuffer(const AttribIndex &index) const
	{
		return this->getAttribBuffers().count(index) > 0;
	}
};


using PullerSet             = std::set<VertexPullerID>;
using PullerAttribSet       = std::set<PullerAttrib>;
using BufferReferencesTuple = std::tuple<PullerSet, PullerAttribSet>;


class BufferReferences : BufferReferencesTuple
{
public:
	BufferReferences(
		const PullerSet &indexings = PullerSet{},
		const PullerAttribSet &attribs = PullerAttribSet{}
	)
		: BufferReferencesTuple(indexings, attribs)
	{}


	enum Parts
	{
		INDEXING = 0,
		ATTRIBS = 1,
	};


	const PullerSet &getIndexings() const
	{
		return std::get<INDEXING>(*this);
	}


	const PullerAttribSet &getAttribs() const
	{
		return std::get<ATTRIBS>(*this);
	}


	PullerSet &getIndexings()
	{
		return std::get<INDEXING>(*this);
	}


	PullerAttribSet &getAttribs()
	{
		return std::get<ATTRIBS>(*this);
	}
};


class AllUniforms
{
public:
	std::vector<UniformImplementation> uniforms;
	std::map<std::string, size_t> locations;
};


class AttribInterpolation
{
public:
	AttributeType type;
	InterpolationType interpolation;


	AttribInterpolation(
		const AttributeType &t = ATTRIB_EMPTY,
		const InterpolationType &inter = SMOOTH
	)
		: type(t), interpolation(inter)
	{}
};


class ProgramSettings
{
public:
	VertexShader vertexShader = nullptr;
	FragmentShader fragmentShader = nullptr;
	std::array<AttribInterpolation, MAX_ATTRIBUTES> interpolations;


	ProgramSettings(
		const VertexShader &vs = nullptr,
		const FragmentShader &fs = nullptr
	)
		: vertexShader(vs), fragmentShader(fs)
	{}
};


class GpuImplementation
{
public:
	GpuImplementation()
	{}


	~GpuImplementation()
	{}


	static const size_t EMPTY_BUFFER_ID = 0;
	static const size_t EMPTY_VAO_ID = 0;

	size_t viewportWidth = 0;
	size_t viewportHeight = 0;
	AllUniforms uniforms;
	std::vector<float> depthBuffer;
	std::vector<Vec4> colorBuffer;

	static const size_t outOfRange;


	size_t getLinearPixelCoord(
		size_t x, size_t y, const std::string &fceName
	) const
	{
		const size_t w = ((size_t) this->viewportWidth);
		const size_t h = ((size_t) this->viewportHeight);
		if (x >= w)
		{
			std::cerr << fceArgError2Str(x, fceName)
				<< "x coord is out of range: [0," << w << ")" << std::endl;
			return outOfRange;
		}
		if (y >= h)
		{
			std::cerr << fceArgError2Str(y, fceName)
				<< "y coord is out of range: [0," << h << ")" << std::endl;
			return outOfRange;
		}
		return y * w + x;
	}


	// this holds gpu buffers
	std::map<BufferID, std::vector<uint8_t>> buffers;
	// this holds number of already allocated buffers
	BufferID bufferCounter = 1;
	// (even deleted), zero is reserved for empty
	// buffer

	std::map<VertexPullerID, GPUVertexPullerConfiguration>
		vaos;            // this holds gpu vertex arrays
	// this holds number of already allocated vaos
	VertexPullerID vaoCounter = 1;
	// (even deleted), zero is reserved for empty
	// vao
	VertexPullerID activeVao = 0;   // currently bound vao

	std::map<VertexPullerID, PullerReferences> pullerReferences;
	std::map<BufferID, BufferReferences> bufferReferences;

	std::map<ProgramID, ProgramSettings> programs;  // this holds all programs
	ProgramID programCounter = 1;  // this holds number of already created
	// programs (even deleted), zero is reserved
	// for emty program
	ProgramID activeProgram = 0;   // currently used program


	enum ProgramParts
	{
		VERTEX_SHADER = 0,
		FRAGMENT_SHADER = 1,
	};


	using bufferIterator  = decltype(buffers)::iterator;
	using vaoIterator     = decltype(vaos)::iterator;
	using programIterator = decltype(programs)::iterator;


	bufferIterator getBuffer(const BufferID &id, const std::string &fceName)
	{
		auto bit = this->buffers.find(id);
		if (bit == this->buffers.end())
		{
			std::cerr << fceArgError2Str(id, fceName)
				<< "there is no such buffer, see cpu_createBuffers"
				<< std::endl;
			return this->buffers.end();
		}
		return bit;
	}


	vaoIterator getVAO(const VertexPullerID &id, const std::string &fceName)
	{
		auto it = this->vaos.find(id);
		if (it == this->vaos.end())
		{
			std::cerr << fceArgError2Str(id, fceName)
				<< "there is no such vertex puller, see cpu_createVertexPullers"
				<< std::endl;
			return this->vaos.end();
		}
		return it;
	}


	programIterator getProgram(const ProgramID &id, const std::string &fceName)
	{
		auto it = this->programs.find(id);
		if (it == this->programs.end())
		{
			std::cerr << fceArgError2Str(id, fceName)
				<< "there is no such program, see cpu_createProgram"
				<< std::endl;
			return this->programs.end();
		}
		return it;
	}


	void setEnableVertexAttrib(
		const VertexPullerID &puller,
		const size_t &headIndex, const bool &enable,
		const std::string &fceName
	)
	{
		if (headIndex >= MAX_ATTRIBUTES)
		{
			printAttribIndexError(headIndex, fceName);
			return;
		}

		auto vaoIt = this->getVAO(puller, __func__);
		if (vaoIt == this->vaos.end())
		{ return; }

		vaoIt->second.heads[headIndex].enabled = enable;
	}
};


const size_t GpuImplementation::outOfRange = std::numeric_limits<size_t>::max();


GPU cpu_createGPU()
{
	auto gpu = new GpuImplementation();
	assert(gpu != nullptr);
	return static_cast<GPU>(gpu);
}


void cpu_destroyGPU(GPU gpu)
{ delete static_cast<GpuImplementation *>(gpu); }


size_t uniformSize(const UniformType &type)
{
	switch (type)
	{
		case UNIFORM_FLOAT:
			return sizeof(float);
		case UNIFORM_VEC2:
			return sizeof(float) * 2;
		case UNIFORM_VEC3:
			return sizeof(float) * 3;
		case UNIFORM_VEC4:
			return sizeof(float) * 4;
		case UNIFORM_UINT:
			return sizeof(uint32_t);
		case UNIFORM_MAT4:
			return sizeof(float) * 16;
		default:
			return 0;
	}
}


void cpu_reserveUniform(
	const GPU gpu, const char *const name, const UniformType type
)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	auto stringName = std::string(name);
	auto it = g->uniforms.locations.find(stringName);
	if (it != g->uniforms.locations.end())
	{
		std::cerr << fceArgError2Str(name, __func__)
			<< "uniform name is already reserved." << std::endl;
		return;
	}
	auto location = g->uniforms.locations.size();
	g->uniforms.locations[stringName] = location;

	auto data = new uint8_t[uniformSize(type)];
	assert(data != nullptr);
	std::memset(data, 0, uniformSize(type));

	g->uniforms.uniforms.emplace_back(type, data);
}


UniformLocation getUniformLocation(const GPU gpu, const char *const name)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	auto stringName = std::string(name);
	auto it = g->uniforms.locations.find(stringName);
	if (it == g->uniforms.locations.end())
	{ return -1; }
	return static_cast<UniformLocation>(it->second);
}


#define CPU_UNIFORM_UPLOAD_IMPLEMENTATION(type)                                \
  assert(gpu != nullptr);                                                      \
  if (location < 0) {                                                          \
    std::cerr << fceArgWarning2Str(location, __func__)                         \
              << "negative locations are ignored" << std::endl;                \
    return;                                                                    \
  }                                                                            \
  auto g     = static_cast<GpuImplementation*>(gpu);                           \
  auto index = static_cast<size_t>(location);                                  \
  if (index >= g->uniforms.uniforms.size()) {                                  \
    std::cerr << fceArgError2Str(location, __func__)                           \
              << "location does not point to any reserved uniform value, see " \
                 "cpu_reserveUniform"                                          \
              << std::endl;                                                    \
    return;                                                                    \
  }                                                                            \
  auto ptr = reinterpret_cast<type*>(&*g->uniforms.uniforms.at(index).data)


void cpu_uniform1f(
	const GPU gpu, const UniformLocation location, const float v0
)
{
	CPU_UNIFORM_UPLOAD_IMPLEMENTATION(float);
	ptr[0] = v0;
}


void cpu_uniform2f(
	const GPU gpu, const UniformLocation location,
	const float v0, const float v1
)
{
	CPU_UNIFORM_UPLOAD_IMPLEMENTATION(float);
	ptr[0] = v0;
	ptr[1] = v1;
}


void cpu_uniform3f(
	const GPU gpu, const UniformLocation location,
	const float v0, const float v1, const float v2
)
{
	CPU_UNIFORM_UPLOAD_IMPLEMENTATION(float);
	ptr[0] = v0;
	ptr[1] = v1;
	ptr[2] = v2;
}


void cpu_uniform4f(
	const GPU gpu, const UniformLocation location,
	const float v0, const float v1, const float v2, const float v3
)
{
	CPU_UNIFORM_UPLOAD_IMPLEMENTATION(float);
	ptr[0] = v0;
	ptr[1] = v1;
	ptr[2] = v2;
	ptr[3] = v3;
}


void cpu_uniformMatrix4fv(
	const GPU gpu, const UniformLocation location, const float *const data
)
{
	CPU_UNIFORM_UPLOAD_IMPLEMENTATION(float);
	std::memcpy(ptr, data, uniformSize(UNIFORM_MAT4));
}


#define GPU_UNIFORM_DOWNLOAD_IMPLEMENTATION(uniformType, typeName)         \
  assert(uniforms != nullptr);                                             \
  auto u = static_cast<AllUniforms const*>(uniforms);                      \
  if (location < 0) {                                                      \
    std::cerr << fceArgWarning2Str(location, __func__);                    \
    std::cerr << "negative locations cannot be used" << std::endl;         \
    return nullptr;                                                        \
  }                                                                        \
  auto index = static_cast<size_t>(location);                              \
  if (index >= u->uniforms.size()) {                                       \
    std::cerr << fceArgError2Str(location, __func__);                      \
    std::cerr << "location does not point to any reserved ";               \
    std::cerr << "uniform value, see cpu_reserveUniform" << std::endl;     \
    return nullptr;                                                        \
  }                                                                        \
  if (u->uniforms.at(index).type != uniformType) {                         \
    std::cerr << fceArgError2Str(location, __func__);                      \
    std::cerr << "type of uniform value is not ";                          \
    std::cerr << uniformType2Str(uniformType) << " but ";                  \
    std::cerr << uniformType2Str(u->uniforms.at(index).type) << std::endl; \
    return nullptr;                                                        \
  }                                                                        \
  return reinterpret_cast<typeName const*>(&*u->uniforms.at(index).data)


const float *shader_interpretUniformAsFloat(
	const Uniforms uniforms, const UniformLocation location
)
{
	GPU_UNIFORM_DOWNLOAD_IMPLEMENTATION(UNIFORM_FLOAT, float);
}


const Vec2 *shader_interpretUniformAsVec2(
	const Uniforms uniforms, const UniformLocation location
)
{
	GPU_UNIFORM_DOWNLOAD_IMPLEMENTATION(UNIFORM_VEC2, Vec2);
}


const Vec3 *shader_interpretUniformAsVec3(
	const Uniforms uniforms, const UniformLocation location
)
{
	GPU_UNIFORM_DOWNLOAD_IMPLEMENTATION(UNIFORM_VEC3, Vec3);
}


const Vec4 *shader_interpretUniformAsVec4(
	const Uniforms uniforms, const UniformLocation location
)
{
	GPU_UNIFORM_DOWNLOAD_IMPLEMENTATION(UNIFORM_VEC4, Vec4);
}


const Mat4 *shader_interpretUniformAsMat4(
	const Uniforms uniforms, const UniformLocation location
)
{
	GPU_UNIFORM_DOWNLOAD_IMPLEMENTATION(UNIFORM_MAT4, Mat4);
}


void cpu_createBuffers(const GPU gpu, const size_t n, BufferID *const buffers)
{
	assert(gpu != nullptr);
	assert(buffers != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	for (size_t i = 0; i < n; ++i)
	{
		const auto currentId = g->bufferCounter + i;
		buffers[i] = currentId;
		g->buffers[currentId] = std::vector<uint8_t>();
		g->bufferReferences[currentId] = BufferReferences();
	}
	g->bufferCounter += n;
}


void cpu_bufferData(
	const GPU gpu, const BufferID buffer, const size_t size,
	const void *const data
)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	auto it = g->buffers.find(buffer);
	if (it == g->buffers.end())
	{
		std::cerr << fceArgError2Str(buffer, __func__);
		std::cerr
			<< "there is no such buffer, see cpu_createBuffers" << std::endl;
		return;
	}
	auto &buf = it->second;
	buf.resize(size);
	std::memcpy(buf.data(), data, size);

	// update vertex arrays attrib pointers
	const auto &bufferReferences = g->bufferReferences.at(buffer);
	for (const auto &puller : bufferReferences.getIndexings())
	{
		g->vaos.at(puller).indices = (const VertexIndex *) buf.data();
	}

	for (const auto &pullerAttrib : bufferReferences.getAttribs())
	{
		g->vaos.at(pullerAttrib.getPuller())
			.heads[pullerAttrib.getAttrib()]
			.buffer = buf.data();
	}
}


void cpu_createVertexPullers(
	const GPU gpu, const size_t n, VertexPullerID *const arrays
)
{
	assert(gpu != nullptr);
	assert(arrays != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	for (size_t i = 0; i < n; ++i)
	{
		const auto currentId = g->vaoCounter + i;
		arrays[i] = currentId;
		g->vaos[currentId] = GPUVertexPullerConfiguration{{}, nullptr};
		for (size_t h = 0; h < MAX_ATTRIBUTES; ++h)
		{
			auto &head = g->vaos[currentId].heads[h];
			head.buffer = nullptr;
			head.enabled = false;
			head.stride = 0;
			head.offset = 0;
		}
		g->pullerReferences[currentId] = PullerReferences();
	}
	g->vaoCounter += n;
}


void cpu_setVertexPullerHead(
	const GPU gpu, const VertexPullerID puller,
	const size_t attribIndex, const BufferID buffer,
	const size_t offset, const size_t stride
)
{
	assert(gpu != nullptr);
	if (attribIndex >= MAX_ATTRIBUTES)
	{
		printAttribIndexError(attribIndex, __func__);
		return;
	}

	auto g = static_cast<GpuImplementation *>(gpu);
	auto vaoIt = g->getVAO(puller, __func__);
	if (vaoIt == g->vaos.end())
	{ return; }

	auto bufferIt = g->getBuffer(buffer, __func__);
	if (bufferIt == g->buffers.end())
	{ return; }

	auto &head = vaoIt->second.heads[attribIndex];
	head.buffer = &*bufferIt->second.data();
	head.stride = stride;
	head.offset = offset;

	auto &pullerReferences = g->pullerReferences.at(puller);
	if (pullerReferences.hasAttribBuffer(attribIndex))
	{
		const auto old = pullerReferences.getAttribBuffer(attribIndex);
		g->bufferReferences.at(old).getAttribs().erase(
			PullerAttrib(puller, attribIndex));
	}
	auto &attribBuffers = pullerReferences.getAttribBuffers();
	attribBuffers[attribIndex] = buffer;
	g->bufferReferences.at(buffer).getAttribs().insert(
		PullerAttrib(puller, attribIndex));
}


void cpu_setIndexing(
	const GPU gpu, const VertexPullerID puller,
	const BufferID buffer, const size_t indexSize
)
{
	assert(gpu != nullptr);
	if (indexSize != sizeof(uint8_t) && indexSize != sizeof(uint16_t) &&
		indexSize != sizeof(uint32_t))
	{
		std::cerr << fceArgError2Str(indexSize, __func__)
			<< "indexSizeInBytes has to be ";
		std::cerr << sizeof(uint8_t) << "," << sizeof(uint16_t) << " or "
			<< sizeof(uint32_t) << std::endl;
		return;
	}
	auto g = static_cast<GpuImplementation *>(gpu);

	auto vaoIt = g->getVAO(puller, __func__);
	if (vaoIt == g->vaos.end())
	{ return; }

	const void *ptr = nullptr;
	if (buffer != GpuImplementation::EMPTY_BUFFER_ID)
	{
		auto bufferIt = g->getBuffer(buffer, __func__);
		if (bufferIt == g->buffers.end())
		{ return; }
		ptr = &*bufferIt->second.data();
	}

	vaoIt->second.indices = (const VertexIndex *) ptr;

	auto &pullerReferences = g->pullerReferences.at(puller);
	if (pullerReferences.getIndexBuffer() != GpuImplementation::EMPTY_BUFFER_ID)
	{
		const auto old = pullerReferences.getIndexBuffer();
		g->bufferReferences.at(old).getIndexings().erase(puller);
	}
	pullerReferences.getIndexBuffer() = buffer;
	g->bufferReferences.at(buffer).getIndexings().insert(puller);
}


void cpu_enableVertexPullerHead(
	const GPU gpu, const VertexPullerID puller, const size_t attribIndex
)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	g->setEnableVertexAttrib(puller, attribIndex, true, __func__);
}


void cpu_disableVertexPullerHead(
	const GPU gpu, const VertexPullerID puller, const size_t attribIndex
)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	g->setEnableVertexAttrib(puller, attribIndex, false, __func__);
}


void cpu_bindVertexPuller(const GPU gpu, const VertexPullerID id)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);

	auto vaoId = g->getVAO(id, __func__);
	if (vaoId == g->vaos.end())
	{ return; }

	g->activeVao = id;
}


ProgramID cpu_createProgram(const GPU gpu)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	ProgramID result = g->programCounter;
	g->programs[result] = ProgramSettings();
	g->programCounter += 1;
	return result;
}


void cpu_deleteProgram(const GPU gpu, const ProgramID program)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	auto it = g->getProgram(program, __func__);
	if (it == g->programs.end())
	{ return; }
	g->programs.erase(it->first);
}


void cpu_attachVertexShader(
	const GPU gpu, const ProgramID program, const VertexShader shader
)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	auto it = g->getProgram(program, __func__);
	if (it == g->programs.end())
	{ return; }
	it->second.vertexShader = shader;
}


void cpu_attachFragmentShader(
	const GPU gpu, const ProgramID program, const FragmentShader shader
)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	auto it = g->getProgram(program, __func__);
	if (it == g->programs.end())
	{ return; }
	it->second.fragmentShader = shader;
}


void cpu_useProgram(const GPU gpu, const ProgramID program)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	auto it = g->getProgram(program, __func__);
	if (it == g->programs.end())
	{ return; }
	g->activeProgram = program;
}


Uniforms gpu_getUniformsHandle(const GPU gpu)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	return static_cast<AllUniforms *>(&g->uniforms);
}


void cpu_setViewportSize(const GPU gpu, const size_t width, const size_t height)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	g->viewportWidth = width;
	g->viewportHeight = height;
	const size_t nofPixels = g->viewportWidth * g->viewportHeight;
	g->colorBuffer.resize(nofPixels);
	g->depthBuffer.resize(nofPixels);
}


size_t gpu_getViewportWidth(const GPU gpu)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	return g->viewportWidth;
}


size_t gpu_getViewportHeight(const GPU gpu)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	return g->viewportHeight;
}


const GPUVertexPullerConfiguration *gpu_getActiveVertexPuller(const GPU gpu)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	auto it = g->getVAO(g->activeVao, __func__);
	if (it == g->vaos.end())
	{ exit(1); }
	return &it->second;
}


VertexShader gpu_getActiveVertexShader(const GPU gpu)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	auto it = g->getProgram(g->activeProgram, __func__);
	if (it == g->programs.end())
	{ exit(1); }
	return it->second.vertexShader;
}


FragmentShader gpu_getActiveFragmentShader(const GPU gpu)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	auto it = g->getProgram(g->activeProgram, __func__);
	if (it == g->programs.end())
	{ exit(1); }
	return it->second.fragmentShader;
}


void cpu_clearColor(const GPU gpu, const Vec4 *const color)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	for (auto &x : g->colorBuffer)
	{ copy_Vec4(&x, color); }
}


void cpu_clearDepth(const GPU gpu, const float depth)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	for (auto &x : g->depthBuffer)
	{ x = depth; }
}


const Vec4 *cpu_getColor(const GPU gpu, const size_t x, const size_t y)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	auto index = g->getLinearPixelCoord(x, y, __func__);
	if (index == GpuImplementation::outOfRange)
	{ exit(1); }
	return &g->colorBuffer.at(index);
}


float gpu_getDepth(const GPU gpu, const size_t x, const size_t y)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	auto index = g->getLinearPixelCoord(x, y, __func__);
	if (index == GpuImplementation::outOfRange)
	{ exit(1); }
	return g->depthBuffer.at(index);
}


void gpu_setDepth(
	const GPU gpu, const size_t x, const size_t y, const float depth
)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	auto index = g->getLinearPixelCoord(x, y, __func__);
	if (index == GpuImplementation::outOfRange)
	{ exit(1); }
	g->depthBuffer.at(index) = depth;
}


void gpu_setColor(
	const GPU gpu, const size_t x, const size_t y, const Vec4 *const color
)
{
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	auto index = g->getLinearPixelCoord(x, y, __func__);
	if (index == GpuImplementation::outOfRange)
	{ exit(1); }
	copy_Vec4(&g->colorBuffer.at(index), color);
}


void cpu_setAttributeInterpolation(
	const GPU gpu, const ProgramID program,
	const size_t attribIndex,
	const AttributeType type,
	const InterpolationType interpolation
)
{
	if (attribIndex >= MAX_ATTRIBUTES)
	{
		printAttribIndexError(attribIndex, __func__);
		exit(1);
	}
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	auto it = g->getProgram(program, __func__);
	if (it == g->programs.end())
	{ exit(1); }
	it->second.interpolations[attribIndex].type = type;
	it->second.interpolations[attribIndex].interpolation = interpolation;
}


InterpolationType gpu_getAttributeInterpolation(
	const GPU gpu, const size_t attribIndex
)
{
	if (attribIndex >= MAX_ATTRIBUTES)
	{
		printAttribIndexError(attribIndex, __func__);
		exit(1);
	}
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	auto it = g->getProgram(g->activeProgram, __func__);
	if (it == g->programs.end())
	{ exit(1); }
	return it->second.interpolations[attribIndex].interpolation;
}


AttributeType gpu_getAttributeType(const GPU gpu, const size_t attribIndex)
{
	if (attribIndex >= MAX_ATTRIBUTES)
	{
		printAttribIndexError(attribIndex, __func__);
		exit(1);
	}
	assert(gpu != nullptr);
	auto g = static_cast<GpuImplementation *>(gpu);
	auto it = g->getProgram(g->activeProgram, __func__);
	if (it == g->programs.end())
	{ exit(1); }
	return it->second.interpolations[attribIndex].type;
}


#define GPU_IMPLEMENTATION_OF_INTERPRETATION_OF_ATTRIB_OF_INPUT_VERTEX(TYPE)   \
  assert(vertex != nullptr);                                                   \
  if (attributeIndex >= MAX_ATTRIBUTES) {                                      \
    printAttribIndexError(attributeIndex, __func__);                           \
    exit(1);                                                                   \
    return nullptr;                                                            \
  }                                                                            \
  auto       g            = (GpuImplementation*)gpu;                           \
  const auto referencesIt = g->pullerReferences.find(g->activeVao);            \
  if (referencesIt == g->pullerReferences.end()) {                             \
    std::cerr << fceArgError2Str(attributeIndex, __func__)                     \
              << "active vertex puller: " << g->activeVao;                     \
    std::cerr << " does not exists";                                           \
    std::cerr << ", vertex puller id: " << GpuImplementation::EMPTY_VAO_ID     \
              << " is reserved for empty puller" << std::endl;                 \
    exit(1);                                                                   \
    return nullptr;                                                            \
  }                                                                            \
  const auto &references = referencesIt->second;                               \
  if (!references.hasAttribBuffer(attributeIndex)) {                           \
    std::cerr << fceArgError2Str(attributeIndex, __func__)                     \
              << "active vertex puller: " << g->activeVao;                     \
    std::cerr << " does not have active vertex attribute: " << attributeIndex; \
    std::cerr << ", see cpu_setVertexPullerHead" << std::endl;                 \
    exit(1);                                                                   \
    return nullptr;                                                            \
  }                                                                            \
  const auto buffer = references.getAttribBuffer(attributeIndex);              \
  if (buffer == GpuImplementation::EMPTY_BUFFER_ID) {                          \
    std::cerr << fceArgError2Str(attributeIndex, __func__)                     \
              << "vertex attribute: " << attributeIndex;                       \
    std::cerr << " of active vertex puller: " << g->activeVao                  \
              << " has buffer set to: " << GpuImplementation::EMPTY_BUFFER_ID; \
    std::cerr << " which is reserved for empty buffer" << std::endl;           \
    exit(1);                                                                   \
    return nullptr;                                                            \
  }                                                                            \
  const auto &data = g->buffers.at(buffer);                                    \
  if (reinterpret_cast<uint8_t const*>(                                        \
          vertex->attributes->attributes[attributeIndex]) +                    \
          sizeof(TYPE) >                                                       \
      data.data() + data.size()) {                                             \
    std::cerr << fceArgError2Str(attributeIndex, __func__)                     \
              << "out of range - this can be caused by one of the following "  \
                 "sources:"                                                    \
              << std::endl;                                                    \
    std::cerr << "1) your vertex puller incorrectly computes pointers"         \
              << std::endl;                                                    \
    std::cerr << "2) you are using indexing and your indices are too large"    \
              << std::endl;                                                    \
    std::cerr                                                                  \
        << "3) you are not using indexing and number of vertices is too large" \
        << std::endl;                                                          \
    std::cerr << "4) your vertex puller settings are wrong" << std::endl;      \
    std::cerr << "5) your attribute is too large (or has different type)"      \
              << std::endl;                                                    \
    exit(1);                                                                   \
    return nullptr;                                                            \
  }                                                                            \
  return reinterpret_cast<TYPE const*>(                                        \
      vertex->attributes->attributes[attributeIndex])


const float *vs_interpretInputVertexAttributeAsFloat(
	const GPU gpu, const GPUVertexShaderInput *const vertex,
	const AttribIndex attributeIndex
)
{
	GPU_IMPLEMENTATION_OF_INTERPRETATION_OF_ATTRIB_OF_INPUT_VERTEX(float);
}


const Vec2 *vs_interpretInputVertexAttributeAsVec2(
	const GPU gpu, const GPUVertexShaderInput *const vertex,
	const AttribIndex attributeIndex
)
{
	GPU_IMPLEMENTATION_OF_INTERPRETATION_OF_ATTRIB_OF_INPUT_VERTEX(Vec2);
}


const Vec3 *vs_interpretInputVertexAttributeAsVec3(
	const GPU gpu, const GPUVertexShaderInput *const vertex,
	const AttribIndex attributeIndex
)
{
	GPU_IMPLEMENTATION_OF_INTERPRETATION_OF_ATTRIB_OF_INPUT_VERTEX(Vec3);
}


const Vec4 *vs_interpretInputVertexAttributeAsVec4(
	const GPU gpu, const GPUVertexShaderInput *const vertex,
	const AttribIndex attributeIndex
)
{
	GPU_IMPLEMENTATION_OF_INTERPRETATION_OF_ATTRIB_OF_INPUT_VERTEX(Vec4);
}


#define GPU_IMPLEMENTATION_OF_INTERPRETATION_OF_ATTRIB_OF_OUTPUT_VERTEX(TYPE, \
                                                                        ENUM) \
  if (attributeIndex >= MAX_ATTRIBUTES) {                                     \
    printAttribIndexError(attributeIndex, __func__);                          \
    exit(1);                                                                  \
  }                                                                           \
  assert(gpu != nullptr);                                                     \
  auto g  = static_cast<GpuImplementation*>(gpu);                             \
  auto it = g->getProgram(g->activeProgram, __func__);                        \
  if (it == g->programs.end()) exit(1);                                       \
  if (it->second.interpolations[attributeIndex].type != ENUM) {               \
    std::cerr << fceArgError2Str(attributeIndex, __func__)                    \
              << " attribute is not " << attribType2Str(ENUM);                \
    std::cerr << " but "                                                      \
              << attribType2Str(                                              \
                     it->second.interpolations[attributeIndex].type)          \
              << std::endl;                                                   \
    exit(1);                                                                  \
    return nullptr;                                                           \
  }                                                                           \
  return reinterpret_cast<TYPE*>(vertex->attributes[attributeIndex])


float *vs_interpretOutputVertexAttributeAsFloat(
	const GPU gpu, GPUVertexShaderOutput *const vertex,
	const AttribIndex attributeIndex
)
{
	GPU_IMPLEMENTATION_OF_INTERPRETATION_OF_ATTRIB_OF_OUTPUT_VERTEX(float,
		ATTRIB_FLOAT);
}


Vec2 *vs_interpretOutputVertexAttributeAsVec2(
	const GPU gpu, GPUVertexShaderOutput *const vertex,
	const AttribIndex attributeIndex
)
{
	GPU_IMPLEMENTATION_OF_INTERPRETATION_OF_ATTRIB_OF_OUTPUT_VERTEX(Vec2,
		ATTRIB_VEC2);
}


Vec3 *vs_interpretOutputVertexAttributeAsVec3(
	const GPU gpu, GPUVertexShaderOutput *const vertex,
	const AttribIndex attributeIndex
)
{
	GPU_IMPLEMENTATION_OF_INTERPRETATION_OF_ATTRIB_OF_OUTPUT_VERTEX(Vec3,
		ATTRIB_VEC3);
}


Vec4 *vs_interpretOutputVertexAttributeAsVec4(
	const GPU gpu, GPUVertexShaderOutput *const vertex,
	const AttribIndex attributeIndex
)
{
	GPU_IMPLEMENTATION_OF_INTERPRETATION_OF_ATTRIB_OF_OUTPUT_VERTEX(Vec4,
		ATTRIB_VEC4);
}


#define GPU_IMPLEMENTATION_OF_INTERPRETATION_OF_ATTRIB_OF_INPUT_FRAGMENT(TYPE, \
                                                                         ENUM) \
  if (attributeIndex >= MAX_ATTRIBUTES) {                                      \
    printAttribIndexError(attributeIndex, __func__);                           \
    exit(1);                                                                   \
  }                                                                            \
  assert(gpu != nullptr);                                                      \
  auto g  = static_cast<GpuImplementation*>(gpu);                              \
  auto it = g->getProgram(g->activeProgram, __func__);                         \
  if (it == g->programs.end()) exit(1);                                        \
  if (it->second.interpolations[attributeIndex].type != ENUM) {                \
    std::cerr << fceArgError2Str(attributeIndex, __func__)                     \
              << " attribute is not " << attribType2Str(ENUM);                 \
    std::cerr << " but "                                                       \
              << attribType2Str(                                               \
                     it->second.interpolations[attributeIndex].type)           \
              << std::endl;                                                    \
    exit(1);                                                                   \
    return nullptr;                                                            \
  }                                                                            \
  return reinterpret_cast<TYPE const*>(                                        \
      fragment->attributes.attributes[attributeIndex])


const float *fs_interpretInputAttributeAsFloat(
	const GPU gpu, const GPUFragmentShaderInput *const fragment,
	const AttribIndex attributeIndex
)
{
	GPU_IMPLEMENTATION_OF_INTERPRETATION_OF_ATTRIB_OF_INPUT_FRAGMENT(
		float, ATTRIB_FLOAT);
}


const Vec2 *fs_interpretInputAttributeAsVec2(
	const GPU gpu, const GPUFragmentShaderInput *const fragment,
	const AttribIndex attributeIndex
)
{
	GPU_IMPLEMENTATION_OF_INTERPRETATION_OF_ATTRIB_OF_INPUT_FRAGMENT(Vec2,
		ATTRIB_VEC2);
}


const Vec3 *fs_interpretInputAttributeAsVec3(
	const GPU gpu, const GPUFragmentShaderInput *const fragment,
	const AttribIndex attributeIndex
)
{
	GPU_IMPLEMENTATION_OF_INTERPRETATION_OF_ATTRIB_OF_INPUT_FRAGMENT(Vec3,
		ATTRIB_VEC3);
}


const Vec4 *fs_interpretInputAttributeAsVec4(
	const GPU gpu, const GPUFragmentShaderInput *const fragment,
	const AttribIndex attributeIndex
)
{
	GPU_IMPLEMENTATION_OF_INTERPRETATION_OF_ATTRIB_OF_INPUT_FRAGMENT(Vec4,
		ATTRIB_VEC4);
}
