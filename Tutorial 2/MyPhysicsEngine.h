#pragma once

#include "BasicActors.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	using namespace std;

	//a list of colours: Circus Palette
	static const PxVec3 color_palette[] = {PxVec3(46.f/255.f,9.f/255.f,39.f/255.f),PxVec3(217.f/255.f,0.f/255.f,0.f/255.f),
		PxVec3(255.f/255.f,45.f/255.f,0.f/255.f),PxVec3(255.f/255.f,140.f/255.f,54.f/255.f),PxVec3(4.f/255.f,117.f/255.f,111.f/255.f)};

	//pyramid vertices
	static PxVec3 pyramid_verts[] = {PxVec3(0,1,0), PxVec3(1,0,0), PxVec3(-1,0,0), PxVec3(0,0,1), PxVec3(0,0,-1)};
	//pyramid triangles: a list of three vertices for each triangle e.g. the first triangle consists of vertices 1, 4 and 0
	//vertices have to be specified in a counter-clockwise order to assure the correct shading in rendering
	static PxU32 pyramid_trigs[] = {1, 4, 0, 3, 1, 0, 2, 3, 0, 4, 2, 0, 3, 2, 1, 2, 4, 1};

	class Pyramid : public ConvexMesh
	{
	public:
		Pyramid(PxTransform pose=PxTransform(PxIdentity), PxReal density=1.f) :
			ConvexMesh(vector<PxVec3>(begin(pyramid_verts),end(pyramid_verts)), pose, density)
		{
		}
	};

	class PyramidStatic : public TriangleMesh
	{
	public:
		PyramidStatic(PxTransform pose=PxTransform(PxIdentity)) :
			TriangleMesh(vector<PxVec3>(begin(pyramid_verts),end(pyramid_verts)), vector<PxU32>(begin(pyramid_trigs),end(pyramid_trigs)), pose)
		{
		}
	};


	class MySimulationEventCallback : public PxSimulationEventCallback
	{
	public:
		//an example variable that will be checked in the main simulation loop
		bool trigger;

		MySimulationEventCallback() : trigger(false) {}

		///Method called when the contact with the trigger object is detected.
		virtual void onTrigger(PxTriggerPair* pairs, PxU32 count)
		{
			//you can read the trigger information here
			for (PxU32 i = 0; i < count; i++)
			{
				//filter out contact with the planes
				if (pairs[i].otherShape->getGeometryType() != PxGeometryType::ePLANE)
				{
					//check if eNOTIFY_TOUCH_FOUND trigger
					if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
					{
						cerr << "onTrigger::eNOTIFY_TOUCH_FOUND" << endl;
						trigger = true;
					}
					//check if eNOTIFY_TOUCH_LOST trigger
					if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST)
					{
						cerr << "onTrigger::eNOTIFY_TOUCH_LOST" << endl;
						trigger = false;
					}
				}
			}
		}

		virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
		{
			// do nothing
		}

		virtual void onWake(PxActor** actors, PxU32 count)
		{
			// do nothing
		}

		virtual void onSleep(PxActor** actors, PxU32 count)
		{
			// do nothing
		}

		virtual void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {
			//xx
		}

		///Method called when the contact by the filter shader is detected.
		virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
		{
			cerr << "Contact found between " << pairHeader.actors[0]->getName() << " " << pairHeader.actors[1]->getName() << endl;

			//check all pairs
			for (PxU32 i = 0; i < nbPairs; i++)
			{
				//check eNOTIFY_TOUCH_FOUND
				if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
				{
					cerr << "onContact::eNOTIFY_TOUCH_FOUND" << endl;
				}
				//check eNOTIFY_TOUCH_LOST
				if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					cerr << "onContact::eNOTIFY_TOUCH_LOST" << endl;
				}
			}
		}
	};



	///Custom scene class
	class MyScene : public Scene
	{

		//actors
		Plane* plane;
		Box* box;
		RugbyGoalPost* gPost;
		RugbyBall* ball;
		FieldLines* fieldlines;
		OuterLines* outerlines;
		Castle* castleTB;
		SeesawBase* ssBase;
		Seesaw* ss;
		Sphere* sphere;
		Cloth* cloth;
		Pyramid* pyramid;

		//triggers
		triggerBox* tBox;
		bool boxSpawned;
		MySimulationEventCallback* callback;


		//materials
		PxMaterial* ballMat = CreateMaterial(0.7f, 0.5f, 0.7f);
		PxMaterial* grassMat = CreateMaterial(0.9f, 0.5f, 0.3f);


	public:
		///A custom scene class
		void SetVisualisation()
		{
			px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
		}

		//Custom scene initialisation
		virtual void CustomInit() 
		{
			SetVisualisation();			

			GetMaterial()->setDynamicFriction(.2f);

			plane = new Plane();
			plane->Color(PxVec3(0,0.3f,0));
			Add(plane);

			/*box = new Box();
			box->Color(color_palette[0]);
			box->Get()->is<PxRigidDynamic>()->setGlobalPose(PxTransform(PxVec3(0, 85, -25.0f)));
			box->Get()->is<PxRigidDynamic>()->setMass(39);
			Add(box);*/

			//pyramid = new Pyramid();
			//Add(pyramid);

			tBox = new triggerBox();
			tBox->Color(PxVec3(0, 0, 1));
			tBox->SetTrigger(true);

			callback = new MySimulationEventCallback();
			PxShape* shape = tBox->GetShape();
			shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			px_scene->setSimulationEventCallback(callback);

			Add(tBox);


			gPost = new RugbyGoalPost();
			Add(gPost);

			ball = new RugbyBall();
			ball->Color(PxVec3(0.4f, 0.2f, 0));
			ball->Get()->is<PxRigidDynamic>()->setGlobalPose(PxTransform(PxVec3(0, 6, -6.4f)));
			ball->Material(ballMat);
			Add(ball);

			fieldlines = new FieldLines();
			Add(fieldlines);

			outerlines = new OuterLines();
			Add(outerlines);

			castleTB = new Castle();
			castleTB->Color(PxVec3(0.4f, 0.4f, 0.4f));
			Add(castleTB);

			ssBase = new SeesawBase();
			Add(ssBase);

			ss = new Seesaw();
			Add(ss);

			/*sphere = new Sphere();
			Add(sphere);*/

			//cloth left top
			cloth = new Cloth(PxTransform(PxVec3(-36.f, 20.f, -7.f)), PxVec2(6.f, 6.f), 40, 40);
			cloth->Color(PxVec3(1,0,0));
			Add(cloth);        

			//cloth right top
			cloth = new Cloth(PxTransform(PxVec3(30.f, 20.f, -7.f)), PxVec2(6.f, 6.f), 10, 10);
			cloth->Color(PxVec3(0, 0, 1));
			Add(cloth);

		}

		//Custom udpate function
		virtual void CustomUpdate() 
		{

		}


		virtual void spawnBox() {
			if (boxSpawned == false) {
				box = new Box();
				box->Color(color_palette[0]);
				box->Get()->is<PxRigidDynamic>()->setGlobalPose(PxTransform(PxVec3(0, 85, -28.0f)));
				box->Get()->is<PxRigidDynamic>()->setMass(25);
				Add(box);
			}
		}


	};
}
