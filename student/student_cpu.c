/**
 * @file
 * @brief This file contains implementation of cpu side for phong shading.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Dominik Harmim <harmim6@gmail.com>
 */


#include <assert.h>
#include <math.h>

#include <student/student_cpu.h>
#include <student/fwd.h>
#include <student/gpu.h>
#include <student/linearAlgebra.h>
#include <student/mouseCamera.h>
#include <student/swapBuffers.h>
#include <student/uniforms.h>
#include <student/program.h>
#include <student/student_shader.h>
#include <student/buffer.h>
#include <student/bunny.h>
#include <student/vertexPuller.h>
#include <student/globals.h>
#include <student/student_pipeline.h>


/**
 * @brief This structure contains all global variables for this method.
 */
static struct PhongVariables
{
	/// This variable contains GPU handle.
	GPU gpu;
	/// This variable contains light poistion in world-space.
	Vec3 lightPosition;
	/// This variable contains shader program.
	ProgramID program;
	/// This variable contains vertex puller.
	VertexPullerID puller;
} phong; ///<instance of all global variables for phong


/**
 * @addtogroup cpu_side Úkoly v cpu části
 * @{
 */
void phong_onInit(int32_t width, int32_t height)
{
	// create gpu
	phong.gpu = cpu_createGPU();
	// set viewport size
	cpu_setViewportSize(phong.gpu, (size_t) width, (size_t) height);
	// init matrices
	cpu_initMatrices(width, height);
	// init lightPosition
	init_Vec3(&phong.lightPosition, 1000.f, 1000.f, 1000.f);

/**
 * @todo Doprogramujte inicializační funkci.
 * Zde byste měli vytvořit buffery na GPU, nahrát data do bufferů, vytvořit
 * vertex puller a správně jej nakonfigurovat, vytvořit program, připojit k
 * němu shadery a nastavit interpolace.
 * Také byste zde měli zarezervovat unifromní proměnné pro matice, pozici
 * kamery, světla a další vaše proměnné.
 * Do bufferů nahrajte vrcholy králička (pozice, normály) a indexy na vrcholy
 * ze souboru bunny.h.
 * Shader program by měl odkazovat na funkce/shadery v souboru
 * student_shader.h.
 * V konfiguraci vertex pulleru nastavte dvě čtecí hlavy.
 * Jednu pro pozice vrcholů a druhou pro normály vrcholů.
 * Nultý vertex/fragment atribut by měl obsahovat pozici vertexu.
 * První vertex/fragment atribut by měl obsahovat normálu vertexu.
 * Budete využívat minimálně 4 uniformní proměnné
 * Uniformní proměnná pro view matici by měla být pojmenována "viewMatrix".
 * Uniformní proměnná pro projekční matici by měla být pojmenována
 * "projectionMatrix".
 * Uniformní proměnná pro pozici kamery by se měla jmenovat "cameraPosition".
 * Uniformní proměnná pro pozici světla by se měla jmenovat "lightPosition".
 * Je důležité udržet pozice atributů a názvy uniformních proměnných z důvodu
 * akceptačních testů.
 * Interpolace vertex atributů do fragment atributů je také potřeba nastavit.
 * Oba vertex atributy nastavte na \link SMOOTH\endlink interpolaci -
 * perspektivně korektní interpolace.<br>
 * <b>Seznam funkcí, které jistě využijete:</b>
 *  - cpu_reserveUniform()
 *  - cpu_createProgram()
 *  - cpu_attachVertexShader()
 *  - cpu_attachFragmentShader()
 *  - cpu_setAttributeInterpolation()
 *  - cpu_createBuffers()
 *  - cpu_bufferData()
 *  - cpu_createVertexPullers()
 *  - cpu_setVertexPullerHead()
 *  - cpu_enableVertexPullerHead()
 *  - cpu_setIndexing()
 */

	// reserve uniforms
	cpu_reserveUniform(phong.gpu, "viewMatrix", UNIFORM_MAT4);
	cpu_reserveUniform(phong.gpu, "projectionMatrix", UNIFORM_MAT4);
	cpu_reserveUniform(phong.gpu, "cameraPosition", UNIFORM_VEC3);
	cpu_reserveUniform(phong.gpu, "lightPosition", UNIFORM_VEC3);

	// create program
	phong.program = cpu_createProgram(phong.gpu);

	// attach vertex shaders
	cpu_attachVertexShader(
		phong.gpu, phong.program, (VertexShader) phong_vertexShader
	);
	cpu_attachFragmentShader(
		phong.gpu, phong.program, (FragmentShader) phong_fragmentShader
	);

	// set attribute interpolation
	cpu_setAttributeInterpolation( // vertex position
		phong.gpu, phong.program, 0, ATTRIB_VEC3, SMOOTH
	);
	cpu_setAttributeInterpolation( // vertex normal
		phong.gpu, phong.program, 1, ATTRIB_VEC3, SMOOTH
	);

	// create buffers
	BufferID bunnyVerticesBuffer;
	cpu_createBuffers(phong.gpu, 1, &bunnyVerticesBuffer);
	BufferID bunnyIndicesBuffer;
	cpu_createBuffers(phong.gpu, 1, &bunnyIndicesBuffer);

	// set data to buffers
	cpu_bufferData(
		phong.gpu, bunnyVerticesBuffer,
		sizeof(bunnyVertices) / sizeof(BunnyVertex), bunnyVertices
	);
	cpu_bufferData(
		phong.gpu, bunnyIndicesBuffer,
		sizeof(bunnyIndices) / sizeof(VertexIndex), bunnyIndices
	);

	// create vertex puller
	cpu_createVertexPullers(phong.gpu, 1, &phong.puller);

	// set vertex puller heads
	const size_t stride = sizeof(float) * 6;
	cpu_setVertexPullerHead( // vertex position head
		phong.gpu, phong.puller, 0, bunnyVerticesBuffer, 0, stride
	);
	cpu_setVertexPullerHead( // vertex normal head
		phong.gpu, phong.puller, 1, bunnyVerticesBuffer, sizeof(float) * 3,
		stride
	);

	// enable vertex puller heads
	// vertex position head
	cpu_enableVertexPullerHead(phong.gpu, phong.puller, 0);
	// vertex normal head
	cpu_enableVertexPullerHead(phong.gpu, phong.puller, 1);

	// set vertex puller indexing
	cpu_setIndexing(
		phong.gpu, phong.puller, bunnyIndicesBuffer, sizeof(uint32_t)
	);
}
/**
 * @}
 */


void phong_onExit()
{
	cpu_destroyGPU(phong.gpu);
}


/**
 * @addtogroup cpu_side
 * @{
 */
void phong_onDraw(SDL_Surface *surface)
{
	assert(surface != NULL);

	// clear depth buffer
	cpu_clearDepth(phong.gpu, +INFINITY);
	// clear color buffer
	Vec4 color;
	init_Vec4(&color, .1f, .1f, .1f, 1.f);
	cpu_clearColor(phong.gpu, &color);

/**
 * @todo Doprogramujte kreslící funkci.
 * Zde byste měli aktivovat shader program, aktivovat vertex puller, nahrát
 * data do uniformních proměnných a
 * vykreslit trojúhelníky pomocí funkce cpu_drawTriangles.
 * Data pro uniformní proměnné naleznete v externích globálních proměnnénych
 * viewMatrix, projectionMatrix, cameraPosition a globální proměnné
 * phong.lightPosition.<br>
 * <b>Seznam funkcí, které jistě využijete:</b>
 *  - cpu_useProgram()
 *  - cpu_bindVertexPuller()
 *  - cpu_uniformMatrix4fv()
 *  - cpu_uniform3f()
 *  - cpu_drawTriangles()
 *  - getUniformLocation()
 */

	// activate shader program
	cpu_useProgram(phong.gpu, phong.program);

	// activate vertex puller
	cpu_bindVertexPuller(phong.gpu, phong.puller);

	// set view matrix uniform data
	cpu_uniformMatrix4fv(
		phong.gpu, getUniformLocation(phong.gpu, "viewMatrix"),
		(float *) &viewMatrix
	);
	// set view matrix uniform data
	cpu_uniformMatrix4fv(
		phong.gpu, getUniformLocation(phong.gpu, "projectionMatrix"),
		(float *) &projectionMatrix
	);
	// set view matrix uniform data
	cpu_uniform3f(
		phong.gpu, getUniformLocation(phong.gpu, "cameraPosition"),
		cameraPosition.data[0],
		cameraPosition.data[1],
		cameraPosition.data[2]
	);
	// set view matrix uniform data
	cpu_uniform3f(
		phong.gpu, getUniformLocation(phong.gpu, "lightPosition"),
		phong.lightPosition.data[0],
		phong.lightPosition.data[1],
		phong.lightPosition.data[2]
	);

	// let's draw
	cpu_drawTriangles(phong.gpu, sizeof(bunnyVertices) / sizeof(BunnyVertex));

	// copy image from gpu to SDL surface
	cpu_swapBuffers(surface, phong.gpu);
}
/**
 * @}
 */
