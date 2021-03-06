#include "MyWinHeader.h"
#include "Rhombus.h"
#include "Window.h"
#include "Keyboard.h"
#include "WindowToKeyboardPipe.h"
#include "SceneBoard.h"
#include "Triangle.h"
#include "Rhombus.h"
#include "AlternatingWobbleAnimation.h"
#include "SinusWave.h"
#include "TimeHelper.h"
#include "KeyboardMovementAnimation.h"
#include "MessageSource.h"
#include "CameraControl.h"
#include "Particle.h"
#include "ParticleManager.h"
#include "Eigen/Core.h"

#include <random>
#include <sstream>
#include <string>
#include <iostream>
#include <thread>

float Rand(bool negative = true)
{
	auto value = (float)std::rand() / (float)RAND_MAX;
	return value * (negative && std::rand() % 2 == 0 ? 1 : -1);
}

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	try
	{
		Window wnd(1024, 768, "My first custom window");
		auto msSource = std::make_shared<MessageSource>();
		
		sp<Keyboard> keyBoard = std::make_shared<Keyboard>(msSource);
		sp<Camera> camera = std::make_shared<Camera>();
		
		sp<CameraControl> controller = std::make_shared<CameraControl>(keyBoard, camera);
		auto sb = std::make_shared<SceneBoard>(wnd.GetWindowHandle());
		sp<AbstractShape> shape = std::make_shared<Rhombus>(Point(0, 0), 15, 15, Color(1.0f, Rand(), Rand(), Rand()));
		
		sb->AddShape(shape);
		
		auto man = std::make_shared<ParticleManager>(500,20000, sb);
		man->SetGlobalForce(Eigen::Vector2f(0, -0.01));
		auto part = std::make_shared<Particle>(Eigen::Vector2f(0, 0()),shape);
		part->SetVelocity(Eigen::Vector2f(0.0001f, 0.0f));
		man->AddParticle(part);
		
		
		bool continueRender = true;
		std::thread renderThread = std::thread([&continueRender, &sb, &camera, &man]()
			{
				while (continueRender)
				{
					man->Tick();
					sb->Tick();

					sb->Render(camera);
				}
			});
		
		msSource->Subscribe([&continueRender](WindowMessage mes) {
			if (mes.GetMessageType() == WM_QUIT)
			{
				continueRender = false;
			}
			});
		auto messageResult = msSource->MessageLoop();
		renderThread.join();
		return static_cast<int>(messageResult);
	}
	catch (const std::exception & e)
	{
		MessageBox(nullptr, e.what(), "Exception :'(", MB_OK | MB_ICONEXCLAMATION);
	}

	return EXIT_SUCCESS;
}
