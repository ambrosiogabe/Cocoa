#pragma once

#include "TestFactory.h"
#include "jade/renderer/Line2D.h"
#include "jade/physics2d/primitives/AABB.h"
#include "jade/physics2d/primitives/Circle.h"
#include "jade/physics2d/primitives/Box2D.h"
#include "jade/physics2d/primitives/Raycast2D.h"
#include "jade/physics2d/rigidbody/CollisionDetector2D.h"

#include <glm/vec2.hpp>

namespace Jade
{
	namespace CollisionDetectorTester2D
	{
        static const float EPSILON = 0.000001f;

        // =========================================================================================================
        // Line IntersectionTester tests
        // =========================================================================================================
        JADE_TEST(pointOnLineShouldReturnTrueTestOne)
        {
            Line2D line{ glm::vec2{0, 0}, glm::vec2{12, 4} };
            glm::vec2 point { 0, 0 };

            bool res = CollisionDetector2D::PointOnLine(point, line);
            Log::Assert(res, "Point should be on line.");
            return res;
        }

        
        JADE_TEST(pointOnLineShouldReturnTrueTestTwo)
        {
            Line2D line {glm::vec2 {0, 0}, glm::vec2 {12, 4}};
            glm::vec2 point {6, 2};

            bool res = CollisionDetector2D::PointOnLine(point, line);
            Log::Assert(res, "");
            return res;
        }

        
        JADE_TEST(pointOnLineShouldReturnFalseTestOne)
        {
            Line2D line {glm::vec2 {0, 0}, glm::vec2 {12, 4}};
            glm::vec2 point {4, 2};

            bool res = CollisionDetector2D::PointOnLine(point, line);
            Log::Assert(!res, "");
            return !res;
        }

        
        JADE_TEST(pointOnLineShouldReturnTrueTestThree)
        {
            Line2D line {glm::vec2 {10, 10}, glm::vec2 {22, 14}};
            glm::vec2 point {10, 10};

            bool res = CollisionDetector2D::PointOnLine(point, line);
            Log::Assert(res, "");
            return res;
        }

        
        JADE_TEST(pointOnLineShouldReturnTrueTestFour)
        {
            Line2D line {glm::vec2 {10, 10}, glm::vec2 {22, 14}};
            glm::vec2 point {16, 12};

            bool res = CollisionDetector2D::PointOnLine(point, line);
            Log::Assert(res, "");
            return res;
        }

        
        JADE_TEST(pointOnLineShouldReturnFalseTestTwo)
        {
            Line2D line {glm::vec2 {10, 10}, glm::vec2 {22, 14}};
            glm::vec2 point {14, 12};

            bool res = CollisionDetector2D::PointOnLine(point, line);
            Log::Assert(!res, "");
            return !res;
        }

        
        //    JADE_TEST(closestPointToLineTestOne)
        //{
        //    Line2D line{ glm::vec2 {0, 0}, glm::vec2 {12, 4} };
        //    glm::vec2 point {6, 2};

        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, line);
        //    glm::vec2 actualClosestPoint {6, 2};

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosestPoint, actualClosestPoint), "");
        //}

        //
        //    JADE_TEST(closestPointToLineTestTwo)
        //{
        //    Line2D line {glm::vec2 {0, 0}, glm::vec2 {12, 4}};
        //    glm::vec2 point {13, 3};

        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, line);
        //    glm::vec2 actualClosestPoint {12, 4};

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosestPoint, actualClosestPoint), "");
        //}

        //
        //    JADE_TEST(closestPointToLineTestThree)
        //{
        //    Line2D line {glm::vec2 {0, 0}, glm::vec2 {12, 4}};
        //    glm::vec2 point {7, 4};

        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, line);
        //    glm::vec2 actualClosestPoint = glm::vec2 { 7.5f, 2.5f };

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosestPoint, actualClosestPoint), "");
        //}

        //
        //    JADE_TEST(closestPointToLineTestFour)
        //{
        //    Line2D line {glm::vec2 {10, 10}, glm::vec2 {22, 14}};
        //    glm::vec2 point {16, 12};

        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, line);
        //    glm::vec2 actualClosestPoint {16, 12};

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosestPoint, actualClosestPoint), "");
        //}

        //
        //    JADE_TEST(closestPointToLineTestFive)
        //{
        //    Line2D line {glm::vec2 {10, 10}, glm::vec2 {22, 14}};
        //    glm::vec2 point {23, 13};

        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, line);
        //    glm::vec2 actualClosestPoint {22, 14};

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosestPoint, actualClosestPoint), "");
        //}

        //
        //    JADE_TEST(closestPointToLineTestSix)
        //{
        //    Line2D line {glm::vec2 {10, 10}, glm::vec2 {22, 14}};
        //    glm::vec2 point {17, 14};

        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, line);
        //    glm::vec2 actualClosestPoint = glm::vec2 { 17.5f, 12.5f };

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosestPoint, actualClosestPoint), "");
        //}




        // =========================================================================================================
        // Raycast IntersectionTester tests
        // =========================================================================================================
        
        //JADE_TEST(pointOnRayShouldReturnTrueTestOne)
        //{
        //    Raycast2D ray{ glm::vec2 {0}, glm::vec2 { 0.948683f, 0.316228f } };
        //    glm::vec2 point {0, 0};

        //    bool res = CollisionDetector2D::PointOnRay(point, ray);
        //    Log::Assert(res, "Point should be on ray");
        //    return res;
        //}

        //
        //JADE_TEST(pointOnRayShouldReturnTrueTestTwo)
        //{
        //    Raycast2D ray{ glm::vec2 {0}, glm::vec2 { 0.948683f, 0.316228f } };
        //    glm::vec2 point {6, 2};

        //    bool res = CollisionDetector2D::PointOnRay(point, ray);
        //    Log::Assert(res, "Point should be on ray");
        //    return res;
        //}

        //
        //JADE_TEST(pointOnRayShouldReturnFalseTestOne)
        //{
        //    Raycast2D ray{ glm::vec2 {0}, glm::vec2 { 0.948683f, 0.316228f } };
        //    glm::vec2 point { -6, -2 };

        //    bool res = CollisionDetector2D::PointOnRay(point, ray);
        //    Log::Assert(res, "Point should be on ray");
        //    return res;
        //}

        //
        //JADE_TEST(pointOnRayShouldReturnFalseTestTwo)
        //{
        //    Raycast2D ray {glm::vec2 {0}, glm::vec2 { 0.948683f, 0.316228f }};
        //    glm::vec2 point {4, 2};

        //    bool res = CollisionDetector2D::PointOnRay(point, ray);
        //    Log::Assert(res, "Point should be on ray");
        //    return res;
        //}

        //
        //JADE_TEST(pointOnRayShouldReturnTrueTestThree)
        //{
        //    Raycast2D ray {glm::vec2 {10, 10}, glm::vec2 { 0.948683f, 0.316228f }};
        //    glm::vec2 point {10, 10};

        //    bool res = CollisionDetector2D::PointOnRay(point, ray);
        //    Log::Assert(res, "Point should be on ray");
        //    return res;
        //}

        //
        //JADE_TEST(pointOnRayShouldReturnTrueTestFour)
        //{
        //    Raycast2D ray {glm::vec2 {10, 10}, glm::vec2 { 0.948683f, 0.316228f }};
        //    glm::vec2 point {16, 12};

        //    bool res = CollisionDetector2D::PointOnRay(point, ray);
        //    Log::Assert(res, "Point should be on ray");
        //    return res;
        //}

        //
        //JADE_TEST(pointOnRayShouldReturnFalseTestThree)
        //{
        //    Raycast2D ray {glm::vec2 {10, 10}, glm::vec2 { 0.948683f, 0.316228f }};
        //    glm::vec2 point{ -6 + 10, -2 + 10 };

        //    bool res = CollisionDetector2D::PointOnRay(point, ray);
        //    Log::Assert(res, "Point should be on ray");
        //    return res;
        //}

        //
        //JADE_TEST(pointOnRayShouldReturnFalseTestFour)
        //{
        //    Raycast2D ray {glm::vec2 {10, 10}, glm::vec2 { 0.948683f, 0.316228f }};
        //    glm::vec2 point {14, 12};

        //    bool res = CollisionDetector2D::PointOnRay(point, ray);
        //    Log::Assert(res, "Point should be on ray");
        //    return res;
        //}

        
        //JADE_TEST(closestPointToRayTestOne)
        //{
        //    Raycast2D ray {glm::vec2 {0, 0}, glm::vec2 { 0.948683f, 0.316228f }};
        //    glm::vec2 point  { -1, -1 };

        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, ray);
        //    glm::vec2 actualClosestPoint {0, 0};

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosestPoint, actualClosestPoint), "");
        //}

        //
        //JADE_TEST(closestPointToRayTestTwo)
        //{
        //    Raycast2D ray {glm::vec2 {0, 0}, new Vector3f((float)(3.0 / Math.sqrt(10f)), (float)(1.0 / Math.sqrt(10f)), 0));
        //    glm::vec2 point {6, 2};

        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, ray);
        //    glm::vec2 actualClosestPoint {6, 2};

        //    assertTrue(JMath.compare(calculatedClosestPoint, actualClosestPoint, EPSILON));
        //}

        //
        //JADE_TEST(closestPointToRayTestThree)
        //{
        //    Raycast2D ray {glm::vec2 {0, 0}, new Vector3f((float)(3.0 / Math.sqrt(10f)), (float)(1.0 / Math.sqrt(10f)), 0));
        //    glm::vec2 point {7, 4};

        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, ray);
        //    glm::vec2 actualClosestPoint = glm::vec2 { 7.5f, 2.5f };

        //    assertTrue(JMath.compare(calculatedClosestPoint, actualClosestPoint, EPSILON));
        //}

        //
        //JADE_TEST(closestPointToRayTestFour)
        //{
        //    Raycast2D ray {glm::vec2 {10, 10}, glm::vec2 { 0.948683f, 0.316228f }};
        //    glm::vec2 point {9, 9};

        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, ray);
        //    glm::vec2 actualClosestPoint {10, 10};

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosestPoint, actualClosestPoint), "");
        //}

        //
        //JADE_TEST(closestPointToRayTestFive)
        //{
        //    Raycast2D ray {glm::vec2 {10, 10}, new Vector3f((float)(3.0 / Math.sqrt(10f)), (float)(1.0 / Math.sqrt(10f)), 0));
        //    glm::vec2 point {16, 12};

        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, ray);
        //    glm::vec2 actualClosestPoint {16, 12};

        //    assertTrue(JMath.compare(calculatedClosestPoint, actualClosestPoint, EPSILON));
        //}

        //
        //JADE_TEST(closestPointToRayTestSix)
        //{
        //    Raycast2D ray {glm::vec2 {10, 10}, new Vector3f((float)(3.0 / Math.sqrt(10f)), (float)(1.0 / Math.sqrt(10f)), 0));
        //    glm::vec2 point {17, 14};

        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, ray);
        //    glm::vec2 actualClosestPoint = glm::vec2 { 17.5f, 12.5f };

        //    assertTrue(JMath.compare(calculatedClosestPoint, actualClosestPoint, EPSILON));
        //}



        // =========================================================================================================
        // Plane IntersectionTester tests
        // =========================================================================================================
        
        //    JADE_TEST(pointOnPlaneShouldReturnTrueTestOne)
        //{
        //    Plane plane = new Plane(glm::vec2 {0, 1}, 0);
        //    glm::vec2 point  { -100, 0 };

        //    Log::Assert(CollisionDetector2D::pointOnPlane(point, plane), "");
        //}

        //
        //    JADE_TEST(pointOnPlaneShouldReturnTrueTestTwo)
        //{
        //    Plane plane = new Plane(glm::vec2 {0, 1}, 0);
        //    glm::vec2 point  { 100, 0 };

        //    Log::Assert(CollisionDetector2D::pointOnPlane(point, plane), "");
        //}

        //
        //    JADE_TEST(pointOnPlaneShouldReturnFalseTestOne)
        //{
        //    Plane plane = new Plane(glm::vec2 {0, 1}, 0);
        //    glm::vec2 point {0, 10};

        //    Log::Assert(CollisionDetector2D::pointOnPlane(point, plane), "");
        //}

        //
        //    JADE_TEST(pointOnPlaneShouldReturnTrueTestThree)
        //{
        //    Plane plane = new Plane(glm::vec2 {0, 1}, 10);
        //    glm::vec2 point  { -110, 10 };

        //    Log::Assert(CollisionDetector2D::pointOnPlane(point, plane), "");
        //}

        //
        //    JADE_TEST(pointOnPlaneShouldReturnTrueTestFour)
        //{
        //    Plane plane = new Plane(glm::vec2 {0, 1}, 10);
        //    glm::vec2 point  { 110, 10 };

        //    Log::Assert(CollisionDetector2D::pointOnPlane(point, plane), "");
        //}

        //
        //    JADE_TEST(pointOnPlaneShouldReturnFalseTestTwo)
        //{
        //    Plane plane = new Plane(glm::vec2 {0, 1}, 10);
        //    glm::vec2 point {10, 0};

        //    Log::Assert(CollisionDetector2D::pointOnPlane(point, plane), "");
        //}

        //
        //    JADE_TEST(pointOnPlaneShouldReturnTrueTestSix)
        //{
        //    Plane plane = new Plane(glm::vec2 {3, 4}, 0);
        //    glm::vec2 point  { -4, 3 };

        //    Log::Assert(CollisionDetector2D::pointOnPlane(point, plane), "");
        //}

        //
        //    JADE_TEST(pointOnPlaneShouldReturnTrueTestSeven)
        //{
        //    Plane plane = new Plane(glm::vec2 {3, 4}, 0);
        //    glm::vec2 point  { 4, -3 };

        //    Log::Assert(CollisionDetector2D::pointOnPlane(point, plane), "");
        //}

        //
        //    JADE_TEST(pointOnPlaneShouldReturnFalseTestThree)
        //{
        //    Plane plane = new Plane(glm::vec2 {3, 4}, 0);
        //    glm::vec2 point = new Vector3f(-3, 4.1f, 0);

        //    Log::Assert(CollisionDetector2D::pointOnPlane(point, plane), "");
        //}

        //
        //    JADE_TEST(closestPointToPlaneTestOne)
        //{
        //    Plane plane = new Plane(glm::vec2 {0, 1}, 0);
        //    glm::vec2 point {0, 10};

        //    glm::vec2 calculatedClosest = IntersectionTester.closestPoint(point, plane);
        //    glm::vec2 actualClosest {0, 0};

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosest, actualClosest), "");
        //}

        //
        //    JADE_TEST(closestPointToPlaneTestTwo)
        //{
        //    Plane plane = new Plane(glm::vec2 {0, 1}, 0);
        //    glm::vec2 point  { -10, -10 };

        //    glm::vec2 calculatedClosest = IntersectionTester.closestPoint(point, plane);
        //    glm::vec2 actualClosest  { -10, 0 };

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosest, actualClosest), "");
        //}

        //
        //    JADE_TEST(closestPointToPlaneTestThree)
        //{
        //    Plane plane = new Plane(glm::vec2 {0, 1}, 0);
        //    glm::vec2 point {0, 0};

        //    glm::vec2 calculatedClosest = IntersectionTester.closestPoint(point, plane);
        //    glm::vec2 actualClosest {0, 0};

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosest, actualClosest), "");
        //}

        //
        //    JADE_TEST(closestPointToPlaneTestFour)
        //{
        //    Plane plane = new Plane(glm::vec2 {3, 4}, 0);
        //    glm::vec2 point  { -1, 7 };

        //    glm::vec2 calculatedClosest = IntersectionTester.closestPoint(point, plane);
        //    glm::vec2 actualClosest  { -4, 3 };

        //    Log::Assert(CollisionDetector2D::compare(actualClosest, calculatedClosest), "");
        //}

        //
        //    JADE_TEST(closestPointToPlaneTestFive)
        //{
        //    Plane plane = new Plane(glm::vec2 {3, 4}, 0);
        //    glm::vec2 point {7, 1};

        //    glm::vec2 calculatedClosest = IntersectionTester.closestPoint(point, plane);
        //    glm::vec2 actualClosest  { 4, -3 };

        //    assertTrue(JMath.compare(actualClosest, calculatedClosest, EPSILON));
        //}

        //
        //    JADE_TEST(closestPointToPlaneTestSix)
        //{
        //    Plane plane = new Plane(glm::vec2 {3, 4}, 0);
        //    glm::vec2 point {3, 4};

        //    glm::vec2 calculatedClosest = IntersectionTester.closestPoint(point, plane);
        //    glm::vec2 actualClosest {0, 0};

        //    Log::Assert(CollisionDetector2D::compare(actualClosest, calculatedClosest), "");
        //}


        //// =========================================================================================================
        //// Sphere intersection tester tests
        //// =========================================================================================================
        //
        //    JADE_TEST(pointInSphereShouldReturnTrueTestOne)
        //{
        //    Sphere sphere = new Sphere(5f);
        //    GameObject gameObject = new GameObject("Sphere Test", new Transform(new Vector3f()));
        //    gameObject.addComponent(sphere);

        //    glm::vec2 point  { 3, -2 };

        //    boolean result = IntersectionTester.pointInSphere(point, sphere);
        //    assertTrue(result);
        //}

        //
        //    JADE_TEST(pointInSphereShouldReturnTrueTestTwo)
        //{
        //    Sphere sphere = new Sphere(5f);
        //    GameObject gameObject = new GameObject("Sphere Test", new Transform(new Vector3f()));
        //    gameObject.addComponent(sphere);

        //    glm::vec2 point = new Vector3f(-4.9f, 0, 0);

        //    boolean result = IntersectionTester.pointInSphere(point, sphere);
        //    assertTrue(result);
        //}

        //
        //    JADE_TEST(pointInSphereShouldReturnFalseTestOne)
        //{
        //    Sphere sphere = new Sphere(5f);
        //    GameObject gameObject = new GameObject("Sphere Test", new Transform(new Vector3f()));
        //    gameObject.addComponent(sphere);

        //    glm::vec2 point  { -6, -6 };

        //    boolean result = IntersectionTester.pointInSphere(point, sphere);
        //    assertFalse(result);
        //}

        //
        //    JADE_TEST(pointInSphereShouldReturnTrueTestFour)
        //{
        //    Sphere sphere = new Sphere(5f);
        //    GameObject gameObject = new GameObject("Sphere Test", new Transform(glm::vec2 {10}});
        //    gameObject.addComponent(sphere);

        //    glm::vec2 point = new Vector3f(3 + 10, -2 + 10, -2 + 10);

        //    boolean result = IntersectionTester.pointInSphere(point, sphere);
        //    assertTrue(result);
        //}

        //
        //    JADE_TEST(pointInSphereShouldReturnTrueTestFive)
        //{
        //    Sphere sphere = new Sphere(5f);
        //    GameObject gameObject = new GameObject("Sphere Test", new Transform(glm::vec2 {10}});
        //    gameObject.addComponent(sphere);

        //    glm::vec2 point = new Vector3f(-4.9f + 10, 0 + 10, 0 + 10);

        //    boolean result = IntersectionTester.pointInSphere(point, sphere);
        //    assertTrue(result);
        //}

        //
        //    JADE_TEST(pointInSphereShouldReturnFalseTestTwo)
        //{
        //    Sphere sphere = new Sphere(5f);
        //    GameObject gameObject = new GameObject("Sphere Test", new Transform(glm::vec2 {10}});
        //    gameObject.addComponent(sphere);

        //    glm::vec2 point = new Vector3f(-6 + 10, -6 + 10, 1 + 10);

        //    boolean result = IntersectionTester.pointInSphere(point, sphere);
        //    assertFalse(result);
        //}

        //
        //    JADE_TEST(closestPointToSphereTestOne)
        //{
        //    Sphere sphere = new Sphere(1f);
        //    GameObject gameObject = new GameObject("Sphere Test", new Transform(glm::vec2 {0}});
        //    gameObject.addComponent(sphere);

        //    glm::vec2 point {5, 0};

        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, sphere);
        //    glm::vec2 actualClosestPoint {1, 0};

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosestPoint, actualClosestPoint), "");
        //}

        //
        //    JADE_TEST(closestPointToSphereTestTwo)
        //{
        //    Sphere sphere = new Sphere(1f);
        //    GameObject gameObject = new GameObject("Sphere Test", new Transform(glm::vec2 {0}});
        //    gameObject.addComponent(sphere);

        //    glm::vec2 point = new Vector3f(2.5f, -2.5f, 2.5f);

        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, sphere);
        //    glm::vec2 actualClosestPoint = new Vector3f(0.5773502f, -0.5773502f, 0.5773502f);

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosestPoint, actualClosestPoint), "");
        //}

        //
        //    JADE_TEST(closestPointToSphereTestThree)
        //{
        //    Sphere sphere = new Sphere(1f);
        //    GameObject gameObject = new GameObject("Sphere Test", new Transform(glm::vec2 {10}});
        //    gameObject.addComponent(sphere);

        //    glm::vec2 point = new Vector3f(5 + 10, 0 + 10, 0 + 10);

        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, sphere);
        //    glm::vec2 actualClosestPoint = new Vector3f(1 + 10, 0 + 10, 0 + 10);

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosestPoint, actualClosestPoint), "");
        //}

        //
        //    JADE_TEST(closestPointToSphereTestFour)
        //{
        //    Sphere sphere = new Sphere(1f);
        //    GameObject gameObject = new GameObject("Sphere Test", new Transform(glm::vec2 {10}});
        //    gameObject.addComponent(sphere);

        //    glm::vec2 point = new Vector3f(2.5f + 10, -2.5f + 10, 2.5f + 10);

        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, sphere);
        //    glm::vec2 actualClosestPoint = new Vector3f(0.5773502f + 10, -0.5773502f + 10, 0.5773502f + 10);

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosestPoint, actualClosestPoint), "");
        //}



        //// =========================================================================================================
        //// Box intersection tester tests
        //// =========================================================================================================
        //
        //    JADE_TEST(pointInBoxShouldReturnTrueTestOne)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {0}});
        //    gameObject.addComponent(box);

        //    glm::vec2 point = glm::vec2 { 4, 0 };

        //    Log::Assert(CollisionDetector2D::pointInBox(point, box), "");
        //}

        //
        //    JADE_TEST(pointInBoxShouldReturnTrueTestTwo)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {0}});
        //    gameObject.addComponent(box);

        //    glm::vec2 point = new Vector3f(-4.9f, -4.9f, -4.9f);

        //    Log::Assert(CollisionDetector2D::pointInBox(point, box), "");
        //}

        //
        //    JADE_TEST(pointInBoxShouldReturnFalseTestOne)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {0}});
        //    gameObject.addComponent(box);

        //    glm::vec2 point = glm::vec2 { 0, 5.1f };

        //    Log::Assert(CollisionDetector2D::pointInBox(point, box), "");
        //}

        //
        //    JADE_TEST(pointInBoxShouldReturnTrueTestThree)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {10}});
        //    gameObject.addComponent(box);

        //    glm::vec2 point = new Vector3f(4 + 10, 0 + 10, 4.3f + 10);

        //    Log::Assert(CollisionDetector2D::pointInBox(point, box), "");
        //}

        //
        //    JADE_TEST(pointInBoxShouldReturnTrueTestFour)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {10}});
        //    gameObject.addComponent(box);

        //    glm::vec2 point = new Vector3f(-4.9f + 10, -4.9f + 10, -4.9f + 10);

        //    Log::Assert(CollisionDetector2D::pointInBox(point, box), "");
        //}

        //
        //    JADE_TEST(pointInBoxShouldReturnFalseTestTwo)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {10}});
        //    gameObject.addComponent(box);

        //    glm::vec2 point = new Vector3f(0 + 10, 5.1f + 10, 0 + 10);

        //    Log::Assert(CollisionDetector2D::pointInBox(point, box), "");
        //}

        //
        //    JADE_TEST(pointInRotatedBoxShouldReturnTrueTestOne)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {0}, glm::vec2 {1}, glm::vec2 {45, 45}}};
        //    gameObject.addComponent(box);

        //    glm::vec2 point  { -1, -1 };

        //    Log::Assert(CollisionDetector2D::pointInBox(point, box), "");
        //}

        //
        //    JADE_TEST(pointInRotatedShouldReturnTrueTestTwo)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {0}, glm::vec2 {1}, glm::vec2 {0, 0}}};
        //    gameObject.addComponent(box);

        //    glm::vec2 point = new Vector3f(-3.43553390593f, 3.43553390593f, 0);

        //    Log::Assert(CollisionDetector2D::pointInBox(point, box), "");
        //}

        //
        //    JADE_TEST(pointInRotatedShouldReturnFalseTestOne)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {0}, glm::vec2 {1}, glm::vec2 {0, 0}}};
        //    gameObject.addComponent(box);

        //    glm::vec2 point = new Vector3f(-3.63553390593f, 3.63553390593f, 0);

        //    Log::Assert(CollisionDetector2D::pointInBox(point, box), "");
        //}

        //
        //    JADE_TEST(pointInRotatedBoxShouldReturnTrueTestThree)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {10}, glm::vec2 {1}, glm::vec2 {45, 45}}};
        //    gameObject.addComponent(box);

        //    glm::vec2 point = new Vector3f(-1 + 10, -1 + 10, -1 + 10);

        //    Log::Assert(CollisionDetector2D::pointInBox(point, box), "");
        //}

        //
        //    JADE_TEST(pointInRotatedShouldReturnTrueTestFour)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {10}, glm::vec2 {1}, glm::vec2 {0, 0}}};
        //    gameObject.addComponent(box);

        //    glm::vec2 point = new Vector3f(-3.43553390593f + 10, 3.43553390593f + 10, 0 + 10);

        //    Log::Assert(CollisionDetector2D::pointInBox(point, box), "");
        //}

        //
        //    JADE_TEST(pointInRotatedShouldReturnFalseTestTwo)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {10}, glm::vec2 {1}, glm::vec2 {0, 0}}};
        //    gameObject.addComponent(box);

        //    glm::vec2 point = new Vector3f(-3.63553390593f + 10, 3.63553390593f + 10, 0 + 10);

        //    Log::Assert(CollisionDetector2D::pointInBox(point, box), "");
        //}


        //
        //    JADE_TEST(closestPointToBoxTestOne)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {0}});
        //    gameObject.addComponent(box);

        //    glm::vec2 point {0, 10};
        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, box);
        //    glm::vec2 actualClosestPoint {0, 5};

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosestPoint, actualClosestPoint), "");
        //}

        //
        //    JADE_TEST(closestPointToBoxTestTwo)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {0}});
        //    gameObject.addComponent(box);

        //    glm::vec2 point  { -6, -4 };
        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, box);
        //    glm::vec2 actualClosestPoint  { -5, -4 };

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosestPoint, actualClosestPoint), "");
        //}

        //
        //    JADE_TEST(closestPointToBoxTestThree)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {0}});
        //    gameObject.addComponent(box);

        //    glm::vec2 point  { 3, -4 };
        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, box);
        //    glm::vec2 actualClosestPoint  { 3, -4 };

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosestPoint, actualClosestPoint), "");
        //}

        //
        //    JADE_TEST(closestPointToBoxTestFour)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {10}});
        //    gameObject.addComponent(box);

        //    glm::vec2 point = new Vector3f(0 + 10, 10 + 10, 0 + 10);
        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, box);
        //    glm::vec2 actualClosestPoint = new Vector3f(0 + 10, 5 + 10, 0 + 10);

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosestPoint, actualClosestPoint), "");
        //}

        //
        //    JADE_TEST(closestPointToBoxTestFive)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {10}});
        //    gameObject.addComponent(box);

        //    glm::vec2 point = new Vector3f(-6 + 10, -4 + 10, 0 + 10);
        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, box);
        //    glm::vec2 actualClosestPoint = new Vector3f(-5 + 10, -4 + 10, 0 + 10);

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosestPoint, actualClosestPoint), "");
        //}

        //
        //    JADE_TEST(closestPointToBoxTestSix)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {10}});
        //    gameObject.addComponent(box);

        //    glm::vec2 point = new Vector3f(3 + 10, -4 + 10, 0 + 10);
        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, box);
        //    glm::vec2 actualClosestPoint = new Vector3f(3 + 10, -4 + 10, 0 + 10);

        //    Log::Assert(CollisionDetector2D::compare(calculatedClosestPoint, actualClosestPoint), "");
        //}

        //
        //    JADE_TEST(closestPointToRotatedBoxTestOne)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {0}, glm::vec2 {1}, glm::vec2 {0, 0}}};
        //    gameObject.addComponent(box);

        //    glm::vec2 point {10, 0};
        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, box);
        //    glm::vec2 actualClosestPoint = glm::vec2 { 7.07106781187f, 0 };

        //    assertTrue(JMath.compare(calculatedClosestPoint, actualClosestPoint, EPSILON));
        //}

        //
        //    JADE_TEST(closestPointToRotatedBoxTestTwo)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {0}, glm::vec2 {1}, glm::vec2 {0, 0}}};
        //    gameObject.addComponent(box);

        //    glm::vec2 point = new Vector3f(-5.5355339f, -5.5355339f, 0);
        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, box);
        //    glm::vec2 actualClosestPoint = new Vector3f(-3.5355339f, -3.5355339f, 0);

        //    assertTrue(JMath.compare(calculatedClosestPoint, actualClosestPoint, EPSILON));
        //}

        //
        //    JADE_TEST(closestPointToRotatedBoxTestThree)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {0}, glm::vec2 {1}, glm::vec2 {0, 0}}};
        //    gameObject.addComponent(box);

        //    glm::vec2 point = glm::vec2 { 0, 7.07106781187f };
        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, box);
        //    glm::vec2 actualClosestPoint = glm::vec2 { 0, 7.07106781187f };

        //    assertTrue(JMath.compare(calculatedClosestPoint, actualClosestPoint, EPSILON));
        //}

        //
        //    JADE_TEST(closestPointToRotatedBoxTestFour)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {10}, glm::vec2 {1}, glm::vec2 {0, 0}}};
        //    gameObject.addComponent(box);

        //    glm::vec2 point = new Vector3f(10 + 10, 0 + 10, 0 + 10);
        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, box);
        //    glm::vec2 actualClosestPoint = new Vector3f(7.07106781187f + 10, 0 + 10, 0 + 10);

        //    assertTrue(JMath.compare(calculatedClosestPoint, actualClosestPoint, EPSILON));
        //}

        //
        //    JADE_TEST(closestPointToRotatedBoxTestFive)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {10}, glm::vec2 {1}, glm::vec2 {0, 0}}};
        //    gameObject.addComponent(box);

        //    glm::vec2 point = new Vector3f(-5.5355339f + 10, -5.5355339f + 10, 0 + 10);
        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, box);
        //    glm::vec2 actualClosestPoint = new Vector3f(-3.5355339f + 10, -3.5355339f + 10, 0 + 10);

        //    assertTrue(JMath.compare(calculatedClosestPoint, actualClosestPoint, EPSILON));
        //}

        //
        //    JADE_TEST(closestPointToRotatedBoxTestSix)
        //{
        //    Box box = new Box(glm::vec2 {10}};
        //    GameObject gameObject = new GameObject("Box Test", new Transform(glm::vec2 {10}, glm::vec2 {1}, glm::vec2 {0, 0}}};
        //    gameObject.addComponent(box);

        //    glm::vec2 point = new Vector3f(0 + 10, 7.07106781187f + 10, 0 + 10);
        //    glm::vec2 calculatedClosestPoint = IntersectionTester.closestPoint(point, box);
        //    glm::vec2 actualClosestPoint = new Vector3f(0 + 10, 7.07106781187f + 10, 0 + 10);

        //    assertTrue(JMath.compare(calculatedClosestPoint, actualClosestPoint, EPSILON));
        //}
	}
}