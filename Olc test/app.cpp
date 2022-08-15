#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <random>

std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_real_distribution<float> distr(1, 10);
std::uniform_real_distribution<float> vectorGen(-1, 1);

struct Ball
{
	olc::Pixel color = olc::Pixel(rand() % 256, rand() % 256, rand() % 256);
	int32_t radius = rand() % 15;
	olc::vf2d pos;
	olc::vf2d vBallDir = { vectorGen(eng),vectorGen(eng) };
	float vel;
	float fTime;
};

class MyGame : public olc::PixelGameEngine
{
private:
	std::vector<Ball> balls;

public:
	MyGame()
	{
		sAppName = "Bouncing Balls";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		addBall();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::WHITE);
		SetPixelMode(olc::Pixel::MASK); // Dont draw pixels which have any transparency
		if (GetKey(olc::Key::TAB).bPressed)
		{
			ConsoleShow(olc::Key::ESCAPE);
		}
		drawBalls(fElapsedTime);
		SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels
		return true;
	}


	void drawBalls(float fElapsedTime) {

		for (size_t i = 0; i < balls.size(); i++)
		{
			Ball* b = &balls[i];
			if (willCollideX(*b, fElapsedTime))
			{
				b->vBallDir.x *= -1;
			}
			if (willCollideY(*b, fElapsedTime))
			{
				b->vBallDir.y *= -1;
			}
			b->pos = b->pos + (b->vBallDir * b->vel * fElapsedTime);
			FillCircle(b->pos, b->radius, b->color);
		}
	}

	bool OnConsoleCommand(const std::string& sCommand) override {
		if (sCommand.compare("help") == 0) {
			println("Here are all commands:");
		}
		if (sCommand.compare("exit") == 0) {
			exit(0);
		}
		if (sCommand.rfind("add", 0) == 0) {
			size_t index = sCommand.find(" ");
			if (index != std::string::npos)
			{
				println("adding balls");
				int amount = std::stoi(sCommand.substr(index));
				for (size_t i = 0; i < amount; i++)
				{
					addBall();
				}
				std::string sAmount = sCommand.substr(index);
				println("Added" + sAmount + " balls");
			}
			else {
				addBall();
				println("Adding ball");
			}
		}
		if (sCommand.rfind("rm", 0) == 0) {
			size_t index = sCommand.find(" ");
			size_t removed;
			if (index != std::string::npos)
			{
				int amount = std::stoi(sCommand.substr(index));
				removed = removeBall(amount);
			}
			else {
				removed = removeBall(1);
			}
			println("Removed " + std::to_string(removed) + " ball" + (removed != 1 ? "s" : ""));
		}

		if (sCommand.compare("count") == 0) {
			ConsoleOut() << "There are " << balls.size() << " Balls." << std::endl;
		}
		return true;
	}
	void println(std::string line) {
		ConsoleOut() << line << std::endl;
		std::cout << line << std::endl;
	}

	void addBall() {

		Ball b;
		b.pos = olc::vi2d(ScreenWidth() / 2, ScreenHeight() / 2);
		b.vel = distr(eng);
		balls.push_back(b);
	}

	size_t removeBall(size_t amount) {
		auto removed = 0;
		for (size_t i = 0; i < amount; i++)
		{
			if (balls.size() > 0)
			{
				balls.pop_back();
				removed = i + 1;
			}
			else {
				break;
			}
		}
		return removed;
	}

	bool willCollideY(const Ball ball, float fElapstedTime)
	{
		const olc::vi2d futurePos = ball.pos + (ball.vBallDir * ball.vel * fElapstedTime);
		return futurePos.y <= 0 || futurePos.y >= ScreenHeight();
	}

	bool willCollideX(const Ball ball, float fElapstedTime)
	{
		const olc::vi2d futurePos = ball.pos + (ball.vBallDir * ball.vel * fElapstedTime);
		return futurePos.x <= 0 || futurePos.x >= ScreenWidth();
	}

};



int main()
{
	MyGame game;
	if (game.Construct(256, 240, 3, 3, false))
		game.Start();
	return 0;

}