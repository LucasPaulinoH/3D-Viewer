#include "GOSpawner.h"


GeometricObject GOSpawner::box() {
	Box box(2.0f, 2.0f, 2.0f);
	GeometricObject boxGO(box, DEFAULT_COLOR, viewportsQuantity, sizeOfConstants);

	return boxGO;
}

GeometricObject GOSpawner::cylinder() {
	Cylinder cylinder(1.0f, 0.5f, 3.0f, 20, 20);
	GeometricObject cylinderGO(cylinder, DEFAULT_COLOR, viewportsQuantity, sizeOfConstants);

	return cylinderGO;
}

GeometricObject GOSpawner::geosphere() {
	GeoSphere geosphere(2.0f, 5);
	GeometricObject geosphereGO(geosphere, DEFAULT_COLOR, viewportsQuantity, sizeOfConstants);

	return geosphereGO;
}

GeometricObject GOSpawner::plane() {
	Grid grid(2.0f, 2.0f, 20, 20);
	GeometricObject gridGO(grid, DEFAULT_COLOR, viewportsQuantity, sizeOfConstants);

	return gridGO;
}

GeometricObject GOSpawner::quad() {
	Quad quad(2.0f, 2.0f);
	GeometricObject quadGO(quad, DEFAULT_COLOR, viewportsQuantity, sizeOfConstants);

	return quadGO;
}

GeometricObject GOSpawner::sphere() {
	Sphere sphere(1.0f, 20, 20);
	GeometricObject sphereGO(sphere, DEFAULT_COLOR, viewportsQuantity, sizeOfConstants);

	return sphereGO;
}

GeometricObject GOSpawner::generateGO(Input * input) {
	if (input->KeyDown('B')) return box();
	if (input->KeyDown('C')) return cylinder();
	if (input->KeyDown('G')) return geosphere();
	if (input->KeyDown('P')) return plane();
	if (input->KeyDown('Q')) return quad();
	if (input->KeyDown('S')) return sphere();
}
