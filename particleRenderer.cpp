#include "particleRenderer.h"
#include "glParticleRenderer.h"

#include <string>

using namespace std;

std::shared_ptr<IParticleRenderer> RendererFactory :: create (const char *name)
{
	std::string renderer(name);

	if(renderer == "gl")
		return std::make_shared<GLParticleRenderer>();
}
