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
	illum{}
{
	;
}

Material::Material(fastObjMaterial& mat) {
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