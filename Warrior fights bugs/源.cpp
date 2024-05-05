/*

开发日志：目前没有把玩家操纵地角色地class封装好，其次是子弹地运行逻辑有问题，
Enemy类中的GetPosition函数没写


项目名称：Warrior fights bugs
项目周期：5周
项目制作原则：

关于游戏引擎的基本框架：
初始化（）
主循环{
读取操作（）；
处理数据（）；
绘制画面（）；
解耦合思想，使读取和处理数据的逻辑与画面渲染分离。
}
释放资源（）；
*/

//编码原则，先搭建框架，代码细节用注释代替。


#include <graphics.h>
#include <windows.h>
#include <string>


// 子弹类
class Bullet
{
public:
	POINT position{ 0, 0 };

public:
	Bullet() = default;
	~Bullet() = default;

	void Draw() const
	{
		setlinecolor(RGB(255, 155, 50));
		setfillcolor(RGB(200, 75, 10));
		fillcircle(position.x, position.y, RADIUS);
	}

private:
	const int RADIUS = 10;
};

class Enemy 
{
public:
	Enemy()
	{
		loadimage(&img_shadow, _T("img/shadow_enemy.png"));
		anim_left = new Animation(_T("img/enemy_left_%d.png"), 3, 45);
		anim_right = new Animation(_T("img/enemy_left_%d.png"), 3, 45);

	}
	bool CheckBulletCollision(const Bullet& Bullet)
	{
		return false;
	}

	bool CheckPlayerCollision(const Player& player)
	{
		return false;
	}
	/*
	有意识地给参数加const，可以预防参数在
	函数间传递时，被修改以导致错误
	*/
	void Move(const Player& player) 
	{
		const POINT& player_postion = player.GetPosition();
		int dir_x = player_position.x - position, x;
		int dir_y = player_position.y - position.y;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += (int)(SPEED * normalized_x);
			postion.y += (int)(SPEED * normalized_y);
		}
	}
	
	~Enemy() //析构函数，用来释放动态分配的内存
	{
		delete anmi_left;
		delete anmi_right;
	}

private:
	const int SPEED = 2;
	const int FREAME_WIDTH = 80;
	const int FRAME_HEIGHT = 80;
	const int SHADOW_WIDTH = 48;
}

private:

	IMAGE img_shadow;
	Animation* anmi_left;
	Animation* anmi_right;
	POINT postion = { 0, 0 };
	bool facing_left = false;




int idx_current_anim = 0;//存储当前帧

const int PLAYER_ANIM_NUM = 3;
const int PLAYER_SPEED = 3;

IMAGE img_player_left[PLAYER_ANIM_NUM];//朝左的动画帧
IMAGE img_player_right[PLAYER_ANIM_NUM];//朝左的动画帧
POINT player_pos = { 500 , 500 };


#pragma comment(lib, "MSIMG32.LIB")

//这个没必要深究，只是封装了一个检测角色透明部分的加载图片函数
inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255, AC_SRC_ALPHA });

}

void LoadAnimation() //这里将需要的动画帧加载
{
	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++)
	{
		std::wstring path = L"img/player_left_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_left[i], path.c_str());
	}

	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++)
	{
		std::wstring path = L"img/player_right_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_right[i], path.c_str());
	}
}

int main()
{
	initgraph(1280, 720);
	LoadAnimation();

	bool running = true;
	bool turnning = true;

	ExMessage msg;
	IMAGE img_background;

	//虫子引入的逻辑需要改
	IMAGE img_enemy;

	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;


	loadimage(&img_background, _T("img/background.png")); //加载背景
	loadimage(&img_enemy, _T("img/enemy_right_1.png"));
	BeginBatchDraw();

	while (running)
	{
		DWORD start_time = GetTickCount();
		/*
		这里将程序刚开始运行的时长记录在start——time中，方便后期进行动态延时
		GetTickCount 是用来获取程序当前运行的时长
		*/
		while (peekmessage(&msg))
		{
			if (msg.message == WM_KEYDOWN)
			{
				switch (msg.vkcode)
				{
				case VK_UP:
					is_move_up = true;
					break;
				case VK_DOWN:
					is_move_down = true;
					break;
				case VK_LEFT:
					is_move_left = true;
					turnning = false;
					break;
				case VK_RIGHT:
					is_move_right = true;
					turnning = true;
					break;
				}
			}
			
			if (msg.message == WM_KEYUP)
			{
				switch (msg.vkcode)
				{
				case VK_UP:
					is_move_up = false;
					break;
				case VK_DOWN:
					is_move_down = false;
					break;
				case VK_LEFT:
					is_move_left = false;
					break;
				case VK_RIGHT:
					is_move_right = false;
					break;
				}
			}
		}

		if (is_move_up)
		{
			player_pos.y -= PLAYER_SPEED;
		}
		else if (is_move_down)
		{
			player_pos.y += PLAYER_SPEED;
		}
		else if (is_move_left)
		{
			player_pos.x -= PLAYER_SPEED;
		}
		else if (is_move_right)
		{
			player_pos.x += PLAYER_SPEED;
		}

		static int counter = 0; //记录播放了几个游戏帧
		if (++counter % 5 == 0)// 每五个游戏帧切换一个动画帧
		{
			idx_current_anim++;
		}
		//使动画循环播放，？？
		idx_current_anim = idx_current_anim % PLAYER_ANIM_NUM;
		cleardevice();
		
		putimage(0, 0, &img_background);
		//虫子渲染逻辑只是暂时的
		putimage_alpha(100, 100, &img_enemy);
		putimage_alpha(200, 250, &img_enemy);
		putimage_alpha(150, 200, &img_enemy);
		putimage_alpha(300, 60, &img_enemy);
		
		//判定角色的移动方向
		if (turnning == false)
		{
			putimage_alpha(player_pos.x, player_pos.y, &img_player_left[idx_current_anim]);
		}
		else if (turnning == true)
		{
			putimage_alpha(player_pos.x, player_pos.y, &img_player_right[idx_current_anim]);
		}
		
		//渲染部分

		FlushBatchDraw();

		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < 1000 / 144)
		{
			Sleep(1000 / 144 - delta_time);
		}
		/*
	关于这里为什么要加动态延时，以确保游戏按确定的帧率进行更新
	在游戏开发中，确保游戏按照一定的帧率（通常以每秒帧数 FPS 表示）运行是非常重要的。这段代码可以用于实现帧率控制，确保游戏在不同的硬件上都以相似的速度运行，从而提供更一致的游戏体验。

	具体来说，这段代码可以用于以下几个方面：

	游戏主循环控制： 将这段代码放在游戏主循环中，确保每次循环迭代的时间间隔接近目标帧率。这样可以控制游戏的更新速度，使其在不同的硬件上保持相对一致的性能。

	游戏物理模拟： 在游戏中，物理模拟通常需要在固定的时间步长内进行计算，以确保物理模拟的稳定性和一致性。这段代码可以帮助控制物理模拟的更新频率，以满足游戏需求。

	动画控制： 游戏中的动画通常需要以固定的帧率播放，以保证流畅性和一致性。通过控制动画更新的时间间隔，可以确保动画播放的速度稳定，并与游戏的帧率保持一致。

	总的来说，这段代码可以帮助游戏开发者控制游戏的运行速度和帧率，从而提供更好的游戏体验。
		*/
	}	
	

	EndBatchDraw();

	return 0;
}