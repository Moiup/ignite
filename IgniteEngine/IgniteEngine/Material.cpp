#include "Material.h"

Material::Material() :
	Ka{},
	Kd{},
	Ks{},
	Ke{},
	Kt{},
	Ns{},
	Ni{},
	Tf{},
	d{},
	illum{},
	map_Ka{ -1 },
	map_Kd{ -1 },
	map_Ks{ -1 },
	map_Ke{ -1 },
	map_Kt{ -1 },
	map_Ns{ -1 },
	map_Ni{ -1 },
	map_d{ -1 },
	map_bump{ -1 }
{
	;
}

Material::Material(fastObjMaterial& mat) :
	Material::Material()
{
	setMaterial(mat);
}

const void Material::setMaterial(fastObjMaterial& mat) {
	Ka.r = mat.Ka[0];
	Ka.g = mat.Ka[1];
	Ka.b = mat.Ka[2];

	Kd.r = mat.Kd[0];
	Kd.g = mat.Kd[1];
	Kd.b = mat.Kd[2];

	Ks.r = mat.Ks[0];
	Ks.g = mat.Ks[1];
	Ks.b = mat.Ks[2];

	Ke.r = mat.Ke[0];
	Ke.g = mat.Ke[1];
	Ke.b = mat.Ke[2];

	Kt.r = mat.Kt[0];
	Kt.g = mat.Kt[1];
	Kt.b = mat.Kt[2];

	Ns = mat.Ns;
	Ni = mat.Ni;

	Tf.r = mat.Tf[0];
	Tf.g = mat.Tf[1];
	Tf.b = mat.Tf[2];

	d = mat.d;
	illum = mat.illum;
}

glsl::Mat::Mat() :
	Ka{},
	map_Ka{ -1 },
	Kd{},
	map_Kd{ -1 },
	Ks{},
	map_Ks{ -1 },
	Ke{},
	map_Ke{ -1 },
	Kt{},
	map_Kt{ -1 },
	Tf{},
	pad0{},
	Ns{},
	Ni{},
	d{},
	illum{},
	map_Ns{ -1 },
	map_Ni{ -1 },
	map_d{ -1 },
	map_bump{ -1 }
{ ; }

glsl::Mat::Mat(Material mat) :
	glsl::Mat::Mat()
{
	setMaterial(mat);
}

const void glsl::Mat::setMaterial(Material mat) {
	Ka.r = mat.Ka[0];
	Ka.g = mat.Ka[1];
	Ka.b = mat.Ka[2];

	Kd.r = mat.Kd[0];
	Kd.g = mat.Kd[1];
	Kd.b = mat.Kd[2];

	map_Kd = mat.map_Kd;
	std::cout << mat.map_Kd << std::endl;

	Ks.r = mat.Ks[0];
	Ks.g = mat.Ks[1];
	Ks.b = mat.Ks[2];

	Ke.r = mat.Ke[0];
	Ke.g = mat.Ke[1];
	Ke.b = mat.Ke[2];

	Kt.r = mat.Kt[0];
	Kt.g = mat.Kt[1];
	Kt.b = mat.Kt[2];

	Ns = mat.Ns;
	Ni = mat.Ni;

	Tf.r = mat.Tf[0];
	Tf.g = mat.Tf[1];
	Tf.b = mat.Tf[2];

	d = mat.d;
	illum = mat.illum;
}