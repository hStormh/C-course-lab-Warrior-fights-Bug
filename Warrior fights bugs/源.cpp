// hover 悬停

/*
**注意改名

项目名称：Warrior fights boars
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
#include <vector> //标准模板库的vector容器
#include <easyx.h>
#include <tchar.h> //用了_T宏

#pragma comment(lib, "Winmm.LIB")
#pragma comment(lib, "MSIMG32.LIB")

//这个没必要深究，只是封装了一个检测角色透明部分的加载图片函数
inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255, AC_SRC_ALPHA });

}


//动画类，用来封装动画相关逻辑和数据

/*class Button
{
public:
	Button()
	{
		region = 
	}
	~Button() = default;


private:
	RECT region;
	IMAGE img_idle;
	IMAGE img_hovered;
	IMAGE img_pushed;

	enum class Status
	{
		Idle = 0,
		Hovered = 0,
		Pushed
	};
};*/

class Animation
{
public:
	Animation(LPCTSTR path,int num, int interval)
	{
		interval_ms = interval;

		TCHAR path_file[256];
		for (size_t i = 0; i < num; i++)// for循环加载图片
		{
			_stprintf_s(path_file, path, i);

			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list.push_back(frame); //将每个已载入的图片都置入帧列表
		}
	}

	~Animation()  //动画对象的析构函数
	{
		for (size_t i = 0; i < frame_list.size(); i++)
		{
			delete frame_list[i];
		}
	}

	//這個Play方法，每次調用只渲染一幀
	void Play(int x, int y, int delta) 
	{
		/*在更新人物动画的逻辑中用计时器比用
		计数器更可靠，因为计数器会因为画面刷新
		速度加快，动画播放越快，造成不稳定*/

		timer += delta; //每次調用都叠加一個游戲幀所用時間
		if (timer >= interval_ms)
		{
			idx_frame = (idx_frame + 1) % frame_list.size(); //作取余是让帧索引循环变化
			timer = 0;
		}
		putimage_alpha(x, y, frame_list[idx_frame]);
	}

private: // 一个小点， 类或结构体成员方法要使用的变量，如果不是传参给出的，请一定要在成员变量中写出
	int timer = 0; //ani 计时器
	int idx_frame = 0; //动画帧索引
	int interval_ms = 0;
	std::vector<IMAGE*>frame_list;

};

//玩家动画实例化
Animation anim_left_player(_T("img/player_left_%d.png"), 3, 45);
Animation anim_right_player(_T("img/player_right_%d.png"), 3, 45);
//野猪动画实例化

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

class Player
{
public:
	
	const int FRAME_WIDTH = 80;//人物边框的宽度
	const int FRAME_HEIGHT = 80;//人物边框的高度
	Player()
	{
		loadimage(&img_shadow, _T("img/shadow_player.png"));
		anim_left = new Animation(_T("img/player_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("img/player_right_%d.png"), 6, 45);

	}

	~Player()
	{
		delete anim_left;
		delete anim_right;
	}

	void ProcessEvent(const ExMessage& msg) //处理玩家操作信息
	{
		switch (msg.message)
		{
		case WM_KEYDOWN:
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
				break;
			case VK_RIGHT:
				is_move_right = true;
				break;

			}

		}
		break;

		case WM_KEYUP:
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
		break;
		}
	}

	void Move() //处理玩家移动
		{
		//保证角色以同一速度移动，解决了斜着走速度更快的操作
		//向量求和原理
		    int dir_x = is_move_right - is_move_left;
			int dir_y = is_move_down - is_move_up;
			double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
			if (len_dir != 0)
			{
				double normalized_x = dir_x / len_dir;
				double normalized_y = dir_y / len_dir;
				player_pos.x += (int)(PLAYER_SPEED * normalized_x);
				player_pos.y += (int)(PLAYER_SPEED * normalized_y);
			}
			//确保玩家在窗口内部

			if (player_pos.x < 0)player_pos.x = 0;
			if (player_pos.y < 0)player_pos.y = 0;
			if (player_pos.x + FRAME_WIDTH > WINDOW_WIDTH) player_pos.x = WINDOW_WIDTH - FRAME_WIDTH;
			if (player_pos.y + FRAME_HEIGHT > WINDOW_HEIGHT) player_pos.y = WINDOW_HEIGHT - FRAME_HEIGHT;
		}

	void Draw(int delta) //图像渲染
	{
		int pos_shadow_x = player_pos.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
		// 计算阴影应该绘制的水平位置
		int pos_shadow_y = player_pos.y + FRAME_HEIGHT - 8;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);
		
		static bool facing_left = false;
		int dir_x = is_move_right - is_move_left;
		if (dir_x < 0)
		{
			facing_left = true;
		}
		else if (dir_x > 0)
		{
			facing_left = false;
		}

		if (facing_left)
			anim_left_player.Play(player_pos.x, player_pos.y, delta);
		else
			anim_right_player.Play(player_pos.x, player_pos.y, delta);
	
	}

	//传输角色坐标，
	const POINT& GetPosition() const
	{
		return player_pos;
	}

private:
	const int PLAYER_SPEED = 3;
	const int SHADOW_WIDTH = 32;
	const int WINDOW_HEIGHT = 720;
	const int WINDOW_WIDTH = 1280;
	POINT player_pos = { 500 , 500 };//玩家位置   成员变量中没加这个变量
	 //这就变成了一个新的问题，一个类中要用到的一个变量，是只能通过传参和在类中单独定义成员变量来实现吗
	//不能在主函数中直接定义吗

private:
	IMAGE img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	POINT position = { 500, 500 };
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;
};


class Enemy
{
public:
	Enemy()
	{
		loadimage(&img_shadow, _T("img/shadow_enemy.png"));
		anim_left = new Animation(_T("img/enemy_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("img/enemy_right_%d.png"), 6, 45);

		//敌人生成边界, 利用枚举
		enum class SpawnEdge
		{
			Up = 0,
			Down,
			Left,
			Right
		};
		// 将敌人生成在地图外随机位置
		SpawnEdge edge = (SpawnEdge)(rand() % 4);
		switch (edge)
		/*
		逻辑是 在上下生成时，野猪的下边界紧贴地图上边界
		或野猪上边界紧贴地图下边界，而左右坐标任意
		*/
		{
		case SpawnEdge::Up:
			//position是野猪坐标
			position.x = rand() % WINDOW_WIDTH;
			position.y = -FRAME_HEIGHT;
			break;
		case SpawnEdge::Down:
			position.x = rand() % WINDOW_WIDTH;
			position.y = WINDOW_HEIGHT;
			break;
		case SpawnEdge::Left:
			position.x = -FRAME_WIDTH;
			position.y = rand() % WINDOW_HEIGHT;
			break;
		case SpawnEdge::Right:
			position.x = WINDOW_WIDTH;
			position.y = rand() % WINDOW_HEIGHT;
			break;
		}
	}
	
	
	bool CheckBulletCollision(const Bullet& bullet)
	{
		//将子弹等效为一个点，判断点是否在敌人矩形内
		bool is_overlap_x = bullet.position.x >= position.x && bullet.position.x <= position.x + FRAME_WIDTH;
		bool is_overlap_y = bullet.position.y >= position.y && bullet.position.y <= position.y + FRAME_HEIGHT;

		return is_overlap_x && is_overlap_y;
	}
	//检测玩家和野猪是否碰撞
	bool CheckPlayerCollision(const Player& player)
	{
		//这里接收了玩家实例
		const POINT& player_position = player.GetPosition();
		bool is_overlap_x = false;
		bool is_overlap_y = false;
		// 以敌人中心点和玩家矩形重叠作为碰撞检测的标准
		POINT check_position = { position.x + FRAME_WIDTH / 2, position.y + FRAME_HEIGHT / 2 };
		//检测敌人中心点是否在玩家矩形内
		is_overlap_x = check_position.x >= player_position.x && check_position.x <= player_position.x + FRAME_WIDTH;
		is_overlap_y = check_position.y >= player_position.y && check_position.y <= player_position.y + FRAME_HEIGHT;
	
		return is_overlap_x && is_overlap_y;
	}
	/*
	有意识地给参数加const，可以预防参数在
	函数间传递时，被修改以导致错误
	*/
	void Move(const Player& player)
	{	//接收角色xy坐标
		const POINT& player_position = player.GetPosition();
		int dir_x = player_position.x - position.x;
		int dir_y = player_position.y - position.y;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			//和Player类类似，使用了向量分解的逻辑
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += (int)(SPEED * normalized_x);
			position.y += (int)(SPEED * normalized_y);
		}
	}

	void Draw(int delta)
	{
		const Player player;
		
		int pos_shadow_x = position.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
		// 计算阴影应该绘制的水平位置
		int pos_shadow_y = position.y + FRAME_HEIGHT - 35;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);
		
		if (facing_left)
			anim_left->Play(position.x, position.y, delta);
		else
			anim_right->Play(position.x, position.y, delta);

	}

	~Enemy() //析构函数，用来释放动态分配的内存
	{
		delete anim_left;
		delete anim_right;
	}
	void Hurt()
	{
		//检测敌人是否受伤， 这里使用一击必杀
		alive = false;
	}

	bool CheckAlive()
	{
		//获取当前敌人的存活状态
		return alive;
	}


private:
	const int SPEED = 2;
	const int FRAME_WIDTH = 80;
	const int FRAME_HEIGHT = 80;
	const int SHADOW_WIDTH = 48;
	const int WINDOW_HEIGHT = 720;
	const int WINDOW_WIDTH = 1280;
	

private:

	IMAGE img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	POINT position = { 0, 0 };
	bool facing_left = false;
	bool alive = true;
};



int idx_current_anim = 0;//存储当前帧

const int PLAYER_ANIM_NUM = 3;
const int PLAYER_SPEED = 3;

IMAGE img_player_left[PLAYER_ANIM_NUM];//朝左的动画帧
IMAGE img_player_right[PLAYER_ANIM_NUM];//朝左的动画帧

POINT player_pos = { 500 , 500 };//玩家位置

#pragma comment(lib, "MSIMG32.LIB")


//敌人生成函数
void TryGenerateEnemy(std::vector<Enemy*>& enemy_list)//先接收一個vector容器指針
{
	const int INTERVAL = 100;
	static int counter = 0;// 静态变量，在調用函數之間，變量值始終不變，會保留上次結果
	if ((++counter) % INTERVAL == 0) //每100*100/144ms出一只猪
		enemy_list.push_back(new Enemy());
}

void UpdateBullets(std::vector<Bullet>& bullet_list, const Player& player)
{
	const double RADIAL_SPEED = 0.0045; //径向速度
	const double TANGENT_SPEED = 0.0055; //切向速度
	double radian_interval = 2 * 3.14159 / bullet_list.size();
	POINT player_position = player.GetPosition();
	double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);
	//遍历列表中的每一颗子弹，根据玩家当前的位置依次修改它们的位置
	for (size_t i = 0; i < bullet_list.size(); i++)
	{
		double radian = GetTickCount() * TANGENT_SPEED + radian_interval * i; //当前子弹的弧度
		bullet_list[i].position.x = player_position.x + player.FRAME_WIDTH / 2 + (int)(radius * cos(radian));
		bullet_list[i].position.y = player_position.y + player.FRAME_HEIGHT / 2 + (int)(radius * sin(radian));

	}
}

void DrawPlayerScore(int score)
{
	static TCHAR text[64];
	_stprintf_s(text, _T("当前玩家得分：%d"), score);

	settextstyle(50, 0, _T("Consolas"));
	setbkmode(TRANSPARENT);
	settextcolor(RGB(255, 85, 185));
	outtextxy(10, 10, text);
}


int main()
{
	initgraph(1280, 720);

	mciSendString(_T("open mus/bgm.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("open mus/hit.wav alias hit"), NULL, 0, NULL);
	//加载音乐

	mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
	bool running = true;

	int score = 0;
	Player player;
	ExMessage msg;
	IMAGE img_background;
	IMAGE img_shadow; //引入阴影
	IMAGE img_enemy;
	std::vector<Enemy*> enemy_list;
	std::vector<Bullet> bullet_list(1);// 定义了球的数量

	loadimage(&img_background, _T("img/background.png")); //加载背景
	
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;
	
	

	loadimage(&img_shadow, _T("img/shadow_player.png"));
	
	BeginBatchDraw();
	
	//LoadAnimation();
	while (running)
	{
		DWORD start_time = GetTickCount();
		/*
		这里将程序刚开始运行的时长记录在start——time中，方便后期进行动态延时
		GetTickCount 是用来获取程序当前运行的时长
		*/
		while (peekmessage(&msg))
		{
			player.ProcessEvent(msg);
		}

		player.Move();
		UpdateBullets(bullet_list, player);
		TryGenerateEnemy(enemy_list);
		for (Enemy* enemy : enemy_list)
		{
			enemy->Move(player);
		}
		
		
		// (游戏结束判定)遍历敌人vector容器，依次检测野猪是否与玩家相碰
		for (Enemy* enemy : enemy_list)
		{
			if (enemy->CheckPlayerCollision(player))
			{
				static TCHAR text[128];
				_stprintf_s(text, _T("最终得分：%d !!!"), score);
				MessageBox(GetHWnd(),text, _T("游戏结束, 已经结束啦"), MB_OK);
				running = false;
				break;
			}	

		}
		//检测子弹和野猪的碰撞
		for (Enemy* enemy : enemy_list)
		{
			for (const Bullet& bullet : bullet_list)
			{
				if (enemy->CheckBulletCollision(bullet))
				{
					enemy->Hurt();
					mciSendString(_T("setaudio hit volume to 10000"), NULL, 0, NULL);
					mciSendString(_T("play hit repeat from 0"), NULL, 0, NULL);
					score++;
				}
			}
		}

		//移除生命值归零的敌人
		for (size_t i = 0; i < enemy_list.size(); i++)
		{
			Enemy* enemy = enemy_list[i];
			if (!enemy->CheckAlive())
			{
				std::swap(enemy_list[i], enemy_list.back());
				enemy_list.pop_back();
				//这里删除敌人元素运用了组合技， 先swap将欲删除元素和顶部元素交换位置，再删顶部元素
				delete enemy;
			}
		}
		
		cleardevice();
		
		putimage(0, 0, &img_background);
		player.Draw(1000 / 144);
		for (Enemy* enemy : enemy_list)
		{
			//循环调用Draw函数
			enemy->Draw(1000 / 144);
		}
		for (const Bullet& bullet : bullet_list)
		{
			bullet.Draw();
		}

		DrawPlayerScore(score);

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