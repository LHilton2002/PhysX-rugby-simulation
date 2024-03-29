#pragma once

#include "BasicActors.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <Windows.h>
#include "VisualDebugger.h"
#include <vector>
#include "Extras\Camera.h"
#include "Extras\Renderer.h"
#include "Extras\HUD.h"

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
						//cerr << "onTrigger::eNOTIFY_TOUCH_FOUND" << endl;
						cerr << "Well done, you scored" << endl;
						trigger = true;
					}
					//check if eNOTIFY_TOUCH_LOST trigger
					if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST)
					{
						//cerr << "onTrigger::eNOTIFY_TOUCH_LOST" << endl;
						trigger = false;
					}
				}
			}
		}


		virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
		{
		}

		virtual void onWake(PxActor** actors, PxU32 count)
		{
		}

		virtual void onSleep(PxActor** actors, PxU32 count)
		{
		}

		virtual void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
		{
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
		Pyramid* pyramid;
		blockerBox* blocker;
		flagPole* pole;
		DistanceJoint* joint;
		CannonBall* cannonBall;

		//store actors created to delete later on
		std::vector<Box*> boxesSpawned;
		std::vector<Sphere*> spheresSpawned;
		std::vector<CannonBall*>cballsSpawned;

		//triggers
		triggerBox* tBox;
		bool boxSpawned;
		MySimulationEventCallback* callback;

		//extra variables
		bool blockerSpawned = false;
		bool celebrationSpawned = false;


		
		//materials
		// https://www.engineeringtoolbox.com/friction-coefficients-d_778.html
		// https://hypertextbook.com/facts/2006/restitution.shtml
		PxMaterial* ballMaterial = CreateMaterial(1.16f, 0.65f, 0.828f); //rugby ball
		PxMaterial* grassMaterial = CreateMaterial(0.9f, 0.5f, 0.3f); //grass
		PxMaterial* postMaterial = CreateMaterial(0.65f, 0.42f, 0.597f); // steel post
		PxMaterial* castleMaterial = CreateMaterial(0.5f, 0.4f, 0.8f); //stone castle
		PxMaterial* cannonballMaterial = CreateMaterial(0.65f, 0.42f, 0.0f); //cannon ball material (steel based) 
		PxMaterial* glassMaterial = CreateMaterial(0.9f,0.4f,0.1f); //glass material
		PxMaterial* iceMaterial = CreateMaterial(0.1f, 0.02f, 0.2f); //ice material
		
	public:
		///A custom scene class
		void SetVisualisation()
		{
			px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

			//joint visualisation
			px_scene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
		}

		//Custom scene initialisation
		virtual void CustomInit() 
		{
			SetVisualisation();			

			GetMaterial()->setDynamicFriction(.2f);

			//grass
			plane = new Plane();
			plane->Color(PxVec3(0,0.3f,0));
			plane->Material(grassMaterial);
			Add(plane);


			/*box = new Box();
			box->Color(color_palette[0]);
			box->Get()->is<PxRigidDynamic>()->setGlobalPose(PxTransform(PxVec3(0, 85, -25.0f)));
			box->Get()->is<PxRigidDynamic>()->setMass(39);
			Add(box);*/

			//pyramid = new Pyramid();
			//Add(pyramid);

			//trigger box
			tBox = new triggerBox();
			tBox->Color(PxVec3(0.6f, 0.3f, 1));
			tBox->SetTrigger(true);

			callback = new MySimulationEventCallback();
			PxShape* shape = tBox->GetShape();
			shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			px_scene->setSimulationEventCallback(callback);

			Add(tBox);

			//goal post
			gPost = new RugbyGoalPost();
			gPost->Material(postMaterial);
			Add(gPost);


			//rugby ball
			ball = new RugbyBall();
			ball->Color(PxVec3(0.4f, 0.2f, 0));
			ball->Get()->is<PxRigidDynamic>()->setGlobalPose(PxTransform(PxVec3(0, 6, -34.5f)));
			ball->Material(ballMaterial);
			Add(ball);


			//field lines
			fieldlines = new FieldLines();
			fieldlines->Material(grassMaterial);
			Add(fieldlines);

			outerlines = new OuterLines();
			outerlines->Material(grassMaterial);
			Add(outerlines);

			//castle
			castleTB = new Castle();
			castleTB->Color(PxVec3(0.4f, 0.4f, 0.4f));
			castleTB->Material(castleMaterial);
			Add(castleTB);


			//see saw
			ssBase = new SeesawBase();
			SeesawBase* ssBase2;
			ssBase2 = new SeesawBase();
			Add(ssBase);
			Add(ssBase2);


			ss = new Seesaw();
			Add(ss);

			/*sphere = new Sphere();
			Add(sphere);*/

			/*//cloth left top
			Cloth* cloth_left_top = new Cloth(PxTransform(PxVec3(-26.0f, 20.f, -108.f)), PxVec2(6.f, 6.f), 40, 40);
			cloth_left_top->Color(PxVec3(1, 0, 0));
			Add(cloth_left_top);

			//cloth right top
			Cloth* cloth_right_top = new Cloth(PxTransform(PxVec3(20.f, 20.f, -108.f)), PxVec2(6.f, 6.f), 40, 40);
			cloth_right_top->Color(PxVec3(0, 0, 1));
			Add(cloth_right_top);*/

			//cloth pole
			pole = new flagPole;
			pole->Color(PxVec3(0.4f, 0.4f, 0.4f));
			Add(pole);

			//joint to hold see saw in place after kicks
			DistanceJoint* joint = new DistanceJoint(ssBase, PxTransform(PxVec3(0.0f, 5.0f, 0.0f)), ss, PxTransform(PxVec3(0.0f, 3.0f, 0.0f)));
			joint->Stiffness(10.0f);
			joint->Damping(20.0f);


		}

		//Custom udpate function
		virtual void CustomUpdate() 
		{
			//Trigger
			if (callback->trigger && !celebrationSpawned) {
				spawnCelebrationFlags();
				celebrationSpawned = true;
			}
		}


		virtual void spawnBox() {
			if (boxSpawned == false) {
				box = new Box();
				box->Color(PxVec3(1.0f,0.5f,0.0f));
				box->Get()->is<PxRigidDynamic>()->setGlobalPose(PxTransform(PxVec3(0, 85, -60.3f)));
				box->Get()->is<PxRigidDynamic>()->setMass(30);
				Add(box);
				boxesSpawned.push_back(box);
			}
		}

		virtual void despawnBricks() {
			for (auto box : boxesSpawned) {
				box->Get()->is<PxActor>()->release();
			}
			boxesSpawned.clear();
		}

		virtual void spawnBall() {
			//rugby ball
			ball->Get()->is<PxActor>()->release();
			ball = new RugbyBall();
			ball->Color(PxVec3(0.4f, 0.2f, 0));
			ball->Get()->is<PxRigidDynamic>()->setGlobalPose(PxTransform(PxVec3(0, 5, -40.0f)));
			ball->Material(ballMaterial);
			Add(ball);
		}

		virtual void spawnCelebrationFlags() {
			//cloth left top
			Cloth* cloth_left_top = new Cloth(PxTransform(PxVec3(-26.0f, 20.f, -108.f)), PxVec2(6.f, 6.f), 40, 40);
			cloth_left_top->Color(PxVec3(1, 0, 0));
			Add(cloth_left_top);

			//cloth right top
			Cloth* cloth_right_top = new Cloth(PxTransform(PxVec3(20.f, 20.f, -108.f)), PxVec2(6.f, 6.f), 40, 40);
			cloth_right_top->Color(PxVec3(0, 0, 1));
			Add(cloth_right_top);

			// Check if the celebration has already been triggered
			if (celebrationSpawned) {
				return;
			}
			celebrationSpawned = true;

			// Generate random positions and colors for the spheres
			const PxVec3 spawn(0.0f, 50.0f, -200.0f);
			std::vector<PxVec3> positions(100);
			std::vector<PxVec3> colors(100);
			for (int i = 0; i < 100; i++) {
				float x = ((float)rand() / RAND_MAX);
				float y = ((float)rand() / RAND_MAX);
				float z = ((float)rand() / RAND_MAX);
				PxTransform transform(PxVec3(x, y, z));

				positions[i] = spawn + PxVec3(x, y, z);
				colors[i] = PxVec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
			}

			// Spawn the spheres
			for (int i = 0; i < 100; i++) {
				Sphere* sphere = new Sphere(PxTransform(positions[i]));
				sphere->Color(colors[i]);
				Add(sphere);
				spheresSpawned.push_back(sphere);
			}
		}

		virtual void despawnCBalls() {
			for (auto sphere : spheresSpawned) {
				sphere->Get()->is<PxActor>()->release();
			}
			spheresSpawned.clear();
		}

		void toggleBlocker() {
			if (blockerSpawned ) {
				blocker->Get()->is<PxActor>()->release();
				blockerSpawned = false;
			}
			else {
				blocker = new blockerBox();
				blocker->Color(PxVec3(1, 0, 0));
				Add(blocker);
				blockerSpawned = true;
			}
		}


		virtual void spawnCannonBallBlocker() {
			cannonBall = new CannonBall();
			cannonBall->Color(PxVec3(0, 0, 0));
			cannonBall->Material(cannonballMaterial);
			Add(cannonBall);
			cballsSpawned.push_back(cannonBall);
		}

		virtual void despawncannonBalls() {
			for (auto cannonBall : cballsSpawned) {
				cannonBall->Get()->is<PxActor>()->release();
			}
			cballsSpawned.clear();
		}

		virtual void planeMatGlass() {
			plane->Color(PxVec3(0.9f, 1.0f, 0.9f));
			plane->Material(glassMaterial);
		}

		virtual void planeMatIce() {
			plane->Color(PxVec3(0.2f, 0.0f, 1.0f));
			plane->Material(iceMaterial);
		}

		virtual void planeMatGrass() {
			plane->Color(PxVec3(0, 0.3f, 0));
			plane->Material(grassMaterial);
		}
	};
}
