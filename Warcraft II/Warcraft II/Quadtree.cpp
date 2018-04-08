#include "Quadtree.h"

// This is used for debug purpose, to draw subdivisions done
std::vector<SDL_Rect> QuadtreeAABBs;

// Insert new point in the quadtree
bool AABB::Insert(iPoint* newpoint)
{
	// If new point is not in the quadtree AABB, return
	if (!contains(newpoint))
		return false;

	// If in this node there is space for the point, pushback it
	if (objects.size() < Max_Elements_in_Same_Node)
	{
		objects.push_back(*newpoint);
		return true;
	}

	// Otherwise, subdivide and add the point to one of the new nodes
	if (children[0] == nullptr)
		subdivide();

	for (uint i = 0; i < 4; i++)
		if (children[i]->Insert(newpoint))
			return true;

	return false;
}

// Subdivade this quadtree node into 4 smaller children
void AABB::subdivide()
{
	// Get subdivision size
	iPoint qSize = { (int)(aabb.w * 0.5), (int)(aabb.h * 0.5) };
	iPoint qCentre;

	//Calculate new AABB centre for each child
	qCentre = { aabb.x,aabb.y };
	children[0] = new AABB({ qCentre.x,qCentre.y,qSize.x,qSize.y });
	children[0]->root = this;
	QuadtreeAABBs.push_back({ qCentre.x,qCentre.y,qSize.x,qSize.y }); // Tis line is for debug draw purpose

	qCentre = { aabb.x + qSize.x,aabb.y };
	children[1] = new AABB({ qCentre.x,qCentre.y,qSize.x,qSize.y });
	children[1]->root = this;
	QuadtreeAABBs.push_back({ qCentre.x,qCentre.y,qSize.x,qSize.y }); // Tis line is for debug draw purpose

	qCentre = { aabb.x,aabb.y + qSize.y };
	children[2] = new AABB({ qCentre.x,qCentre.y,qSize.x,qSize.y });
	children[2]->root = this;
	QuadtreeAABBs.push_back({ qCentre.x,qCentre.y,qSize.x,qSize.y }); // Tis line is for debug draw purpose

	qCentre = { aabb.x + qSize.x,aabb.y + qSize.y };
	children[3] = new AABB({ qCentre.x,qCentre.y,qSize.x,qSize.y });
	children[3]->root = this;
	QuadtreeAABBs.push_back({ qCentre.x,qCentre.y,qSize.x,qSize.y }); // Tis line is for debug draw purpose
}

int AABB::CollectCandidates(std::vector< iPoint* >& nodes, const SDL_Rect& r)
{
	uint ret = 0;

	// If range is not in the quadtree, return
	if(!AABB(aabb).intersects(&AABB(r)))
		return ret;

	// See if the points of this node are in range and pushback them to the vector
	if (!objects.empty())
		for (int i = 0; i < objects.size(); i++)
		{
			ret++;
			if (AABB(r).contains(&objects[i]))
				nodes.push_back(&objects[i]);
		}
			

	// If there is no children, end
	if (children[0] == nullptr)
		return ret;

	// Otherwise, add the points from the children
	ret += children[0]->CollectCandidates(nodes, r);
	ret += children[1]->CollectCandidates(nodes, r);
	ret += children[2]->CollectCandidates(nodes, r);
	ret += children[3]->CollectCandidates(nodes, r);

	return ret;
}