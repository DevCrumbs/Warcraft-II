#ifndef QUADTREE_H_
#define QUADTREE_H_

#include <vector>

#include "p2Point.h"
#include "SDL\include\SDL_rect.h"

static const uint Max_Elements_in_Same_Node = 4;

// Axis-Aligned Bounding Box
// This is the rectangles used by the quadtree to subdivide the space
struct AABB
{
	//The box itself
	SDL_Rect aabb;

	// Points in this quad tree node
	std::vector <iPoint> objects;

	//From what node this comes?
	AABB* root;

	// 4 children, the "magic number" 4 you will see is for that
	AABB* children[4];
	// 0 = nw child
	// 1 = ne child
	// 2 = sw child
	// 3 = se child

	// Creates the rectangle from an SDL_Rect
	AABB(SDL_Rect r): aabb(r)
	{
		for (uint i = 0; i < 4; i++)
			children[i] = nullptr;
	}

	~AABB()
	{
		for (int i = 0; i < 4; ++i)
			if (children[i] != nullptr) 
				delete children[i];
	}

	// See if the point is in the rectangle, including its boundaries. The code in 
	// SDL_PointInRect can be modified to adapt witch points are considered "in"
	bool contains(iPoint* a) const
	{
		SDL_Point point = { a->x,a->y };
		return SDL_PointInRect(&point, &aabb);
	}

	// See if this rectangle intersects with other
	bool intersects(const AABB* other) const
	{
		return SDL_HasIntersection(&aabb, &other->aabb);
	}

	// Insert new point in the quadtree
	bool Insert(iPoint* newpoint);

	// Subdivade this quadtree node into 4 smaller children
	void subdivide();

	int CollectCandidates(std::vector< iPoint* >& nodes, const SDL_Rect& r);

	void CollectPoints(std::vector< AABB* >& nodes)
	{
		nodes.push_back(this);
		for (int i = 0; i < 4; ++i)
			if (children[i] != NULL) children[i]->CollectPoints(nodes);
	}
};

class Quadtree
{
public:
	AABB* root = nullptr;

public:
	Quadtree()
	{

	}

	Quadtree(const SDL_Rect& r)
	{
		SetBoundaries(r);
	}

	~Quadtree()
	{
		Clear();
	}

	//Change the root AABB
	void SetBoundaries(const SDL_Rect& r)
	{
		if (root != NULL)
			delete root;

		root = new AABB(r);
	}

	// Insert new point in the quadtree
	bool Insert(iPoint* newpoint)
	{
		bool ret = false;
		if (root != NULL)
		{
			if (root->contains(newpoint))
				ret = root->Insert(newpoint);
		}
		return ret;
	}

	// Clear the tree
	void Clear()
	{
		if (root != NULL)
		{
			delete root;
			root = NULL;
		}
	}

	int CollectCandidates(std::vector< iPoint* >& nodes, const SDL_Rect& r) const
	{
		int tests = 1;
		
		if (root != NULL)
			if(AABB(root->aabb).intersects(&AABB(r)))
				tests = root->CollectCandidates(nodes, r);
			
		return tests;
	}

	void CollectPoints(std::vector< AABB* >& nodes) const
	{
		if (root != NULL)
			root->CollectPoints(nodes);
	}
};

// This is used for debug purpose, to draw subdivisions done
extern std::vector<SDL_Rect> QuadtreeAABBs;

#endif