#pragma once

#include "PhysicsEngine.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	///Plane class
	class Plane : public StaticActor
	{
	public:
		//A plane with default paramters: XZ plane centred at (0,0,0)
		Plane(PxVec3 normal = PxVec3(0.f, 1.f, 0.f), PxReal distance = 0.f)
			: StaticActor(PxTransformFromPlaneEquation(PxPlane(normal, distance)))
		{
			CreateShape(PxPlaneGeometry());
		}
	};

	///Sphere class
	class Sphere : public DynamicActor
	{
	public:
		//a sphere with default parameters:
		// - pose in 0,0,0
		// - dimensions: 1m
		// - denisty: 1kg/m^3
		Sphere(const PxTransform& pose = PxTransform(0,4,26), PxReal radius = 1.f, PxReal density = .5f)
			: DynamicActor(pose)
		{
			CreateShape(PxSphereGeometry(radius), density);
		}
	};

	///Box class
	class Box : public DynamicActor
	{
	public:
		//a Box with default parameters:
		// - pose in 0,0,0
		// - dimensions: 1m x 1m x 1m
		// - denisty: 1kg/m^3
		Box(const PxTransform& pose = PxTransform(PxIdentity), PxVec3 dimensions = PxVec3(1.3f, 1.3f, 1.3f), PxReal density = 1.0f)
			: DynamicActor(pose)
		{
			CreateShape(PxBoxGeometry(dimensions), density);

			//GetShape(0)->setLocalPose(PxTransform(PxVec3(0, 10, 13.0f)));
		}

	};


	/// trigger Box class
	class triggerBox : public StaticActor
	{
	public:

		triggerBox(const PxTransform& pose = PxTransform(0,16.9f,-45), PxVec3 dimensions = PxVec3(5.2f, 10.5f, 0.5f), PxReal density = 1.0f)
			: StaticActor(pose)
		{
			CreateShape(PxBoxGeometry(dimensions), density);

		}

	};

	class blockerBox : public StaticActor
	{
	public:

		blockerBox(const PxTransform& pose = PxTransform(0, 13, -40), PxVec3 dimensions = PxVec3(5.6f, 11.5f, 0.5f), PxReal density = 1.0f)
			: StaticActor(pose)
		{
			CreateShape(PxBoxGeometry(dimensions), density);

		}

	};

	class Capsule : public DynamicActor
	{
	public:
		Capsule(const PxTransform& pose = PxTransform(PxIdentity), PxVec2 dimensions = PxVec2(1.f, 1.f), PxReal density = 1.f)
			: DynamicActor(pose)
		{
			CreateShape(PxCapsuleGeometry(dimensions.x, dimensions.y), density);
		}
	};


	///The ConvexMesh class
	class ConvexMesh : public DynamicActor
	{
	public:
		//constructor
		ConvexMesh(const std::vector<PxVec3>& verts, const PxTransform& pose = PxTransform(PxIdentity), PxReal density = 1.f)
			: DynamicActor(pose)
		{
			PxConvexMeshDesc mesh_desc;
			mesh_desc.points.count = (PxU32)verts.size();
			mesh_desc.points.stride = sizeof(PxVec3);
			mesh_desc.points.data = &verts.front();
			mesh_desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
			mesh_desc.vertexLimit = 256;

			CreateShape(PxConvexMeshGeometry(CookMesh(mesh_desc)), density);
		}

		//mesh cooking (preparation)
		PxConvexMesh* CookMesh(const PxConvexMeshDesc& mesh_desc)
		{
			PxDefaultMemoryOutputStream stream;

			if (!GetCooking()->cookConvexMesh(mesh_desc, stream))
				throw new Exception("ConvexMesh::CookMesh, cooking failed.");

			PxDefaultMemoryInputData input(stream.getData(), stream.getSize());

			return GetPhysics()->createConvexMesh(input);
		}
	};

	///The TriangleMesh class
	class TriangleMesh : public StaticActor
	{
	public:
		//constructor
		TriangleMesh(const std::vector<PxVec3>& verts, const std::vector<PxU32>& trigs, const PxTransform& pose = PxTransform(PxIdentity))
			: StaticActor(pose)
		{
			PxTriangleMeshDesc mesh_desc;
			mesh_desc.points.count = (PxU32)verts.size();
			mesh_desc.points.stride = sizeof(PxVec3);
			mesh_desc.points.data = &verts.front();
			mesh_desc.triangles.count = (PxU32)trigs.size() / 3;
			mesh_desc.triangles.stride = 3 * sizeof(PxU32);
			mesh_desc.triangles.data = &trigs.front();

			CreateShape(PxTriangleMeshGeometry(CookMesh(mesh_desc)));
		}

		//mesh cooking (preparation)
		PxTriangleMesh* CookMesh(const PxTriangleMeshDesc& mesh_desc)
		{
			PxDefaultMemoryOutputStream stream;

			if (!GetCooking()->cookTriangleMesh(mesh_desc, stream))
				throw new Exception("TriangleMesh::CookMesh, cooking failed.");

			PxDefaultMemoryInputData input(stream.getData(), stream.getSize());

			return GetPhysics()->createTriangleMesh(input);
		}
	};

		// GOAL POST
	class RugbyGoalPost : public StaticActor
	{
	public:

		RugbyGoalPost(const PxTransform& pose = PxTransform(PxIdentity), PxVec3 dimensions = PxVec3(0.5f, 26.0f, 0.5f), PxReal density = 1.f) // rugby union post. 26/2 = 13m high
			: StaticActor(pose)
		{
			for (unsigned int i = 0; i < 3; i++)
			{
				CreateShape(PxBoxGeometry(dimensions), density);

				if (i == 1) {
					dimensions = PxVec3(5.6f, 0.3f, .5f); //2.8 * 2 = 5.6m crossbar 
				}
			}

			// Set Shape Transform 

			// Left
			GetShape(0)->setLocalPose(PxTransform(PxVec3(-5.6f, 1.f, -45.0f)));
			// Right
			GetShape(1)->setLocalPose(PxTransform(PxVec3(5.6f, 1.f, -45.0f)));

			// Horizontal (cross bar)
			GetShape(2)->setLocalPose(PxTransform(PxVec3(0.0f, 6.0f, -45.0f)));
		}
	};


		//Rugby Ball
		class RugbyBall : public DynamicActor
		{
		public:

			RugbyBall(const PxTransform& pose = PxTransform(0,4,26), PxReal radius = 0.7f, PxReal density = 0.6f)
				: DynamicActor(pose) {

				for (int i = 0; i < 5; i++)
				{
					if (i != 0) {
						radius = 0.5f;
					}

					if (i != 0 and i != 1 and i != 2) {
						radius = 0.3f;
					}

					CreateShape(PxSphereGeometry(radius), density);
				}

				//middle ball
				//GetShape(0)->setLocalPose(PxTransform(PxVec3(0, 1, 3)));

				//first outer balls
				//GetShape(1)->setLocalPose(PxTransform(PxVec3(0.5f, 1, 3)));
				//GetShape(2)->setLocalPose(PxTransform(PxVec3(-0.5f, 1, 3)));
				GetShape(1)->setLocalPose(PxTransform(PxVec3(0.5f, 0, 0)));
				GetShape(2)->setLocalPose(PxTransform(PxVec3(-0.5f, 0, 0)));

				//second outer balls
				//GetShape(3)->setLocalPose(PxTransform(PxVec3(0.85f, 1, 3)));
				//GetShape(4)->setLocalPose(PxTransform(PxVec3(-0.85f, 1, 3)));
				GetShape(3)->setLocalPose(PxTransform(PxVec3(0.85f, 0, 0)));
				GetShape(4)->setLocalPose(PxTransform(PxVec3(-0.85f, 0, 0)));
			}
		};

			// Field Lines
			class FieldLines : public StaticActor
			{
			public:

				FieldLines(const PxTransform& pose = PxTransform(PxIdentity), PxVec3 dimensions = PxVec3(20.0f, 1.0f, .5f), PxReal density = 1.f)
					: StaticActor(pose)
				{
					for (unsigned int i = 0; i < 7; i++)
					{
						CreateShape(PxBoxGeometry(dimensions), density);

					}

					// Set Shape Transform 

					GetShape(0)->setLocalPose(PxTransform(PxVec3(0.0f, -0.9f, -50.0f)));

					//+10 each
					GetShape(1)->setLocalPose(PxTransform(PxVec3(0.0f, -0.9f, -30.0f)));

					GetShape(2)->setLocalPose(PxTransform(PxVec3(0.0f, -0.9f, -20.0f)));

					GetShape(3)->setLocalPose(PxTransform(PxVec3(0.0f, -0.9f, -10.0f)));

					GetShape(5)->setLocalPose(PxTransform(PxVec3(0.0f, -0.9f, 0.0f)));

					GetShape(6)->setLocalPose(PxTransform(PxVec3(0.0f, -0.9f, 10.0f)));

					//try -5
					GetShape(4)->setLocalPose(PxTransform(PxVec3(0, -0.9f, -45)));

				}
			};

			// outer Lines
			class OuterLines : public StaticActor
			{
			public:

				OuterLines(const PxTransform& pose = PxTransform(PxIdentity), PxVec3 dimensions = PxVec3(0.5f, 1.0f, 30.0f), PxReal density = 1.f)
					: StaticActor(pose)
				{
					for (unsigned int i = 0; i < 2; i++)
					{
						CreateShape(PxBoxGeometry(dimensions), density);

					}

					// Set Shape Transform 
					GetShape(0)->setLocalPose(PxTransform(PxVec3(-20.0f, -0.9f, -20.0f)));
					GetShape(1)->setLocalPose(PxTransform(PxVec3(20.0f, -0.9f, -20.0f)));

				}
			};


			class Castle : public StaticActor
			{
			public:
				Castle(const PxTransform& pose = PxTransform(PxVec3(0.f, 0.f, -23.f)), PxVec3 dimensions = PxVec3(40.0f, 7.0f, 90.0f), PxReal density = 1.f)
					: StaticActor(pose)
				{
					// Create main walls
					CreateShape(PxBoxGeometry(dimensions.x, dimensions.y, 1.0f), density);  // front wall
					CreateShape(PxBoxGeometry(dimensions.x, dimensions.y, 1.0f), density);  // back wall
					CreateShape(PxBoxGeometry(1.0f, dimensions.y, dimensions.z), density);  // left wall
					CreateShape(PxBoxGeometry(1.0f, dimensions.y, dimensions.z), density);  // right wall

					// Create towers
					CreateShape(PxBoxGeometry(5.0f, 20.0f, 5.0f), density);  // tower 1
					CreateShape(PxBoxGeometry(5.0f, 20.0f, 5.0f), density);  // tower 2
					CreateShape(PxBoxGeometry(5.0f, 20.0f, 5.0f), density);  // tower 3
					CreateShape(PxBoxGeometry(5.0f, 20.0f, 5.0f), density);  // tower 4


					// Set shape transforms
					GetShape(0)->setLocalPose(PxTransform(PxVec3(0.f, 0.f, dimensions.z - 0.5f)));  // front wall
					GetShape(1)->setLocalPose(PxTransform(PxVec3(0.f, 0.f, -dimensions.z + 0.5f)));  // back wall
					GetShape(2)->setLocalPose(PxTransform(PxVec3(-dimensions.x + 0.5f, 0.f, 0.f)));  // left wall
					GetShape(3)->setLocalPose(PxTransform(PxVec3(dimensions.x - 0.5f, 0.f, 0.f)));  // right wall

					GetShape(4)->setLocalPose(PxTransform(PxVec3(-dimensions.x + 7.5f, 1.0f, dimensions.z - 7.5f)));  // tower 1
					GetShape(5)->setLocalPose(PxTransform(PxVec3(dimensions.x - 7.5f, 1.0f, dimensions.z - 7.5f)));  // tower 2
					GetShape(6)->setLocalPose(PxTransform(PxVec3(-dimensions.x + 7.5f, 1.0f, -dimensions.z + 7.5f)));  // tower 3
					GetShape(7)->setLocalPose(PxTransform(PxVec3(dimensions.x - 7.5f, 1.0f, -dimensions.z + 7.5f)));  // tower 4

				}
			};

			class flagPole : public StaticActor
			{
			public:
				flagPole(const PxTransform& pose = PxTransform(PxVec3(0.f, 0.f, -20.f)), PxVec3 dimensions = PxVec3(40.0f, 7.0f, 90.0f), PxReal density = 1.f)
					: StaticActor(pose)
				{
					// Create main walls
					CreateShape(PxBoxGeometry(30.0f, 0.5f, 1.0f), density);  // front wall

					// Set shape transforms
					GetShape(0)->setLocalPose(PxTransform(PxVec3(0.f, 20.f, -dimensions.z + 0.5f)));  // front wall


				}
			};


			///seesaw BASE class
			class SeesawBase : public StaticActor
			{
			public:
				SeesawBase(const PxTransform& pose = PxTransform(0.0f,0.0f,-20.0f), PxVec3 dimensions = PxVec3(5.5f, 2.0f, 2.0f), PxReal density = 1.f)
					: StaticActor(pose)
				{
					CreateShape(PxBoxGeometry(dimensions), density);
				}
			};

			
			class Seesaw : public DynamicActor
			{
			public:

				Seesaw(const PxTransform& pose = PxTransform(0,5,-20), PxReal density = PxReal(1.0f))
					: DynamicActor(pose)
				{
					CreateShape(PxBoxGeometry(PxVec3(1.5f, 0.1f, 15.0f)), density);

					CreateShape(PxBoxGeometry(PxVec3(3.0f, 1.5f, 0.1f)), density);
					GetShape(1)->setLocalPose(PxTransform(PxVec3(0.0f, 0.5f, 15.0f), PxQuat(PxPi / 4, PxVec3(1.0f, 0.0f, 0.0f))));

					CreateShape(PxBoxGeometry(PxVec3(3.0f, 1.5f, 0.1f)), density);
					GetShape(2)->setLocalPose(PxTransform(PxVec3(0.0f, 2.5f, 15.0f), PxQuat(PxPi / 4, PxVec3(-1.0f, 0.0f, 0.0f))));
				}

			};

		

			//CLOTH CODE

			class Cloth : public Actor
			{
				PxClothMeshDesc mesh_desc;

			public:
				//constructor
				Cloth(PxTransform pose = PxTransform(PxIdentity), const PxVec2& size = PxVec2(1.f, 1.f), PxU32 width = 1, PxU32 height = 1, bool fix_top = true)
				{
					//prepare vertices
					PxReal w_step = size.x / width;
					PxReal h_step = size.y / height;

					PxClothParticle* vertices = new PxClothParticle[(width + 1) * (height + 1) * 4];
					PxU32* quads = new PxU32[width * height * 4];

					for (PxU32 j = 0; j < (height + 1); j++)
					{
						for (PxU32 i = 0; i < (width + 1); i++)
						{
							PxU32 offset = i + j * (width + 1);
							vertices[offset].pos = PxVec3(w_step * i, 0.f, h_step * j);
							if (fix_top && (j == 0)) //fix the top row of vertices
								vertices[offset].invWeight = 0.f;
							else
								vertices[offset].invWeight = 1.f;
						}

						for (PxU32 j = 0; j < height; j++)
						{
							for (PxU32 i = 0; i < width; i++)
							{
								PxU32 offset = (i + j * width) * 4;
								quads[offset + 0] = (i + 0) + (j + 0) * (width + 1);
								quads[offset + 1] = (i + 1) + (j + 0) * (width + 1);
								quads[offset + 2] = (i + 1) + (j + 1) * (width + 1);
								quads[offset + 3] = (i + 0) + (j + 1) * (width + 1);
							}
						}
					}

					//init cloth mesh description
					mesh_desc.points.data = vertices;
					mesh_desc.points.count = (width + 1) * (height + 1);
					mesh_desc.points.stride = sizeof(PxClothParticle);

					mesh_desc.invMasses.data = &vertices->invWeight;
					mesh_desc.invMasses.count = (width + 1) * (height + 1);
					mesh_desc.invMasses.stride = sizeof(PxClothParticle);

					mesh_desc.quads.data = quads;
					mesh_desc.quads.count = width * height;
					mesh_desc.quads.stride = sizeof(PxU32) * 4;

					//create cloth fabric (cooking)
					PxClothFabric* fabric = PxClothFabricCreate(*GetPhysics(), mesh_desc, PxVec3(0, -1, 0));

					//create cloth
					actor = (PxActor*)GetPhysics()->createCloth(pose, *fabric, vertices, PxClothFlags());
					//collisions with the scene objects
					((PxCloth*)actor)->setClothFlag(PxClothFlag::eSCENE_COLLISION, true);

					colors.push_back(default_color);
					actor->userData = new UserData(&colors.back(), &mesh_desc);
				}

				~Cloth()
				{
					delete (UserData*)actor->userData;
				}
			};


			//Distance joint with the springs switched on
			class DistanceJoint : public Joint
			{
			public:
				DistanceJoint(Actor* actor0, const PxTransform& localFrame0, Actor* actor1, const PxTransform& localFrame1)
				{
					PxRigidActor* px_actor0 = 0;
					if (actor0)
						px_actor0 = (PxRigidActor*)actor0->Get();

					joint = (PxJoint*)PxDistanceJointCreate(*GetPhysics(), px_actor0, localFrame0, (PxRigidActor*)actor1->Get(), localFrame1);
					joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);
					((PxDistanceJoint*)joint)->setDistanceJointFlag(PxDistanceJointFlag::eSPRING_ENABLED, true);
					Damping(1.f);
					Stiffness(1.f);
				}

				void Stiffness(PxReal value)
				{
					((PxDistanceJoint*)joint)->setStiffness(value);
				}

				PxReal Stiffness()
				{
					return ((PxDistanceJoint*)joint)->getStiffness();
				}

				void Damping(PxReal value)
				{
					((PxDistanceJoint*)joint)->setDamping(value);
				}

				PxReal Damping()
				{
					return ((PxDistanceJoint*)joint)->getDamping();
				}
			};


			///Revolute Joint
			class RevoluteJoint : public Joint
			{
			public:
				RevoluteJoint(Actor* actor0, const PxTransform& localFrame0, Actor* actor1, const PxTransform& localFrame1)
				{
					PxRigidActor* px_actor0 = 0;
					if (actor0)
						px_actor0 = (PxRigidActor*)actor0->Get();

					joint = PxRevoluteJointCreate(*GetPhysics(), px_actor0, localFrame0, (PxRigidActor*)actor1->Get(), localFrame1);
					joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);
				}

				void DriveVelocity(PxReal value)
				{
					//wake up the attached actors
					PxRigidDynamic* actor_0, * actor_1;
					((PxRevoluteJoint*)joint)->getActors((PxRigidActor*&)actor_0, (PxRigidActor*&)actor_1);
					if (actor_0)
					{
						if (actor_0->isSleeping())
							actor_0->wakeUp();
					}
					if (actor_1)
					{
						if (actor_1->isSleeping())
							actor_1->wakeUp();
					}
					((PxRevoluteJoint*)joint)->setDriveVelocity(value);
					((PxRevoluteJoint*)joint)->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
				}

				PxReal DriveVelocity()
				{
					return ((PxRevoluteJoint*)joint)->getDriveVelocity();
				}

				void SetLimits(PxReal lower, PxReal upper)
				{
					((PxRevoluteJoint*)joint)->setLimit(PxJointAngularLimitPair(lower, upper));
					((PxRevoluteJoint*)joint)->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
				}
			};


			//

}