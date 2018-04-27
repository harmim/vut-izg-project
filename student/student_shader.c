/**
 * @file
 * @brief This file contains implemenation of phong vertex and fragment shader.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Dominik Harmim <harmim6@gmail.com>
 */


#include <assert.h>
#include <math.h>

#include <student/student_shader.h>
#include <student/gpu.h>
#include <student/uniforms.h>
#include <student/linearAlgebra.h>


/**
 * Constrain a float value (_x) to lie between
 * two further float values (_min and _max).
 */
#define CLAMPF(_x, _min, _max)                                                 \
	(_x) > (_min) ? fminf((_x), (_max)) : fmaxf((_x), (_min))


/**
 * @addtogroup shader_side Úkoly v shaderech
 * @{
 */
void phong_vertexShader(
	GPUVertexShaderOutput *const output,
	const GPUVertexShaderInput *const input, const GPU gpu
)
{
/**
 * @todo Naimplementujte vertex shader, který transformuje vstupní vrcholy do
 * clip-space.<br>
 * <b>Vstupy:</b><br>
 * Vstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu ve
 * world-space (vec3) a v prvním
 * atributu obsahovat normálu vrcholu ve world-space (vec3).<br>
 * <b>Výstupy:</b><br>
 * Výstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu (vec3)
 * ve world-space a v prvním
 * atributu obsahovat normálu vrcholu ve world-space (vec3).
 * Výstupní vrchol obsahuje pozici a normálu vrcholu proto, že chceme počítat
 * osvětlení ve world-space ve fragment shaderu.<br>
 * <b>Uniformy:</b><br>
 * Vertex shader by měl pro transformaci využít uniformní proměnné obsahující
 * view a projekční matici.
 * View matici čtěte z uniformní proměnné "viewMatrix" a projekční matici
 * čtěte z uniformní proměnné "projectionMatrix".
 * Zachovejte jména uniformních proměnných a pozice vstupních a výstupních
 * atributů.
 * Pokud tak neučiníte, akceptační testy selžou.<br>
 * <br>
 * Využijte vektorové a maticové funkce.
 * Nepředávajte si data do shaderu pomocí globálních proměnných.
 * Pro získání dat atributů použijte příslušné funkce vs_interpret*
 * definované v souboru program.h.
 * Pro získání dat uniformních proměnných použijte příslušné funkce
 * shader_interpretUniform* definované v souboru program.h.
 * Vrchol v clip-space by měl být zapsán do proměnné gl_Position ve výstupní
 * struktuře.<br>
 * <b>Seznam funkcí, které jistě použijete</b>:
 *  - gpu_getUniformsHandle()
 *  - getUniformLocation()
 *  - shader_interpretUniformAsMat4()
 *  - vs_interpretInputVertexAttributeAsVec3()
 *  - vs_interpretOutputVertexAttributeAsVec3()
 */
	assert(output != NULL);
	assert(input != NULL);
	assert(gpu != NULL);

	// get handle to uniforms
	const Uniforms uniforms = gpu_getUniformsHandle(gpu);

	// get view matrix
	const Mat4 *const viewMatrix = shader_interpretUniformAsMat4(
		uniforms, getUniformLocation(gpu, "viewMatrix")
	);
	// get projection matrix
	const Mat4 *const projectionMatrix = shader_interpretUniformAsMat4(
		uniforms, getUniformLocation(gpu, "projectionMatrix")
	);

	// get position attribute
	const Vec3 *const position =
		vs_interpretInputVertexAttributeAsVec3(gpu, input, 0);
	// get normal attribute
	const Vec3 *const normal =
		vs_interpretInputVertexAttributeAsVec3(gpu, input, 1);

	// transform vertices to clip-space
	Mat4 projectionViewMatrix;
	multiply_Mat4_Mat4(&projectionViewMatrix, projectionMatrix, viewMatrix);
	Vec4 positionInWorldSpace;
	copy_Vec3Float_To_Vec4(&positionInWorldSpace, position, 1.f);
	multiply_Mat4_Vec4(
		&output->gl_Position, &projectionViewMatrix, &positionInWorldSpace
	);

	// set output attributes
	copy_Vec3(
		vs_interpretOutputVertexAttributeAsVec3(gpu, output, 0), position
	);
	copy_Vec3(
		vs_interpretOutputVertexAttributeAsVec3(gpu, output, 1), normal
	);
}


/**
 * Constrain a vec3 values to lie between
 * two further float values (min and max).
 *
 * @param v input/output vector
 * @param min minimal value
 * @param max maximal value
 */
static void clampVec3(Vec3 *const v, const float min, const float max)
{
	v->data[0] = CLAMPF(v->data[0], min, max);
	v->data[1] = CLAMPF(v->data[1], min, max);
	v->data[2] = CLAMPF(v->data[2], min, max);
}


void phong_fragmentShader(
	GPUFragmentShaderOutput *const output,
	const GPUFragmentShaderInput *const input, const GPU gpu
)
{
/**
 * @todo Naimplementujte fragment shader, který počítá phongův osvětlovací
 * model s phongovým stínováním.<br>
 * <b>Vstup:</b><br>
 * Vstupní fragment by měl v nultém fragment atributu obsahovat
 * interpolovanou pozici ve world-space a v prvním
 * fragment atributu obsahovat interpolovanou normálu ve world-space.<br>
 * <b>Výstup:</b><br>
 * Barvu zapište do proměnné color ve výstupní struktuře.<br>
 * <b>Uniformy:</b><br>
 * Pozici kamery přečtěte z uniformní proměnné "cameraPosition" a pozici
 * světla přečtěte z uniformní proměnné "lightPosition".
 * Zachovejte jména uniformních proměnný.
 * Pokud tak neučiníte, akceptační testy selžou.<br>
 * <br>
 * Dejte si pozor na velikost normálového vektoru, při lineární interpolaci v
 * rasterizaci může dojít ke zkrácení.
 * Zapište barvu do proměnné color ve výstupní struktuře.
 * Shininess faktor nastavte na 40.f
 * Difuzní barvu materiálu nastavte podle normály povrchu.
 * V případě, že normála směřuje kolmo vzhůru je difuzní barva čistě bílá.
 * V případě, že normála směřuje vodorovně nebo dolů je difuzní barva čiště
 * zelená.
 * Difuzní barvu spočtěte lineární interpolací zelené a bíle barvy pomocí
 * interpolačního parameteru t.
 * Interpolační parameter t spočtěte z y komponenty normály pomocí
 * t = y*y (samozřejmě s ohledem na negativní čísla).
 * Spekulární barvu materiálu nastavte na čistou bílou.
 * Barvu světla nastavte na bílou.
 * Nepoužívejte ambientní světlo.<br>
 * <b>Seznam funkcí, které jistě využijete</b>:
 *  - shader_interpretUniformAsVec3()
 *  - fs_interpretInputAttributeAsVec3()
 */
	assert(output != NULL);
	assert(input != NULL);
	assert(gpu != NULL);

	// get handle to uniforms
	const Uniforms uniforms = gpu_getUniformsHandle(gpu);

	// get camera position
	const Vec3 *const cameraPosition = shader_interpretUniformAsVec3(
		uniforms, getUniformLocation(gpu, "cameraPosition")
	);
	// get light position
	const Vec3 *const lightPosition = shader_interpretUniformAsVec3(
		uniforms, getUniformLocation(gpu, "lightPosition")
	);

	// get position attribute
	const Vec3 *const position =
		fs_interpretInputAttributeAsVec3(gpu, input, 0);
	// get normal attribute
	Vec3 normal = *fs_interpretInputAttributeAsVec3(gpu, input, 1);
	normalize_Vec3(&normal, &normal);

	/******************** phong shading ********************/

	// light = normalize(lightPosition - position)
	Vec3 light;
	sub_Vec3(&light, lightPosition, position);
	normalize_Vec3(&light, &light);

	// camera = normalize(cameraPosition - position))
	Vec3 camera;
	sub_Vec3(&camera, cameraPosition, position);
	normalize_Vec3(&camera, &camera);

	// reflectLightNormal = normalize(-reflect(light, normal))
	Vec3 reflectLightNormal;
	reflect(&reflectLightNormal, &light, &normal);
	multiply_Vec3_Float(&reflectLightNormal, &reflectLightNormal, -1.f);
	normalize_Vec3(&reflectLightNormal, &reflectLightNormal);

	// shininessFaktor = 40
	const float shininessFaktor = 40.f;

	// diffuseColor calculation
	Vec3 diffuseColor;
	const float eps = .001f, nY = normal.data[1];
	if (fabsf(nY - 1.f) <= eps)
	{ // nY == 1 => normal goes vertical up
		// diffuseColor = white
		init_Vec3(&diffuseColor, 1.f, 1.f, 1.f);
	}
	else if (nY < 0 || fabsf(nY) <= eps)
	{ // nY < 0 OR nY == 0 => normal goes down or it is horizontal
		// diffuseColor = green
		init_Vec3(&diffuseColor, 0.f, 1.f, 0.f);
	}
	else
	{ // nY < 1 AND nY > 0
		// diffuseColor = linear interpolation of white and green
		const float t = fabsf(nY * nY);
		init_Vec3(
			&diffuseColor,
			t,   // (1 - 0) * t + 0 => t
			1.f, // (1 - 1) * t + 1 => 1
			t    // (1 - 0) * t + 0 => t
		);
	}

	// diffuse = diffuseColor * max(dot(normal, light), 0)
	Vec3 diffuse;
	multiply_Vec3_Float(
		&diffuse, &diffuseColor, fmaxf(dot_Vec3(&normal, &light), 0.f)
	);
	clampVec3(&diffuse, 0.f, 1.f);

	// specularColor = white
	Vec3 specualrColor;
	init_Vec3(&specualrColor, 1.f, 1.f, 1.f);

	// specular = specularColor
	//     * pow(max(dot(reflectLightNormal, negPosition), 0), shininessFaktor)
	Vec3 specular;
	multiply_Vec3_Float(
		&specular, &specualrColor, powf(
			fmaxf(dot_Vec3(&reflectLightNormal, &camera), 0.f), shininessFaktor
		)
	);
	clampVec3(&specular, 0.f, 1.f);

	// write output color
	Vec3 outputColor;
	add_Vec3(&outputColor, &diffuse, &specular);
	copy_Vec3Float_To_Vec4(&output->color, &outputColor, 1.f);
}
/**
 * @}
 */
