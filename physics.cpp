#pragma once

#include "physics.h"

#include "vulkantypes.h"
#include "Actors.h"
#include <list>
#include <cfloat>


//using Actors::StaticActor;
//using Actors::UnStaticActor;
using Actors::Actor3;

namespace physics {
	bool AABBCheckCollision(GameRect one, GameRect two) // AABB - AABB collision
	{
		// collision x-axis?
		bool collisionX = one.posx + one.sizex >= two.posx &&
			two.posx + two.sizex >= one.posx;
		// collision y-axis?
		bool collisionY = one.posy + one.sizey >= two.posy &&
			two.posy + two.sizey >= one.posy;
		// collision only if on both axes
		return collisionX && collisionY;
	}

	//will mutate actors' velocities if collision based on direction/overlaps
	//template <typename T*>
	//void AABBCollisionBatBallDirection(Actor3<Vertex>& ball, Actor3<Vertex>& bat, KeyboardControls& kbctrler, ps4Controller& ctrler ) {
	//	GameRect ballrect = ball.getAABBgamerect();
	//	GameRect batrect = bat.getAABBgamerect();

	//	float dx = (ballrect.posx + ballrect.sizex / 2) - (batrect.posx + batrect.sizex / 2);
	//	float dy = (ballrect.posy + ballrect.sizey / 2) - (batrect.posy + batrect.sizey / 2);
	//	float colwidth = (ballrect.sizex + batrect.sizex) / 2;
	//	float colheight = (ballrect.sizey + batrect.sizey) / 2;
	//	float crosswidth = colwidth * dy;
	//	float crossheight = colheight * dx;

	//	const float batfactor = 0.0025f;
	//	float batx = 0.0f, baty = 0.0f;
	//	//too much duplication here from bat actor 
	//	if (kbctrler.a || ctrler.ctrlrstate.dpadleft || ctrler.ctrlrstate.lstickLR < -0.1)
	//		batx = -batfactor;
	//	if (kbctrler.d || ctrler.ctrlrstate.dpadright || ctrler.ctrlrstate.lstickLR > 0.1)
	//		batx = batfactor;
	//	if (kbctrler.w || ctrler.ctrlrstate.dpadup || ctrler.ctrlrstate.lstickUD < -0.1)
	//		baty = -batfactor;
	//	if (kbctrler.s || ctrler.ctrlrstate.dpaddown || ctrler.ctrlrstate.lstickUD > 0.1)
	//		baty = batfactor;


	//	if ((std::abs(dx) <= colwidth) && std::abs(dy) <= colheight) {
	//		if (crosswidth > crossheight) {
	//			if (crosswidth > (-crossheight)) {
	//				//bottom
	//				ball.velocity.y = 0.0f - ball.velocity.y + baty;
	//				ball.position.y += 0.1f;
	//			}
	//			else {
	//				//left
	//				ball.velocity.x = 0.0f - ball.velocity.x - batx;
	//				ball.position.x -= 0.1f;
	//			}
	//		}
	//		else {
	//			if (crosswidth > (-crossheight)) {
	//				//right
	//				ball.velocity.x = 0.0f - ball.velocity.x - batx;
	//				ball.position.x += 0.1f;
	//			}
	//			else {
	//				//top
	//				ball.velocity.y = 0.0f - ball.velocity.y + baty;
	//				ball.position.y -= 0.1f;
	//			}
	//		}
	//	}
	//}

	//glm::vec2 getAABBCollisionRectRectDirection(GameRect movingRect, GameRect staticRect, KeyboardControls& kbctrler, ps4Controller& ctrler) {
	//	GameRect ballrect = movingRect; // .getAABBgamerect();
	//	GameRect batrect = staticRect; // .getAABBgamerect();
	//	glm::vec2 retvec{};

	//	float dx = (ballrect.posx + ballrect.sizex / 2) - (batrect.posx + batrect.sizex / 2);
	//	float dy = (ballrect.posy + ballrect.sizey / 2) - (batrect.posy + batrect.sizey / 2);
	//	float colwidth = (ballrect.sizex + batrect.sizex) / 2;
	//	float colheight = (ballrect.sizey + batrect.sizey) / 2;
	//	float crosswidth = colwidth * dy;
	//	float crossheight = colheight * dx;

	//	/*const float batfactor = 0.0025f;
	//	float batx = 0.0f, baty = 0.0f;*/
	//	//too much duplication here from bat actor 
	//	/*if (kbctrler.a || ctrler.ctrlrstate.dpadleft || ctrler.ctrlrstate.lstickLR < -0.1)
	//		batx = -batfactor;
	//	if (kbctrler.d || ctrler.ctrlrstate.dpadright || ctrler.ctrlrstate.lstickLR > 0.1)
	//		batx = batfactor;
	//	if (kbctrler.w || ctrler.ctrlrstate.dpadup || ctrler.ctrlrstate.lstickUD < -0.1)
	//		baty = -batfactor;
	//	if (kbctrler.s || ctrler.ctrlrstate.dpaddown || ctrler.ctrlrstate.lstickUD > 0.1)
	//		baty = batfactor;*/


	//	if ((std::abs(dx) <= colwidth) && std::abs(dy) <= colheight) {
	//		if (crosswidth > crossheight) {
	//			if (crosswidth > (-crossheight)) {
	//				//bottom
	//				/*ball.velocity.y = 0.0f - movingRect.velocity.y + baty;
	//				ball.position.y += 0.1f;*/
	//				retvec = { 0.0f,1.0f };
	//			}
	//			else {
	//				//left
	//				/*ball.velocity.x = 0.0f - movingRect.velocity.x - batx;
	//				ball.position.x -= 0.1f;*/
	//				retvec = { -1.0f,0.0f };
	//			}
	//		}
	//		else {
	//			if (crosswidth > (-crossheight)) {
	//				//right
	//				/*ball.velocity.x = 0.0f - movingRect.velocity.x - batx;
	//				ball.position.x += 0.1f;*/
	//				retvec = { 1.0f,0.0f };
	//			}
	//			else {
	//				//top
	//				/*ball.velocity.y = 0.0f - movingRect.velocity.y + baty;
	//				ball.position.y -= 0.1f;*/
	//				retvec = { 0.0f,-1.0f };
	//			}
	//		}
	//	}

	//	return retvec;
	//}

	//void AABBCollisionWallBallDirection(Actor& ball, StaticActor& wall) {
	//	float dx = (ball.gamerect.posx + ball.gamerect.sizex / 2) - (wall.gamerect.posx + wall.gamerect.sizex / 2);
	//	float dy = (ball.gamerect.posy + ball.gamerect.sizey / 2) - (wall.gamerect.posy + wall.gamerect.sizey / 2);
	//	float colwidth = (ball.gamerect.sizex + wall.gamerect.sizex) / 2;
	//	float colheight = (ball.gamerect.sizey + wall.gamerect.sizey) / 2;
	//	float crosswidth = colwidth * dy;
	//	float crossheight = colheight * dx;

	//	if ((std::abs(dx) <= colwidth) && std::abs(dy) <= colheight) {
	//		if (crosswidth > crossheight) {
	//			if (crosswidth > (-crossheight)) {
	//				//bottom
	//				ball.yvel = 0.0f - ball.yvel;
	//				ball.gamerect.posy += 0.2f;
	//			}
	//			else {
	//				//left
	//				ball.xvel = 0.0f - ball.xvel;
	//				ball.gamerect.posx -= 0.2f;
	//			}
	//		}
	//		else {
	//			if (crosswidth > (-crossheight)) {
	//				//right
	//				ball.xvel = 0.0f - ball.xvel;
	//				ball.gamerect.posx += 0.2f;
	//			}
	//			else {
	//				//top
	//				ball.yvel = 0.0f - ball.yvel;
	//				ball.gamerect.posy -= 0.2f;
	//			}
	//		}
	//	}
	//}

	//void AABBCollisionBallBallDirection(Actor2<Vertex>& ball1, Actor2<Vertex>& ball2) {
	//	float dx = (ball1.position.x + ball1.gamerect.sizex / 2) - (ball2.position.x + ball2.gamerect.sizex / 2);
	//	float dy = (ball1.position.y + ball1.gamerect.sizey / 2) - (ball2.position.y + ball2.gamerect.sizey / 2);
	//	float colwidth = (ball1.gamerect.sizex + ball2.gamerect.sizex) / 2;
	//	float colheight = (ball1.gamerect.sizey + ball2.gamerect.sizey) / 2;
	//	float crosswidth = colwidth * dy;
	//	float crossheight = colheight * dx;

	//	float damping = 0.7f;

	//	if ((std::abs(dx) <= colwidth) && std::abs(dy) <= colheight) {
	//		if (crosswidth > crossheight) {
	//			if (crosswidth > (-crossheight)) {
	//				//bottom
	//				//float avYvel = (ball1.yvel + ball2.yvel) / 2.0f;
	//				float tmpyvel = ball1.velocity.y;
	//				ball1.velocity.y = ball2.velocity.y * damping; //(0.0f - avYvel) * damping;
	//				ball1.position.y += 0.01f;
	//				ball2.velocity.y = tmpyvel * damping; // (0.0f + avYvel)* damping;
	//				ball2.gamerect.posy -= 0.01f;
	//			}
	//			else {
	//				//left
	//				//float avXvel = (ball1.xvel + ball2.xvel) / 2.0f;
	//				float tmpxvel = ball1.velocity.x;
	//				ball1.velocity.x = ball2.velocity.x * damping;  //(0.0f - avXvel) * damping;
	//				ball1.position.x -= 0.01f;
	//				ball2.velocity.x = tmpxvel * damping; // (0.0f + avXvel)* damping;
	//				ball2.position.x += 0.01f;
	//			}
	//		}
	//		else {
	//			if (crosswidth > (-crossheight)) {
	//				//right
	//				//float avXvel = (ball1.xvel + ball2.xvel) / 2.0f;
	//				float tmpxvel = ball1.velocity.x;
	//				ball1.velocity.x = ball2.velocity.x * damping;  //(0.0f - avXvel) * damping;
	//				ball1.position.x += 0.01f;
	//				ball2.velocity.x = tmpxvel * damping; // (0.0f + avXvel)* damping;
	//				ball2.position.x -= 0.01f;
	//			}
	//			else {
	//				//top
	//				//float avYvel = (ball1.yvel + ball2.yvel) / 2.0f;
	//				float tmpyvel = ball1.velocity.y;
	//				ball1.velocity.y = ball2.velocity.y * damping;  //(0.0f + avYvel) * damping;
	//				ball1.position.y += 0.01f;
	//				ball2.velocity.y = tmpyvel * damping; // (0.0f - avYvel)* damping;
	//				ball2.position.y -= 0.01f;
	//			}
	//		}
	//	}
	//}

	//probably inefficient, broad phase should be done first to minimise balls vec size
	//void collideAABBBalls(std::list<Actor2<Vertex>*>& balls) {

	//	std::list<Actor2<Vertex>*> testballs{ balls };
	//	std::list<Actor2<Vertex>*> nextballs;
	//	while (testballs.size() > 0) {
	//		std::list<Actor2<Vertex>*> colliders;
	//		Actor2<Vertex>* collball1 = testballs.back();
	//		testballs.pop_back();
	//		if (testballs.empty()) {// we were the only ball left
	//			continue;
	//		}
	//		else {
	//			while (testballs.size() > 0) {
	//				Actor2<Vertex>* collball2 = testballs.back();
	//				testballs.pop_back();

	//				if (AABBCheckCollision(collball1->getAABBgamerect(), collball2->getAABBgamerect())) {
	//					colliders.push_back(collball2);
	//				}
	//				else {
	//					//push it back to test for it's own colls
	//					nextballs.push_back(collball2);
	//				}
	//			}
	//		}
	//		//need to make this work properly for multi/chain collisions
	//		for (auto& collballi : colliders) {
	//			AABBCollisionBallBallDirection(*collball1, *collballi);
	//		}

	//		testballs = nextballs;
	//		nextballs.clear();

	//	}
	//}

	/*void AABBBallRectAngleBounce(UnStaticActor& ball, StaticActor& wall) {


	}*/

	//Quadtree::Quadtree(int pLevel, GameRect pBounds) {
	//	level = pLevel;
	//	bounds = pBounds;
	//	nodes.resize(4);
	//}


	//void Quadtree::clear() {
	//	objects.clear();
	//	for (int i = 0; i < nodes.size(); i++) {
	//		if (nodes[i]) {
	//			nodes[i]->clear();
	//			nodes[i] = nullptr;
	//		}
	//	}
	//}


	//void Quadtree::split() {

	//	float subWidth = bounds.sizex / 2.0f;
	//	float subHeight = bounds.sizey / 2.0f;
	//	float x = bounds.posx;
	//	float y = bounds.posy;

	//	nodes[0] = new Quadtree(level + 1, GameRect{ x + subWidth, y, subWidth, subHeight });
	//	nodes[1] = new Quadtree(level + 1, GameRect{ x, y, subWidth, subHeight });
	//	nodes[2] = new Quadtree(level + 1, GameRect{ x, y + subHeight, subWidth, subHeight });
	//	nodes[3] = new Quadtree(level + 1, GameRect{ x + subWidth, y + subHeight, subWidth, subHeight });
	//}


	//int Quadtree::getIndex(GameRect pRect) {

	//	int index = -1;
	//	float verticalMidpoint = bounds.posx + (bounds.sizex / 2.0f);
	//	float horizontalMidpoint = bounds.posy + (bounds.sizey / 2.0f);

	//	// Object can completely fit within the top quadrants
	//	bool topQuadrant = (pRect.posy < horizontalMidpoint&& pRect.posy + pRect.sizey < horizontalMidpoint);

	//	// Object can completely fit within the bottom quadrants
	//	bool bottomQuadrant = (pRect.posy > horizontalMidpoint);

	//	// Object can completely fit within the left quadrants
	//	if (pRect.posx < verticalMidpoint && pRect.posx + pRect.sizex < verticalMidpoint) {
	//		if (topQuadrant) {
	//			index = 1;
	//		}
	//		else if (bottomQuadrant) {
	//			index = 2;
	//		}
	//	}

	//	// Object can completely fit within the right quadrants
	//	else if (pRect.posx > verticalMidpoint) {
	//		if (topQuadrant) {
	//			index = 0;
	//		}
	//		else if (bottomQuadrant) {
	//			index = 3;
	//		}
	//	}
	//	return index;
	//}


	//void Quadtree::insert(Actor2<Vertex>* actor) {

	//	if (nodes[0]) {

	//		int index = getIndex(actor->gamerect);
	//		if (index != -1) {
	//			nodes[index]->insert(actor);
	//			return;
	//		}
	//	}

	//	objects.push_back(actor);

	//	if (objects.size() > maxobjs && level < maxlvls) {
	//		if (!nodes[0]) {
	//			split();
	//		}
	//		std::list<Actor2<Vertex>*> templist;

	//		while (objects.size()) {
	//			auto obj = objects.front();
	//			int index = getIndex(obj->gamerect);
	//			if (index != -1) {
	//				nodes[index]->insert(obj);
	//			}
	//			else {
	//				templist.push_back(obj);
	//			}
	//		}
	//		objects.merge(templist);
	//	}
	//}


	//std::list<Actor2<Vertex>*> Quadtree::retrieve(std::list<Actor2<Vertex>*> returnObjects, GameRect pRect) {

	//	int index = getIndex(pRect);
	//	if (index != -1 && nodes[0]) {
	//		nodes[index]->retrieve(returnObjects, pRect);
	//	}

	//	std::list<Actor2<Vertex>*> templist = objects;
	//	returnObjects.merge(templist);

	//	return returnObjects;

	//}


	void projectPolygon(const std::vector<glm::vec2>& polygon, const glm::vec2& axis, float& min, float& max)
	{
		min = glm::dot(polygon[0], axis);
		max = min;
		//0 is centre
		for (int i = 1; i < polygon.size(); i++)
		{
			float p = glm::dot(polygon[i], axis);
			if (p < min) min = p;
			else if (p > max) max = p;
		}
	}

	bool overlapOnAxis(const std::vector<glm::vec2>& polygon1, const std::vector<glm::vec2>& polygon2, const glm::vec2& axis) {
		float min1, max1, min2, max2;
		projectPolygon(polygon1, axis, min1, max1);
		projectPolygon(polygon2, axis, min2, max2);
		return (max1 >= min2 && max2 >= min1);
	}

	bool polygonCollision(const std::vector<glm::vec2>& polygon1, const std::vector<glm::vec2>& polygon2) {
		for (int i = 0; i < polygon1.size(); i++)
		{
			int j = (i + 1) % polygon1.size();
			glm::vec2 axis = glm::vec2(-(polygon1[j].y - polygon1[i].y), polygon1[j].x - polygon1[i].x);
			if (!overlapOnAxis(polygon1, polygon2, axis))
				return false;
		}
		for (int i = 0; i < polygon2.size(); i++)
		{
			int j = (i + 1) % polygon2.size();
			glm::vec2 axis = glm::vec2(-(polygon2[j].y - polygon2[i].y), polygon2[j].x - polygon2[i].x);
			if (!overlapOnAxis(polygon1, polygon2, axis))
				return false;
		}
		return true;
	}


	//...
	// polygon collision directional
	//// Structure that stores the results of the PolygonCollision function
	//struct PolygonCollisionResult {
	//	// Are the polygons going to intersect forward in time?
	//	bool WillIntersect;
	//	// Are the polygons currently intersecting?
	//	bool Intersect;
	//	// The translation to apply to the first polygon to push the polygons apart.
	//	glm::vec2 MinimumTranslationVector;
	//};

	float intervalDistance(float minA, float maxA, float minB, float maxB) {
		if (minA < minB) {
			return minB - maxA;
		}
		else {
			return minA - maxB;
		}
	}

	std::vector<glm::vec2> buildEdgesFromPoints(const std::vector<glm::vec2>& points) {
		std::vector<glm::vec2> ret;
		//vert 0 is centre
		for (int i = 1; i < points.size(); ++i) {
			if (i == points.size()-1) {
				ret.push_back(points[i] - points[1]);
			}
			else {
				ret.push_back(points[i] - points[i + 1]);
			}
		}

		return ret;
	}

	//fine for convex polygons, edgecount = vertex count
	PolygonCollisionResult PolygonCollision(const std::vector<glm::vec2>& polygon1,
											const std::vector<glm::vec2>& polygon2,
											glm::vec2 velocity) {

		PolygonCollisionResult result{};
		result.Intersect = true;
		result.WillIntersect = true;

		//TODO move this out, obs inefficient here
		std::vector<glm::vec2> edges1 = buildEdgesFromPoints(polygon1);
		std::vector<glm::vec2> edges2 = buildEdgesFromPoints(polygon2);
		int edgeCount1 = edges1.size();
		int edgeCount2 = edges2.size();
		float minIntervalDistance = FLT_MAX;
		glm::vec2 translationAxis;
		glm::vec2 edge;

		// Loop through all the edges of both polygons
		int edgemax = edgeCount1 + edgeCount2;
		for (int edgeIndex = 0; edgeIndex < (edgemax); edgeIndex++) {
			if (edgeIndex < edgeCount1) {
				edge = edges1[edgeIndex];
			}
			else {
				int p2idx = edgeIndex - edgeCount1;
				edge = edges2[p2idx];
			}

			// ===== 1. Find if the polygons are currently intersecting =====

			// Find the axis perpendicular to the current edge and normalise
			glm::vec2 axis = glm::vec2(-edge.y, edge.x);
			axis = glm::normalize(axis);

			// Find the projection of the polygon on the current axis
			float minA = 0.0f; float minB = 0.0f; float maxA = 0.0f; float maxB = 0.0f;
			projectPolygon(polygon1, axis, minA, maxA);
			projectPolygon(polygon2, axis, minB, maxB);

			// Check if the polygon projections are currentlty intersecting
			float intervaldist = intervalDistance(minA, maxA, minB, maxB);
			if (intervaldist > 0)
				result.Intersect = false;

			// ===== 2. Now find if the polygons *will* intersect =====

			// Project the velocity on the current axis
			float velocityProjection = glm::dot(axis, velocity); // axis.DotProduct(velocity);

			// Get the projection of polygon A during the movement
			if (velocityProjection < 0) {
				minA += velocityProjection;
			}
			else {
				maxA += velocityProjection;
			}

			// Do the same test as above for the new projection
			float willintervaldist = intervalDistance(minA, maxA, minB, maxB);
			if (willintervaldist > 0) result.WillIntersect = false;
			else intervaldist = willintervaldist;

			// If the polygons are not intersecting and won't intersect, exit the loop
			if (!result.Intersect && !result.WillIntersect) break;

			// Check if the current interval distance is the minimum one. If so store
			// the interval distance and the current distance.
			// This will be used to calculate the minimum translation vector
			intervaldist = std::abs(intervaldist);
			if (intervaldist < minIntervalDistance) {
				minIntervalDistance = intervaldist;
				translationAxis = axis;

				glm::vec2 d = polygon1[0] - polygon2[0]; // polygonA.Center - polygonB.Center;
				if (glm::dot(d,translationAxis) < 0.0f)
					translationAxis = -translationAxis;
			}
		}

		// The minimum translation vector
		// can be used to push the polygons appart.
		if (result.Intersect || result.WillIntersect) {
			result.MinimumTranslationVector = translationAxis * minIntervalDistance;
		}

		return result;
	}






}