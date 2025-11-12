#pragma once


#include "vulkantypes.h"
#include "Actors.h"
#include <list>
#include "kbctrler.h"
#include "controller.h"

//using Actors::StaticActor;
//using Actors::UnStaticActor;
using Actors::Actor3;

namespace physics {
    bool AABBCheckCollision(GameRect one, GameRect two);

    //will mutate actors' velocities if collision based on direction/overlaps
    //template <typename T*>
    /*void AABBCollisionBatBallDirection(Actor2<Vertex>& ball, Actor2<Vertex>& bat, KeyboardControls& kbctrler, ps4Controller& ctrler);
    glm::vec2 getAABBCollisionRectRectDirection(GameRect movingRect, GameRect staticRect, KeyboardControls& kbctrler, ps4Controller& ctrler);*/
    //void AABBCollisionWallBallDirection(UnStaticActor& ball, StaticActor& wall);

    //void collideAABBBalls(std::list<Actor2<Vertex>*>& balls);

    /*void checkQuadBoundingAABBCollision() {

    }

    void collidePolygonPolygon() {

    }*/


    //struct Quadtree {

    //    const int maxobjs = 10;
    //    const int maxlvls = 5;

    //    int level;
    //    std::list<Actor3<Vertex>*> objects{};
    //    GameRect bounds;
    //    std::vector<Quadtree*> nodes{};

    //    Quadtree(int pLevel, GameRect pBounds);

    //    //Clears the quadtree
    //    void clear();
    //    void split();

    //    //Determine which node the object belongs to. -1 means
    //    //object cannot completely fit within a child node and is part
    //    //of the parent node
    //    int getIndex(GameRect pRect);
    //
    //    //Insert the object into the quadtree. If the node
    //    //exceeds the capacity, it will split and add all
    //    //objects to their corresponding nodes.
    //    void insert(Actor3<Vertex>* actor);

    //    //Return all objects that could collide with the given object
    //    std::list<Actor3<Vertex>*> retrieve(std::list<Actor3<Vertex>*> returnObjects, GameRect pRect);


    //};

    //WIP
    /*float DotProduct(const std::vector<float>& v1, const std::vector<float>& v2)
    {
        return v1.x * v2.x + v1.y * v2.y;
    }*/
    //bool actor2GamerectCollision(Actor2<Vertex>& actor1, GameRect& grect);

    void projectPolygon(const std::vector<glm::vec2>& polygon, const glm::vec2& axis, float& min, float& max);
    bool overlapOnAxis(const std::vector<glm::vec2>& polygon1, const std::vector<glm::vec2>& polygon2, const glm::vec2& axis);
    bool polygonCollision(const std::vector<glm::vec2>& polygon1, const std::vector<glm::vec2>& polygon2);

    std::vector<glm::vec2> buildEdgesFromPoints(const std::vector<glm::vec2>& points);
    // Structure that stores the results of the PolygonCollision function
    struct PolygonCollisionResult {
        // Are the polygons going to intersect forward in time?
        bool WillIntersect;
        // Are the polygons currently intersecting?
        bool Intersect;
        // The translation to apply to the first polygon to push the polygons apart.
        glm::vec2 MinimumTranslationVector;
    };
    PolygonCollisionResult PolygonCollision(const std::vector<glm::vec2>& polygon1,
        const std::vector<glm::vec2>& polygon2,
        glm::vec2 velocity);



}