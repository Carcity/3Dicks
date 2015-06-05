#include "QuadTree.h"

QuadTree::QuadTree()
{
}

QuadTree::QuadTree(int levels, vec3 rootBox[8], CameraControl* cc)
{
	cam = cc;
	this->levels = levels;
	for (int c = 0; c < 8; c++)
	{
		root.corners[c] = rootBox[c];
	}
	createQuads(&root, 1);
}

void QuadTree::createQuads(Quad* prevRoot, int level)
{
	prevRoot->children[0] = new Quad();
	prevRoot->children[0]->corners[NWU] = prevRoot->corners[NWU];
	//Calculate middle point of x between relevant corners
	prevRoot->children[0]->corners[NEU] = vec3((prevRoot->corners[NWU].x + prevRoot->corners[NEU].x) *0.5f, prevRoot->corners[NWU].y, prevRoot->corners[NWU].z);
	//Calculate middle point of z between relevant corners
	prevRoot->children[0]->corners[SWU] = vec3(prevRoot->corners[NWU].x, prevRoot->corners[NWU].y, (prevRoot->corners[NWU].z + prevRoot->corners[SWU].z)*0.5f);
	//Take relevant data from previous corners already calculated
	prevRoot->children[0]->corners[SEU] = vec3(prevRoot->children[0]->corners[NEU].x, prevRoot->corners[NWU].y, prevRoot->children[0]->corners[SWU].z);
	prevRoot->children[0]->corners[NWD] = prevRoot->corners[NWD];
	prevRoot->children[0]->corners[NED] = vec3((prevRoot->corners[NWD].x + prevRoot->corners[NED].x) *0.5f, prevRoot->corners[NWD].y, prevRoot->corners[NWD].z);
	prevRoot->children[0]->corners[SWD] = vec3(prevRoot->corners[NWD].x, prevRoot->corners[NWD].y, (prevRoot->corners[NWD].z + prevRoot->corners[SWD].z)*0.5f);
	prevRoot->children[0]->corners[SED] = vec3(prevRoot->children[0]->corners[NED].x, prevRoot->corners[NWD].y, prevRoot->children[0]->corners[SWD].z);

	if (level < levels)
	{
		createQuads(prevRoot->children[0], level + 1);
	}
	else
	{
		prevRoot->children[0]->leaf = true;
	}

	prevRoot->children[1] = new Quad();
	prevRoot->children[1]->corners[NWU] = vec3(prevRoot->children[0]->corners[NEU].x, prevRoot->corners[NWU].y, prevRoot->corners[NWU].z);
	prevRoot->children[1]->corners[NEU] = prevRoot->corners[NEU];
	prevRoot->children[1]->corners[SEU] = vec3(prevRoot->corners[NEU].x, prevRoot->corners[NEU].y, (prevRoot->corners[NEU].z + prevRoot->corners[SEU].z)*0.5f);
	//Middle point, same as earlier
	prevRoot->children[1]->corners[SWU] = vec3(prevRoot->children[0]->corners[SEU].x, prevRoot->corners[NWU].y, prevRoot->children[0]->corners[SEU].z);
	prevRoot->children[1]->corners[NWD] = vec3((prevRoot->corners[NWD].x + prevRoot->corners[NED].x) *0.5f, prevRoot->corners[NWD].y, prevRoot->corners[NWD].z);
	prevRoot->children[1]->corners[NED] = prevRoot->corners[NED];
	prevRoot->children[1]->corners[SED] = vec3(prevRoot->corners[NED].x, prevRoot->corners[NED].y, (prevRoot->corners[NED].z + prevRoot->corners[SED].z)*0.5f);
	prevRoot->children[1]->corners[SWD] = vec3(prevRoot->children[0]->corners[SED].x, prevRoot->corners[NWD].y, prevRoot->children[0]->corners[SED].z);
	if (level < levels)
	{
		createQuads(prevRoot->children[1], level + 1);
	}
	else
	{
		prevRoot->children[1]->leaf = true;
	}

	prevRoot->children[2] = new Quad();
	prevRoot->children[2]->corners[NWU] = vec3(prevRoot->children[0]->corners[SEU].x, prevRoot->corners[NWU].y, prevRoot->children[0]->corners[SEU].z);
	prevRoot->children[2]->corners[NEU] = vec3(prevRoot->children[1]->corners[SEU].x, prevRoot->corners[NEU].y, prevRoot->children[0]->corners[SEU].z);
	prevRoot->children[2]->corners[SEU] = prevRoot->corners[SEU];
	prevRoot->children[2]->corners[SWU] = vec3(prevRoot->children[2]->corners[NWU].x, prevRoot->corners[SWU].y, prevRoot->corners[SWU].z);
	prevRoot->children[2]->corners[NWD] = vec3(prevRoot->children[0]->corners[SED].x, prevRoot->corners[NWD].y, prevRoot->children[0]->corners[SED].z);
	prevRoot->children[2]->corners[NED] = vec3(prevRoot->children[1]->corners[SED].x, prevRoot->corners[NED].y, prevRoot->children[1]->corners[SED].z);
	prevRoot->children[2]->corners[SED] = prevRoot->corners[SED];
	prevRoot->children[2]->corners[SWD] = vec3(prevRoot->children[2]->corners[NWD].x, prevRoot->corners[SWD].y, prevRoot->corners[SWD].z);

	if (level < levels)
	{
		createQuads(prevRoot->children[2], level + 1);
	}
	else
	{
		prevRoot->children[2]->leaf = true;
	}

	prevRoot->children[3] = new Quad();
	prevRoot->children[3]->corners[NWU] = vec3(prevRoot->children[0]->corners[SWU].x, prevRoot->corners[NWU].y, prevRoot->children[0]->corners[SWU].z);
	prevRoot->children[3]->corners[NEU] = vec3(prevRoot->children[0]->corners[SEU].x, prevRoot->corners[NWU].y, prevRoot->children[0]->corners[SEU].z);
	prevRoot->children[3]->corners[SEU] = vec3(prevRoot->children[2]->corners[SWU].x, prevRoot->corners[SWU].y, prevRoot->children[2]->corners[SWU].z);
	prevRoot->children[3]->corners[SWU] = prevRoot->corners[SWU];
	prevRoot->children[3]->corners[NWD] = vec3(prevRoot->children[0]->corners[SWD].x, prevRoot->corners[NWD].y, prevRoot->children[0]->corners[SWD].z);
	prevRoot->children[3]->corners[NED] = vec3(prevRoot->children[0]->corners[SED].x, prevRoot->corners[NWD].y, prevRoot->children[0]->corners[SED].z);
	prevRoot->children[3]->corners[SED] = vec3(prevRoot->children[2]->corners[SWD].x, prevRoot->corners[SWD].y, prevRoot->children[2]->corners[SWD].z);
	prevRoot->children[3]->corners[SWD] = prevRoot->corners[SWD];
	if (level < levels)
	{
		createQuads(prevRoot->children[3], level + 1);
	}
	else
	{
		prevRoot->children[3]->leaf = true;
	}
}

bool QuadTree::addObject(GameObject &obj)
{
	vec3 center = obj.readPos();
	if (center.x < root.corners[SWU].x ||
		center.x > root.corners[SEU].x ||
		center.y < root.corners[SWD].y ||
		center.y > root.corners[SWU].y ||
		center.z < root.corners[SWU].z ||
		center.z > root.corners[NWU].z
		)
	{
		return false;
	}
	
	Quad* iterator = &root;
	bool found = false;
	while (!found)
	{
		int closest = 0;
		vec3 dist = center - iterator->children[closest]->getCenter();
		for (int c = 1; c < 4; c++)
		{
			vec3 newDist = center - iterator->children[c]->getCenter();
			if (sqrt(pow(newDist.x, 2) + pow(newDist.y, 2) + pow(newDist.z, 2)) < sqrt(pow(dist.x, 2) + pow(dist.y, 2) + pow(dist.z, 2)))
			{
				closest = c;
				dist = newDist;
			}
		}
		if (iterator->children[closest]->leaf)
		{
			iterator->children[closest]->addObject(obj);
			found = true;
		}
		else
			iterator = iterator->children[closest];
	}
	return true;
}

void QuadTree::getFrustumCorners(vec3 container[])
{
	float fov = 3.14159f*0.45f;
	float ratio = 640.0f / 480.0f;
	float zNear = 0.5f;
	float zFar = 100.0f;

	vec3 camDir = normalize(cam->getDirection());
	vec3 camPos = cam->getPosition();
	vec3 cUp = normalize(cam->getUpVec());
	vec3 cRight = normalize(cam->getRightVec());
	float tang = (float)tan((fov/180.0f) * 0.5);
	float hNear = tang * zNear;
	float wNear = hNear * ratio;
	float hFar = tang * zFar;
	float wFar = hFar * ratio;
	vec3 cNear = camPos + camDir * zNear;
	vec3 cFar = camPos + camDir * zFar;

	// compute the 4 corners of the frustum on the near plane
	vec3 ntl = cNear + cUp * hNear - cRight * wNear;
	container[0] = ntl;
	vec3 ntr = cNear + cUp * hNear + cRight * wNear;
	container[1] = ntr;
	vec3 nbl = cNear - cUp * hNear - cRight * wNear;
	container[2] = nbl;
	vec3 nbr = cNear - cUp * hNear + cRight * wNear;
	container[3] = nbr;

	// compute the 4 corners of the frustum on the far plane
	vec3 ftl = cFar + cUp * hFar - cRight * wFar;
	container[4] = ftl;
	vec3 ftr = cFar + cUp * hFar + cRight * wFar;
	container[5] = ftr;
	vec3 fbl = cFar - cUp * hFar - cRight * wFar;
	container[6] = fbl;
	vec3 fbr = cFar - cUp * hFar + cRight * wFar;
	container[7] = fbr;
}

vector<GameObject> QuadTree::checkQuadTree()
{
	vector<GameObject> ret;

	vec3 plContain[8];
	getFrustumCorners(plContain);
	vec3 ntl = plContain[0];
	vec3 ntr = plContain[1];
	vec3 nbl = plContain[2];
	vec3 nbr = plContain[3];
	vec3 ftl = plContain[4];
	vec3 ftr = plContain[5];
	vec3 fbl = plContain[6];
	vec3 fbr = plContain[7];
	Plane pl[6];
	pl[0] = Plane(ntr, ntl, ftl);//top plane
	pl[1] = Plane(nbl, nbr, fbr);//bottom plane
	pl[2] = Plane(ntl, nbl, fbl);//left plane
	pl[3] = Plane(nbr, ntr, fbr);//rght plane
	pl[4] = Plane(ntl, ntr, nbr);//near plne
	pl[5] = Plane(ftr, ftl, fbl);//IRL Zora in the farplane

	//Makes sure all the normals are pointing into the frustrum
	//Top and bottom planes
	if (ftr.y > fbl.y) //If the top plane is actually above the bottom plane. This is always true unless we're upside down
	{
		if (pl[0].normal.y > 0) //If the normal of the top plane has a positive y, aka, it is pointing out of the frustrum
			pl[0].normal = pl[0].normal * -1.0f; //Flip it to make sure it's pointing into the frustrum
		if (pl[1].normal.y < 0)
			pl[1].normal = pl[1].normal * -1.0f;
	}
	else //We're upside down
	{
		if (pl[0].normal.y < 0) //If we get here, the top plane is below the bottom plane, and we need the top plane to have a positive y-value on it's normal
			pl[0].normal = pl[0].normal * -1.0f; //Flip it to make sure it's pointing into the frustrum
		if (pl[1].normal.y > 0)
			pl[1].normal = pl[1].normal * -1.0f;
	}
	//Left and right planes
	if (ftl.x > ftr.x) //if the left plane is tothe left of the right plane
	{
		if (pl[2].normal.x > 0) 
			pl[2].normal = pl[2].normal * -1.0f; 
		if (pl[3].normal.x < 0)
			pl[3].normal = pl[3].normal * -1.0f;
	}
	else //we've turned around, making the "left" plane of the frurstum be to the right of the "right" plane, in world space
	{
		if (pl[2].normal.x < 0) 
			pl[2].normal = pl[2].normal * -1.0f; 
		if (pl[3].normal.x > 0)
			pl[3].normal = pl[3].normal * -1.0f;
	}
	//Near and far planes (Wherever you are)
	if (nbl.z > fbr.z) //i the farplan is deeper than the nearplane
	{
		if (pl[4].normal.z > 0)
			pl[4].normal = pl[4].normal * -1.0f;
		if (pl[5].normal.z < 0)
			pl[5].normal = pl[5].normal * -1.0f;
	}
	else //we've turned around, making the "far" plane of the frurstum be more shallow than the near plane, in world space
	{
		if (pl[4].normal.z < 0)
			pl[4].normal = pl[4].normal * -1.0f;
		if (pl[5].normal.z > 0)
			pl[5].normal = pl[5].normal * -1.0f;
	}

	for (int c = 0; c < 4; c++)
	{
		checkQuads(root.children[c], pl, &ret);
	}
	return ret;
}

void QuadTree::checkQuads(Quad* quad, Plane pl[6], vector<GameObject>* ret)
{
	bool out = false;

	for (int i = 0; i < 6; i++) {

		// reset counters for corners in and out
		
		// for each corner of the box do ...
		// get out of the cycle as soon as a box as corners
		// both inside and out of the frustum
		//for (int k = 0; k < 8 && !out; k++) {

			// is the corner outside or inside
			if (cos(pl[i].distance(quad->getCenter())) >= 0)//quad->corners[k])) >= 0)
				out = true;

		//}
	}
		//any of the dot products are negative
		if (out)
		{
			//DO NOT RENDER
		}

		//all of them are positive
		else
		{
			if (quad->leaf)
			{
				for (int i = 0; i < quad->nrOfObjects; i++)
				{
					ret->push_back(*quad->objects[i]);
				}
			}
			else
			{
				for (int c = 0; c < 4; c++)
				{
					checkQuads(quad->children[c], pl, ret);
				}
			}
		}
	
}

void QuadTree::copyChildren(Quad* quad, const Quad* copy)
{
	for (int c = 0; c < 4; c++)
	{
		quad->children[c] = new Quad(*copy->children[c]);
		if (!quad->children[c]->leaf)
		{
			for (int i = 0; i < 4; i++)
			{
				copyChildren(quad->children[c], copy->children[i]);
			}
		}
	}
}

QuadTree& QuadTree::operator=(const QuadTree& copy)
{
	this->cam = copy.cam;
	this->levels = copy.levels;
	this->root = copy.root;
	copyChildren(&this->root, &copy.root);
	return *this;
}